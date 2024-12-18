#!/usr/bin/env bash
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

if [ "$1" = options ]; then
  printf "%s\n" bucket s3cfg s3cmd_prog base_url storage_class
  exit 0
fi

# option defaults
: "${s3cfg:=}"
: "${s3cmd_prog:=$(command -v s3cmd)}"
: "${bucket:=backup}"
: "${base_url:=s3://${bucket}}"
: "${storage_class:=}"

s3cmd_common_args=(--no-progress)
s3cmd_put_args=()

if [ -n "${s3cfg}" ]; then
  if [ ! -r "${s3cfg}" ]; then
    echo "provided configuration file '${s3cfg}' is not readable" >&2
    exit 1
  else
    s3cmd_common_args+=(--config "${s3cfg}")
  fi
fi

if [ -z "${s3cmd_prog}" ]; then
  echo "Cannot find s3cmd command" >&2
  exit 1
elif [ ! -x "${s3cmd_prog}" ]; then
  echo "Cannot execute '${s3cmd_prog}'" >&2
  exit 1
fi

if [ -n "${storage_class}" ]; then
  s3cmd_put_args+=("--storage-class=${storage_class}")
fi

run_s3cmd() {
  "$s3cmd_prog" "${s3cmd_common_args[@]}" "$@"
}
exec_s3cmd() {
  exec "$s3cmd_prog" "${s3cmd_common_args[@]}" "$@"
}

if [ $# -eq 2 ]; then
  volume_url="${base_url}/$2/"
elif [ $# -eq 3 ]; then
  part_url="${base_url}/$2/$3"
elif [ $# -gt 3 ]; then
  echo "$0 takes at most 3 arguments" >&2
  exit 1
fi

case "$1" in
  testconnection)
    exec_s3cmd info "${base_url}"
    ;;
  list)
    run_s3cmd ls "${volume_url}" \
    | while read -r date time size url; do
      [[ "${size}" !=  +([0-9]) ]] && continue
      echo "${url:${#volume_url}}" "$size"
    done
    ;;
  stat)
    # shellcheck disable=SC2030,SC2034
    run_s3cmd ls "$part_url" \
      | (read -r date time size url; echo "$size")
    ;;
  upload)
    exec_s3cmd "${s3cmd_put_args[@]}" put - "$part_url"
    ;;
  download)
    exec_s3cmd get "$part_url" -
    ;;
  remove)
    exec_s3cmd del "$part_url"
    ;;
  *)
    exit 2
    ;;
esac
