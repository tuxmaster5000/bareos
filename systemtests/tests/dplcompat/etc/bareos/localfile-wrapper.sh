#!/bin/bash
#   BAREOS® - Backup Archiving REcovery Open Sourced
#
#   Copyright (C) 2024-2024 Bareos GmbH & Co. KG
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

set -Eeuo pipefail

get_filesize() {
  if [ "$(uname)" = "FreeBSD" ]; then
    stat -f %z "$1"
  else
    stat "--format=%s" "$1"
  fi
}

case "$1" in
  options)
    cat << '_EOT_'
storage_path
_EOT_
    ;;
  testconnection)
    [ -d "$storage_path" ]
    ;;
  list)
    for f in "$storage_path/$2".*; do
      base="$(basename "$f")"
      printf "%s %d\n" "${base##*.}" "$(get_filesize "$f")"
    done
    ;;
  stat)
    get_filesize "$storage_path/$2.$3"
    ;;
  upload)
    exec cat >"$storage_path/$2.$3"
    ;;
  download)
    exec cat "$storage_path/$2.$3"
    ;;
  remove)
    exec rm "$storage_path/$2.$3"
    ;;
  *)
    exit 2
    ;;
esac
