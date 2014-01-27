#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#include <irt_syscalls.h>

/* Truncate PATH to LENGTH bytes.  */
int
__truncate (path, length)
     const char *path;
     off_t length;
{
  int result = __nacl_irt_truncate(path, length);
  if (result != 0) {
    errno = result;
    return -1;
  }
  return 0;
}
weak_alias (__truncate, truncate)
