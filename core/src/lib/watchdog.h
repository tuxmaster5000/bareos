/*
   BAREOS® - Backup Archiving REcovery Open Sourced

   Copyright (C) 2002-2009 Free Software Foundation Europe e.V.
   Copyright (C) 2016-2024 Bareos GmbH & Co. KG

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
// Kern Sibbald, December MMII
/**
 * @file
 * Watchdog timer routines
 */
#ifndef BAREOS_LIB_WATCHDOG_H_
#define BAREOS_LIB_WATCHDOG_H_

#include <signal.h>
#include "lib/dlink.h"
#include "include/bc_types.h"
#include "include/dll_import_export.h"

enum
{
  TYPE_CHILD = 1,
  TYPE_PTHREAD,
  TYPE_BSOCK
};

static inline constexpr int kTimeoutSignal = SIGUSR2;

struct s_watchdog_t {
  bool one_shot;
  utime_t interval;
  void (*callback)(struct s_watchdog_t* wd);
  void (*destructor)(struct s_watchdog_t* wd);
  void* data;
  /* Private data below - don't touch outside of watchdog.c */
  dlink<s_watchdog_t> link;
  utime_t next_fire;
};
typedef struct s_watchdog_t watchdog_t;

/* Exported globals */
BAREOS_IMPORT utime_t watchdog_time; /* this has granularity of SLEEP_TIME */
int StartWatchdog(void);
int StopWatchdog(void);
watchdog_t* NewWatchdog(void);
bool RegisterWatchdog(watchdog_t* wd);
bool UnregisterWatchdog(watchdog_t* wd);
bool IsWatchdog();

#endif  // BAREOS_LIB_WATCHDOG_H_
