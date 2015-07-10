/* Copyright (C) 2003, 2004, 2007, 2008 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@redhat.com>, 2003.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <time.h>
#include <sysdep.h>
#include "pthreadP.h"
#include <kernel-features.h>


int
pthread_condattr_setclock (attr, clock_id)
     pthread_condattr_t *attr;
     clockid_t clock_id;
{
  switch (clock_id)
    {
    case CLOCK_REALTIME:
      /* This is the default state and the only one actually supported.  */
      return 0;

    case CLOCK_MONOTONIC:
      /* NaCl recognizes CLOCK_MONOTONIC for other purposes, so it is a
         "known clock".  But NaCl doesn't support it for this purpose.  */
      return ENOTSUP;

    default:
      /* The only other recognized clocks are CPU-time clocks,
         which POSIX says should get EINVAL.  */
      return EINVAL;
    }
}
