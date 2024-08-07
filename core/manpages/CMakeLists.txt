#   BAREOS® - Backup Archiving REcovery Open Sourced
#
#   Copyright (C) 2017-2024 Bareos GmbH & Co. KG
#
#   This program is Free Software; you can redistribute it and/or
#   modify it under the terms of version three of the GNU Affero General Public
#   License as published by the Free Software Foundation and included
#   in the file LICENSE.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
#   Affero General Public License for more details.
#
#   You should have received a copy of the GNU Affero General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
#   02110-1301, USA.
message("Entering ${CMAKE_CURRENT_SOURCE_DIR}")

set(MAN8 bareos-fd.8 btraceback.8)

if(NOT client-only)
  set(MAN1-server bconsole.1 bsmtp.1 bareos-tray-monitor.1 bwild.1 bregex.1)

  set(MAN8-server
      bareos-dir.8
      bareos-sd.8
      bareos.8
      bcopy.8
      bareos-dbcheck.8
      bextract.8
      bls.8
      bscan.8
      btape.8
      bpluginfo.8
      bscrypto.8
  )

  set(MAN1 "${MAN1-server}")
  set(MAN8 "${MAN8};${MAN8-server}")

endif()
install(FILES ${MAN1} DESTINATION "${mandir}/man1")
install(FILES ${MAN8} DESTINATION "${mandir}/man8")
