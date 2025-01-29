/*
   Copyright (C) 2024 tuxmaster5000 tuxmaster5000@googlemail.com

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Affero General Public License for more details.

   You should have received a copy of the GNU Affero General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "zfs-fd.h"
#include <new>
#include <stdexcept>
#include <errno.h>

/* ZFS header*/
#include <libzfs.h>

namespace filedaemon {


// Plugin Information block
static PluginInformation pluginInfo
    = {sizeof(pluginInfo), FD_PLUGIN_INTERFACE_VERSION,
       FD_PLUGIN_MAGIC,    PLUGIN_LICENSE,
       PLUGIN_AUTHOR,      PLUGIN_DATE,
       PLUGIN_VERSION,     PLUGIN_DESCRIPTION,
       PLUGIN_USAGE};

// Plugin entry points for Bareos
static PluginFunctions pluginFuncs
    = {sizeof(pluginFuncs), FD_PLUGIN_INTERFACE_VERSION,

       // Entry points into plugin
       newPlugin,  // new plugin instance
       freePlugin, // free plugin instance
       getPluginValue, setPluginValue, handlePluginEvent, startBackupFile,
       endBackupFile, startRestoreFile, endRestoreFile, pluginIO, createFile,
       setFileAttributes, checkFile, getAcl, setAcl, getXattr, setXattr};
/**
 * loadPlugin() and unloadPlugin() are called direct by the fd, so
 * it must have an pure C signature.
 */

#ifdef __cplusplus
extern "C" {
#endif

// When the fd will load it.
bRC loadPlugin( PluginApiDefinition* libinfo,
               CoreFunctions* lbareos_core_functions,
               PluginInformation** plugin_information,
               PluginFunctions** plugin_functions) {
  bareos_core_functions = lbareos_core_functions; // set Bareos funct pointers
  bareos_plugin_interface_version = libinfo;
  *plugin_information = &pluginInfo; // return pointer to our info
  *plugin_functions = &pluginFuncs;  // return pointer to our functions

  return bRC_OK;
}

// When the fd will unload it.
bRC unloadPlugin() {
  return bRC_OK;
}

#ifdef __cplusplus
}
#endif


// Create a new instance of the plugin i.e. allocate our private storage
static bRC newPlugin(PluginContext* ctx) {
  ZFSfdConfig* p_ctx = nullptr;
  try {
    p_ctx =  new ZFSfdConfig();
  }
  catch (const std::bad_alloc& e) {
     Jmsg(ctx, M_FATAL, "zfs-fd: unable to create the configuration object. %d\n", e.what());
    return bRC_Error;
  }
  ctx->plugin_private_context = (void*)p_ctx; // set our context pointer

  bareos_core_functions->registerBareosEvents(
      ctx, 6, bEventNewPluginOptions, bEventPluginCommand, bEventJobStart,
      bEventRestoreCommand, bEventEstimateCommand, bEventBackupCommand);

  return bRC_OK;        
} 
// Free a plugin instance, i.e. release our private storage
static bRC freePlugin(PluginContext* ctx) {
  Dmsg(ctx, debuglevel, "zfs-fd: calling freePlugin\n");  
  ZFSfdConfig* p_ctx = static_cast<ZFSfdConfig*>(ctx->plugin_private_context);
  if (!p_ctx) {
    Dmsg(ctx, debuglevel, NO_CONFIG_OBJECT_TEXT);
    return bRC_Error;
  }
  // free our private context.
  Dmsg(ctx, debuglevel, "cleanup libzfs\n");
  cleanup_libzfs(p_ctx);
  delete p_ctx;
  p_ctx = nullptr;
  return bRC_OK;
}
// Not used current in Bareos
static bRC getPluginValue(PluginContext* ctx, pVariable var, [[maybe_unused]] void* value) {
  Dmsg(ctx, debuglevel, "zfs-fd: calling getPluginValue for %s\n", (char*)var);
  return bRC_OK;
}
// Not used current in Bareos
static bRC setPluginValue(PluginContext* ctx, pVariable var, void* value) {
   Dmsg(ctx, debuglevel, "zfs-fd: calling setPluginValue for %s to %s\n", (char*)var, (char*)value);
  return bRC_OK;
}
// Handle an event that comes from Bareos
static bRC handlePluginEvent(PluginContext* ctx, bEvent* event, void* value) {
   Dmsg(ctx, debuglevel, "zfs-fd: calling handlePluginEvent for event number %d.\n", event->eventType);
  bRC retval = bRC_OK;
  ZFSfdConfig* p_ctx = static_cast<ZFSfdConfig*>(ctx->plugin_private_context);
  // No plug-in context -> something must be wrong.
  if (!p_ctx) {
    Dmsg(ctx, debuglevel, NO_CONFIG_OBJECT_TEXT);
    return bRC_Error;
  }
  switch (event->eventType) {
    case bEventJobStart:
      /*
       * Init the the zfs lib now.
       * Parsing the options will be done later.
      */
      Dmsg(ctx, debuglevel, "zfs-fd: Event-JobStart=%s\n", (char*)value);
      if (!init_libzfs(p_ctx)) {
	Dmsg(ctx, debuglevel, "unable init libzfs\n%s\n", libzfs_error_init(errno));
	Jmsg(ctx, M_FATAL, "zfs-fd: unable init libzfs\n%s\n", libzfs_error_init(errno));
        retval = bRC_Error;
      } else {
      	Dmsg(ctx, debuglevel, "zfs-fd: init libzfs ok\n");
      }
      break;
    case bEventRestoreCommand:
      // Fall-through wanted
    case bEventBackupCommand:
      // Fall-through wanted
    case bEventEstimateCommand:
      // Fall-through wanted
    case bEventPluginCommand:
      Dmsg(ctx, debuglevel, "zfs-fd: Event-PluginCommand=%s\n", (char*)value);
      /*
       * Now it will be time for parsing the options
      */
      try {
      	p_ctx->parseConfig((char*)value);
      }
      catch (std::invalid_argument const& e) {
      	Jmsg(ctx, M_FATAL, "zfs-fd: Plugin option error: %d\n", e.what());
	Dmsg(ctx, debuglevel, "zfs-fd: Plugin option error: %d\n", e.what());
      	retval = bRC_Error;
      }
      break;
    case bEventNewPluginOptions:
      Dmsg(ctx, debuglevel, "zfs-fd: Event-NewPlugInOptions=%s\n", (char*)value);
      /* must be done */
      //retval = parse_plugin_definition(ctx, value);
      // Save that we got a plugin override.
      //p_ctx->plugin_options = strdup((char*)value);
      break;
    default:
      Jmsg(ctx, M_FATAL, "zfs-fd: unknown event=%d\n", event->eventType);
      Dmsg(ctx, debuglevel, "zfs-fd: unknown event=%d\n", event->eventType);
      retval = bRC_Error;
      break;
  }
  return retval; 
}
// Start the backup of an file
static bRC startBackupFile(PluginContext* ctx, [[maybe_unused]] save_pkt* sp) {
   Dmsg(ctx, debuglevel, "zfs-fd: calling startBackupFile\n");
  //time_t now;
  ZFSfdConfig* p_ctx = static_cast<ZFSfdConfig*>(ctx->plugin_private_context);
  if (!p_ctx) {
    Dmsg(ctx, debuglevel, NO_CONFIG_OBJECT_TEXT);
    return bRC_Error;
  }
  if (!checkConfig(ctx)) { return bRC_Error; }
  
/* Not implemented */
  return bRC_Error;
}
// Done with backup of this file
static bRC endBackupFile(PluginContext* ctx) {
   Dmsg(ctx, debuglevel, "zfs-fd: calling endBackupFile\n");
  /* bRC_OK when it was the last one
   * bRC_More when more to backup*/
  return bRC_OK;
}
// When the restore starts
static bRC startRestoreFile(PluginContext* ctx, const char* cmd) {
  Dmsg(ctx, debuglevel, "zfs-fd: calling startRestoreFile with %s\n", (char*)cmd);
  if (!checkConfig(ctx)) { return bRC_Error; }
  return bRC_OK;
}
// When the restore ends
static bRC endRestoreFile(PluginContext* ctx) {
  Dmsg(ctx, debuglevel, "zfs-fd: calling endRestoreFile\n");
  ZFSfdConfig* p_ctx = static_cast<ZFSfdConfig*>(ctx->plugin_private_context);
  if (!p_ctx) {
    Dmsg(ctx, debuglevel, NO_CONFIG_OBJECT_TEXT);
    return bRC_Error;
  }
  
  return bRC_OK;
}
// Now the data I/O will be done
static bRC pluginIO(PluginContext* ctx, [[maybe_unused]] io_pkt* io) {
  Dmsg(ctx, debuglevel, "zfs-fd: calling pluginIO\n");
  ZFSfdConfig* p_ctx = static_cast<ZFSfdConfig*>(ctx->plugin_private_context);
  if (!p_ctx) {
    Dmsg(ctx, debuglevel, NO_CONFIG_OBJECT_TEXT);
    return bRC_Error;
  }
  // Not impelented now
  return bRC_Error; 
}
/* On restore create the file before the data comes
 * Here we must prepare the ZFS/zfs_revice stuff
 * */
static bRC createFile(PluginContext* ctx, restore_pkt* rp) {
   Dmsg(ctx, debuglevel, "zfs-fd: calling createFile\n");
  // because not implemented yet
  rp->create_status = CF_SKIP;
  return bRC_OK;
}
// We don't need file attrubutes for ZFS, so ignore this.
static bRC setFileAttributes(PluginContext* ctx, [[maybe_unused]] restore_pkt* rp) {
  Dmsg(ctx, debuglevel, "zfs-fd: calling setFileAttributes\n");
  return bRC_OK;
}
// Check if an file exists in Inc/Diff backups, but this don't matter for zfs_send
static bRC checkFile(PluginContext* ctx, [[maybe_unused]] char* fname) {
  Dmsg(ctx, debuglevel, "zfs-fd: calling PluginContext\n");
  return bRC_OK;
}
// not needed for ZFS
static bRC getAcl(PluginContext* ctx, [[maybe_unused]] acl_pkt* ap) {
  Dmsg(ctx, debuglevel, "zfs-fd: calling getAcl\n");
  return bRC_OK;
}
// not needed for ZFS
static bRC setAcl(PluginContext* ctx, [[maybe_unused]] acl_pkt* ap) {
  Dmsg(ctx, debuglevel, "zfs-fd: calling setAcl\n");
  return bRC_OK;
}
// not needed for ZFS
static bRC getXattr(PluginContext* ctx, [[maybe_unused]] xattr_pkt* xp) {
  Dmsg(ctx, debuglevel, "zfs-fd: calling getXattr\n");
  return bRC_OK;
}
// not needed for ZFS
static bRC setXattr(PluginContext* ctx, [[maybe_unused]] xattr_pkt* xp) {
  Dmsg(ctx, debuglevel, "zfs-fd: calling setXattr\n");
  return bRC_OK;
}

// Plug-In functions
static bool checkConfig(PluginContext* ctx) {
  ZFSfdConfig* p_ctx = static_cast<ZFSfdConfig*>(ctx->plugin_private_context);
  try {
    p_ctx->verifyConfig();
  }
  catch (std::invalid_argument const& e) {
    Jmsg(ctx, M_FATAL, "zfs-fd: config error: %d\n", e.what());
    return false;
  }
  return true;
}
static void cleanup_libzfs(ZFSfdConfig* config) {
	if(config->getLibZFShandle()) {
		libzfs_fini(config->getLibZFShandle());
		config->setLibZFShandle();
	}
}
static bool init_libzfs(ZFSfdConfig* config) {
	bool ret = true;
	libzfs_handle_t* h = libzfs_init();
	if(h == NULL) {
		ret = false;
	} else 
		config->setLibZFShandle(h);
	return ret;
}
} // namespace filedaemon
