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

// Tuxmaster5000, Jun 2024

/**
 * @file
 * The ZFS plugin for the Bareos File Daemon
 */

#ifndef ZFS_FD_HEADER
#define ZFS_FD_HEADER 


#include <unistd.h>

#include "include/fcntl_def.h"
#include "include/bareos.h"
#include "include/filetypes.h"
#include "filed/fd_plugins.h"
#include "plugins/include/common.h"

#include "zfs-fd-config.h"

namespace filedaemon {

#define PLUGIN_LICENSE "GPLv3+"
#define PLUGIN_AUTHOR "Tuxmaster5000"
#define PLUGIN_DATE ZFS_BUILD_STAMP
#define PLUGIN_VERSION ZFS_BUILD_VERSION
#define PLUGIN_DESCRIPTION "ZFS File Daemon Plugin"
#define PLUGIN_USAGE                                                   \
  "zfs:option=<setting>:option2=<setting>:option3=<setting>\n" \
  " option foo\n" \
  " option1 bar\n" \
  " option2 bar\n" \
  " option2 foo"

// Pointers to Bareos functions
static CoreFunctions* bareos_core_functions = nullptr;
static PluginApiDefinition* bareos_plugin_interface_version = nullptr;


// Forward referenced functions
static bRC newPlugin(PluginContext* ctx);
static bRC freePlugin(PluginContext* ctx);
static bRC getPluginValue(PluginContext* ctx, pVariable var, void* value);
static bRC setPluginValue(PluginContext* ctx, pVariable var, void* value);
static bRC handlePluginEvent(PluginContext* ctx, bEvent* event, void* value);
static bRC startBackupFile(PluginContext* ctx, save_pkt* sp);
static bRC endBackupFile(PluginContext* ctx);
static bRC startRestoreFile(PluginContext* ctx, const char* cmd);
static bRC endRestoreFile(PluginContext* ctx);
static bRC pluginIO(PluginContext* ctx, io_pkt* io);
static bRC createFile(PluginContext* ctx, restore_pkt* rp);
static bRC setFileAttributes(PluginContext* ctx, restore_pkt* rp);
static bRC checkFile(PluginContext* ctx, char* fname);
static bRC getAcl(PluginContext* ctx, acl_pkt* ap);
static bRC setAcl(PluginContext* ctx, acl_pkt* ap);
static bRC getXattr(PluginContext* ctx, xattr_pkt* xp);
static bRC setXattr(PluginContext* ctx, xattr_pkt* xp);

static const int debuglevel = 150;

} // namespace filedaemon
#endif // ZFS_FD_HEADER
