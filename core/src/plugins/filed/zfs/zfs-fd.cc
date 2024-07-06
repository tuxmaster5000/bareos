/*
   Copyright (C) 2024 tuxmaster5000 tuxmaster5000@googlemail.com

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "zfs-fd.h"

/* ZFS header*/
// #include <libzfs.h>

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
  ZFSfdConfig* p_ctx =  new ZFSfdConfig();
  if (!p_ctx) {
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
  ZFSfdConfig* p_ctx = static_cast<ZFSfdConfig*>(ctx->plugin_private_context);
       
  if (!p_ctx) {
    return bRC_Error;
  }
  // free our private context.
  delete p_ctx;
  p_ctx = nullptr;
  return bRC_OK;
}
// Not used current in Bareos
static bRC getPluginValue([[maybe_unused]] PluginContext* ctx, [[maybe_unused]] pVariable var, [[maybe_unused]] void* value) {
  return bRC_OK;
}
// Not used current in Bareos
static bRC setPluginValue([[maybe_unused]] PluginContext* ctx, [[maybe_unused]] pVariable var, [[maybe_unused]] void* value) {
  return bRC_OK;
}
// Handle an event that comes from Bareos
static bRC handlePluginEvent(PluginContext* ctx, bEvent* event, void* value) {
  bRC retval = bRC_OK;
  ZFSfdConfig* p_ctx = static_cast<ZFSfdConfig*>(ctx->plugin_private_context);
  // No plug-in context -> something must be wrong.
  if (!p_ctx) {
    return bRC_Error;
  }
  switch (event->eventType) {
    case bEventJobStart:
      Dmsg(ctx, debuglevel, "zfs-fd: JobStart=%s\n", (char*)value);
      break;
    case bEventRestoreCommand:
      // Fall-through wanted
    case bEventBackupCommand:
      // Fall-through wanted
    case bEventEstimateCommand:
      // Fall-through wanted
    case bEventPluginCommand:
      /* must be done */
      //retval = parse_plugin_definition(ctx, value);
      break;
    case bEventNewPluginOptions:
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
  if (plugin_has_all_arguments(ctx) != bRC_OK) {
    return bRC_Error; 
  }
  //time_t now;
  ZFSfdConfig* p_ctx = static_cast<ZFSfdConfig*>(ctx->plugin_private_context);
  if (!p_ctx) {
    return bRC_Error;
  }
  /* Not implemented */
  return bRC_Error;
}
// Done with backup of this file
static bRC endBackupFile([[maybe_unused]] PluginContext* ctx) {
  /* bRC_OK when it was the last one
   * bRC_More when more to backup*/
  return bRC_OK;
}
// When the restore starts
static bRC startRestoreFile(PluginContext* ctx, [[maybe_unused]] const char* cmd) {
  if (plugin_has_all_arguments(ctx) != bRC_OK) {
    return bRC_Error;
  }
  return bRC_OK;
}
// When the restore ends
static bRC endRestoreFile(PluginContext* ctx) {
  ZFSfdConfig* p_ctx = static_cast<ZFSfdConfig*>(ctx->plugin_private_context);
  if (!p_ctx) {
    return bRC_Error;
  }
  return bRC_OK;
}
// Now the data I/O will be done
static bRC pluginIO(PluginContext* ctx, [[maybe_unused]] io_pkt* io) {
  ZFSfdConfig* p_ctx = static_cast<ZFSfdConfig*>(ctx->plugin_private_context);
  if (!p_ctx) {
    return bRC_Error;
  }
  // Not impelented now
  return bRC_Error; 
}
/* On restore create the file before the data comes
 * Here we must prepare the ZFS/zfs_revice stuff
 * */
static bRC createFile([[maybe_unused]] PluginContext* ctx, restore_pkt* rp) {
  // because not implemented yet
  rp->create_status = CF_SKIP;
  return bRC_OK;
}
// We don't need file attrubutes for ZFS, so ignore this.
static bRC setFileAttributes([[maybe_unused]] PluginContext* ctx, [[maybe_unused]] restore_pkt* rp) {
  return bRC_OK;
}
// Check if an file exists in Inc/Diff backups, but this don't matter for zfs_send
static bRC checkFile([[maybe_unused]] PluginContext* ctx, [[maybe_unused]] char* fname) {
  return bRC_OK;
}
// not needed for ZFS
static bRC getAcl([[maybe_unused]] PluginContext* ctx, [[maybe_unused]] acl_pkt* ap) {
  return bRC_OK;
}
// not needed for ZFS
static bRC setAcl([[maybe_unused]] PluginContext* ctx, [[maybe_unused]] acl_pkt* ap) {
  return bRC_OK;
}
// not needed for ZFS
static bRC getXattr([[maybe_unused]] PluginContext* ctx, [[maybe_unused]] xattr_pkt* xp) {
  return bRC_OK;
}
// not needed for ZFS
static bRC setXattr([[maybe_unused]] PluginContext* ctx, [[maybe_unused]] xattr_pkt* xp) {
  return bRC_OK;
}

// Plug-In functions
static bRC plugin_has_all_arguments(PluginContext* ctx) 
{
  bRC retval = bRC_OK;
  ZFSfdConfig* p_ctx = static_cast<ZFSfdConfig*>(ctx->plugin_private_context);
  if (!p_ctx) {
    return bRC_Error;
  }

  // Must be done

  return retval;
}


} // namespace filedaemon
