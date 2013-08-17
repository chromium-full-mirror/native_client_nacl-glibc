#include <errno.h>
#include <time.h>

#include <irt_syscalls.h>

/*
 * Please note that we do not support the general case via the TCB
 * (pthread_getcpuclockid) which means that CLOCK_THREAD_CPUTIME_ID
 * will behave differently from pthread_getcpuclockid(pthread_self()).
 */

#define SYSDEP_GETTIME                                      \
  case CLOCK_REALTIME:                                      \
  case CLOCK_MONOTONIC:                                     \
  case CLOCK_PROCESS_CPUTIME_ID:                            \
  case CLOCK_THREAD_CPUTIME_ID:                             \
    {                                                       \
      int result = __nacl_irt_clock_gettime (clock_id, tp); \
      if (result != 0)                                      \
        {                                                   \
          __set_errno (result);                             \
          retval = -1;                                      \
        }                                                   \
      else                                                  \
        retval = 0;                                         \
    }                                                       \
    break;

#define HANDLED_REALTIME
#define HANDLED_CPUTIME

#include <sysdeps/unix/clock_gettime.c>
