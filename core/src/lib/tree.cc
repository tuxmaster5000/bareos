/*
   BAREOS® - Backup Archiving REcovery Open Sourced

   Copyright (C) 2002-2012 Free Software Foundation Europe e.V.
   Copyright (C) 2013-2024 Bareos GmbH & Co. KG

   This program is Free Software; you can redistribute it and/or
   modify it under the terms of version three of the GNU Affero General Public
   License as published by the Free Software Foundation and included
   in the file LICENSE.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Affero General Public License for more details.

   You should have received a copy of the GNU Affero General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.
*/
/*
 * Directory tree build/traverse routines
 *
 * Kern Sibbald, June MMII
 */

#include "include/bareos.h"
#include "lib/tree.h"
#include "lib/util.h"
#include "lib/fnmatch.h"

#define B_PAGE_SIZE 4096
#define MAX_PAGES 2400
#define MAX_BUF_SIZE (MAX_PAGES * B_PAGE_SIZE) /* approx 10MB */

/* Forward referenced subroutines */
static tree_node* search_and_insert_tree_node(char* fname,
                                              tree_node_type type,
                                              TREE_ROOT* root,
                                              tree_node* parent);
template <typename T> static T* tree_alloc(TREE_ROOT* root, int size);

// NOTE !!!!! we turn off Debug messages for performance reasons.
#undef Dmsg0
#undef Dmsg1
#undef Dmsg2
#undef Dmsg3
#define Dmsg0(n, f)
#define Dmsg1(n, f, a1)
#define Dmsg2(n, f, a1, a2)
#define Dmsg3(n, f, a1, a2, a3)

// This subroutine gets a big buffer.
static void MallocBuf(TREE_ROOT* root, int size)
{
  struct s_mem* mem;

  mem = (struct s_mem*)malloc(size);
  root->total_size += size;
  root->blocks++;
  mem->next = root->mem;
  root->mem = mem;
  mem->mem = mem->first;
  mem->rem = (char*)mem + size - (char*)mem->mem;
  Dmsg2(200, "malloc buf size=%d rem=%d\n", size, mem->rem);
}

/*
 * Note, we allocate a big buffer in the tree root
 * from which we allocate nodes. This runs more
 * than 100 times as fast as directly using malloc()
 * for each of the nodes.
 */
TREE_ROOT* new_tree(int count)
{
  TREE_ROOT* root;
  uint32_t size;

  if (count < 1000) { /* minimum tree size */
    count = 1000;
  }
  root = static_cast<TREE_ROOT*>(malloc(sizeof(TREE_ROOT)));
  root = new (root) TREE_ROOT();

  // Assume filename + node  = 40 characters average length
  size = count * (BALIGN(sizeof(tree_node)) + 40);
  if (count > 1000000 || size > (MAX_BUF_SIZE / 2)) { size = MAX_BUF_SIZE; }
  Dmsg2(400, "count=%d size=%d\n", count, size);
  MallocBuf(root, size);
  root->cached_path_len = -1;
  root->cached_path = GetPoolMemory(PM_FNAME);
  root->type = tree_node_type::Root;
  root->fname = "";
  return root;
}

// Create a new tree node.
static tree_node* new_tree_node(TREE_ROOT* root)
{
  tree_node* node;
  int size = sizeof(tree_node);
  node = tree_alloc<tree_node>(root, size);
  node = new (node) tree_node();
  node->delta_seq = -1;
  return node;
}

// This routine can be called to release the previously allocated tree node.
static void FreeTreeNode(TREE_ROOT* root)
{
  int asize = BALIGN(sizeof(tree_node));
  root->mem->rem += asize;
  root->mem->mem = (char*)root->mem->mem - asize;
}

void TreeRemoveNode(TREE_ROOT* root, tree_node* node)
{
  int asize = BALIGN(sizeof(tree_node));
  node->parent->child.remove(node);
  if (((char*)root->mem->mem - asize) == (char*)node) {
    FreeTreeNode(root);
  } else {
    Dmsg0(0, "Can't release tree node\n");
  }
}

/*
 * Allocate bytes for filename in tree structure.
 * Keep the pointers properly aligned by allocating
 * sizes that are aligned.
 */
template <typename T> static T* tree_alloc(TREE_ROOT* root, int size)
{
  T* buf;
  int asize = BALIGN(size);

  if (root->mem->rem < asize) {
    uint32_t mb_size;
    if (root->total_size >= (MAX_BUF_SIZE / 2)) {
      mb_size = MAX_BUF_SIZE;
    } else {
      mb_size = MAX_BUF_SIZE / 2;
    }
    MallocBuf(root, mb_size);
  }
  root->mem->rem -= asize;
  buf = static_cast<T*>(root->mem->mem);
  root->mem->mem = (char*)root->mem->mem + asize;
  return buf;
}

// This routine frees the whole tree
void FreeTree(TREE_ROOT* root)
{
  struct s_mem *mem, *rel;

  std::destroy_at(&root->hardlinks);
  for (mem = root->mem; mem;) {
    rel = mem;
    mem = mem->next;
    free(rel);
  }
  if (root->cached_path) {
    FreePoolMemory(root->cached_path);
    root->cached_path = NULL;
  }
  free(root);
  return;
}

// Add Delta part for this node
void TreeAddDeltaPart(TREE_ROOT* root,
                      tree_node* node,
                      JobId_t JobId,
                      int32_t FileIndex)
{
  struct delta_list* elt
      = tree_alloc<delta_list>(root, sizeof(struct delta_list));

  elt->next = node->delta_list;
  elt->JobId = JobId;
  elt->FileIndex = FileIndex;
  node->delta_list = elt;
}

/*
 * Insert a node in the tree. This is the main subroutine called when building a
 * tree.
 */
tree_node* insert_tree_node(char* path,
                            char* fname,
                            tree_node_type type,
                            TREE_ROOT* root,
                            tree_node* parent)
{
  char *p, *q;
  int path_len = strlen(path);
  tree_node* node;

  Dmsg1(100, "insert_tree_node: %s\n", path);

  // If trailing slash on path, strip it
  if (path_len > 0) {
    q = path + path_len - 1;
    if (IsPathSeparator(*q)) {
      *q = 0; /* strip trailing slash */
    } else {
      q = NULL; /* no trailing slash */
    }
  } else {
    q = NULL; /* no trailing slash */
  }

  // If no filename, strip last component of path as "filename"
  if (*fname == 0) {
    p = (char*)last_path_separator(path); /* separate path and filename */
    if (p) {
      fname = p + 1; /* set new filename */
      *p = '\0';     /* Terminate new path */
    }
  } else {
    p = NULL;
  }

  if (*fname) {
    if (!parent) { /* if no parent, we need to make one */
      Dmsg1(100, "make_tree_path for %s\n", path);
      path_len = strlen(path); /* get new length */
      if (path_len == root->cached_path_len
          && bstrcmp(path, root->cached_path)) {
        parent = root->cached_parent;
      } else {
        root->cached_path_len = path_len;
        PmStrcpy(root->cached_path, path);
        parent = make_tree_path(path, root);
        root->cached_parent = parent;
      }
      Dmsg1(100, "parent=%s\n", parent->fname);
    }
  } else {
    fname = path;
    if (!parent) { parent = root; }
    Dmsg1(100, "No / found: %s\n", path);
  }

  node = search_and_insert_tree_node(fname, type, root, parent);

  if (q) {    /* if trailing slash on entry */
    *q = '/'; /*  restore it */
  }

  if (p) {    /* if slash in path trashed */
    *p = '/'; /* restore full path */
  }

  return node;
}

// Ensure that all appropriate nodes for a full path exist in the tree.
tree_node* make_tree_path(char* path, TREE_ROOT* root)
{
  tree_node *parent, *node;
  char *fname, *p;
  auto type = tree_node_type::NewDir;

  Dmsg1(100, "make_tree_path: %s\n", path);

  if (*path == 0) {
    Dmsg0(100, "make_tree_path: parent=*root*\n");
    return root;
  }

  // Get last dir component of path
  p = (char*)last_path_separator(path);
  if (p) {
    fname = p + 1;
    *p = 0; /* Terminate path */
    parent = make_tree_path(path, root);
    *p = '/'; /* restore full name */
  } else {
    fname = path;
    parent = root;
    type = tree_node_type::DirWin;
  }

  node = search_and_insert_tree_node(fname, type, root, parent);

  return node;
}

static int NodeCompare(void* item1, void* item2)
{
  tree_node* tn1 = (tree_node*)item1;
  tree_node* tn2 = (tree_node*)item2;

  if (tn1->fname[0] > tn2->fname[0]) {
    return 1;
  } else if (tn1->fname[0] < tn2->fname[0]) {
    return -1;
  }

  return strcmp(tn1->fname, tn2->fname);
}

static const char* copy_string(std::string_view str, TREE_ROOT* root)
{
  char* buf = tree_alloc<char>(root, str.size() + 1);
  memcpy(buf, str.data(), str.size());
  buf[str.size()] = '\0';
  return buf;
}

// See if the fname already exists. If not insert a new node for it.
static tree_node* search_and_insert_tree_node(char* fname,
                                              tree_node_type type,
                                              TREE_ROOT* root,
                                              tree_node* parent)
{
  tree_node *node, *found_node;

  node = new_tree_node(root);
  node->fname = fname;
  found_node = (tree_node*)parent->child.insert(node, NodeCompare);
  if (found_node != node) { /* already in list */
    FreeTreeNode(root);     /* free node allocated above */
    found_node->inserted = false;
    return found_node;
  }

  // It was not found, but is now inserted
  node->fname = copy_string(fname, root);
  node->parent = parent;
  node->type = type;

  // Maintain a linear chain of nodes
  if (!root->first) {
    root->first = node;
    root->last = node;
  } else {
    root->last->next = node;
    root->last = node;
  }

  node->inserted = true; /* inserted into tree */
  return node;
}

static void TreeGetpathItem(tree_node* node, POOLMEM*& path)
{
  if (!node) { return; }

  TreeGetpathItem(node->parent, path);

  /* Fixup for Win32. If we have a Win32 directory and
   * there is only a / in the buffer, remove it since
   * win32 names don't generally start with / */
  if (node->type == tree_node_type::DirWin && IsPathSeparator(path[0])
      && path[1] == '\0') {
    PmStrcpy(path, "");
  }
  PmStrcat(path, node->fname);

  /* Add a slash for all directories unless we are at the root,
   * also add a slash to a soft linked file if it has children
   * i.e. it is linked to a directory. */
  if ((node->type != tree_node_type::File
       && !(IsPathSeparator(path[0]) && path[1] == '\0'))
      || (node->soft_link && TreeNodeHasChild(node))) {
    PmStrcat(path, "/");
  }
}

POOLMEM* tree_getpath(tree_node* node)
{
  POOLMEM* path;

  if (!node) { return NULL; }

  // Allocate a new empty path.
  path = GetPoolMemory(PM_NAME);
  PmStrcpy(path, "");

  // Fill the path with the full path.
  TreeGetpathItem(node, path);

  return path;
}

// Change to specified directory
tree_node* tree_cwd(char* path, TREE_ROOT* root, tree_node* node)
{
  if (path[0] == '.' && path[1] == '\0') { return node; }

  // Handle relative path
  if (path[0] == '.' && path[1] == '.'
      && (IsPathSeparator(path[2]) || path[2] == '\0')) {
    tree_node* parent = node->parent ? node->parent : node;
    if (path[2] == 0) {
      return parent;
    } else {
      return tree_cwd(path + 3, root, parent);
    }
  }

  if (IsPathSeparator(path[0])) {
    Dmsg0(100, "Doing absolute lookup.\n");
    return tree_relcwd(path + 1, root, root);
  }

  Dmsg0(100, "Doing relative lookup.\n");

  return tree_relcwd(path, root, node);
}

// Do a relative cwd -- i.e. relative to current node rather than root node
tree_node* tree_relcwd(char* path, TREE_ROOT* root, tree_node* node)
{
  char* p;
  int len;
  tree_node* cd;
  char save_char;
  int match;

  if (*path == 0) { return node; }

  // Check the current segment only
  if ((p = first_path_separator(path)) != NULL) {
    len = p - path;
  } else {
    len = strlen(path);
  }

  Dmsg2(100, "tree_relcwd: len=%d path=%s\n", len, path);

  foreach_child (cd, node) {
    Dmsg1(100, "tree_relcwd: test cd=%s\n", cd->fname);
    if (cd->fname[0] == path[0] && len == (int)strlen(cd->fname)
        && bstrncmp(cd->fname, path, len)) {
      break;
    }

    // fnmatch has no len in call so we truncate the string
    save_char = path[len];
    path[len] = 0;
    match = fnmatch(path, cd->fname, 0) == 0;
    path[len] = save_char;

    if (match) { break; }
  }

  if (!cd || (cd->type == tree_node_type::File && !TreeNodeHasChild(cd))) {
    return NULL;
  }

  if (!p) {
    Dmsg0(100, "tree_relcwd: no more to lookup. found.\n");
    return cd;
  }

  Dmsg2(100, "recurse tree_relcwd with path=%s, cd=%s\n", p + 1, cd->fname);

  // Check the next segment if any
  return tree_relcwd(p + 1, root, cd);
}
