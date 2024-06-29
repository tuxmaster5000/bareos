#    Copyright (C) 2024 tuxmaster5000 tuxmaster5000@googlemail.com
#
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.

message("Entering ${CMAKE_CURRENT_SOURCE_DIR}")

string(TIMESTAMP ZFS_BUILD_STAMP "%Y-%m-%d %H:%M:%S" UTC)
set(ZFS_BUILD_VERSION "0.0.1")

add_library(zfs-fd MODULE zfs-fd.cc)
set_target_properties(zfs-fd PROPERTIES PREFIX "")
install(
  TARGETS zfs-fd
  DESTINATION ${plugindir}
  COMPONENT filedaemon
)
target_compile_definitions(
    zfs-fd
    PRIVATE -DZFS_BUILD_STAMP="${ZFS_BUILD_STAMP}"
    PRIVATE -DZFS_BUILD_VERSION="${ZFS_BUILD_VERSION}"
)
target_link_libraries(zfs-fd PkgConfig::ZFS)
target_include_directories(zfs-fd PRIVATE ${ZFS_INCLUDE_DIRS})
