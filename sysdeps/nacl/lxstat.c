#include <errno.h>
#include <sys/stat.h>

#include <irt_syscalls.h>

int __lxstat (int vers, const char *name, struct stat *buf)
{
  if (buf == NULL || name == NULL)
    {
      errno = EFAULT;
      return -1;
    }
  struct nacl_abi_stat st;
  int result = __nacl_irt_lstat (name, &st);
  if (result != 0)
    {
      errno = result;
      return -1;
    }
  else
    {
      __nacl_abi_stat_to_stat (&st, buf);
      return 0;
    }
}
hidden_def (__lxstat)
weak_alias (__lxstat, _lxstat);
#ifdef SHARED
extern __typeof (__lxstat64) __lxstat64 __attribute__ ((alias ("__GI___lxstat")));
#endif
