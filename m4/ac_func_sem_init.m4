dnl  Copyright (C) 2006 Daniel Prevost <dprevost@photonsoftware.org>
dnl
dnl See the file COPYING_Macros for license information.
dnl
dnl This m4 macro test whether the POSIX semaphores can be used on a 
dnl specific platforms to provide locks between processes.
dnl
dnl On some systems, the functions are available (so AC_CHECK_FUNCS would
dnl succeed) but would be limited to offer locks for threads, not for
dnl processes (on linux with glibc 2.3 - on AIX 5 the POSIX semaphore
dnl functions always return -1 and set errno to ENOSYS).
dnl
AC_DEFUN([AC_FUNC_SEM_INIT],[
  AC_CACHE_CHECK(
    [whether sem_init works with process], 
    [ac_cv_sem_init_works],
    [
      AC_RUN_IFELSE([
        AC_LANG_SOURCE([[
#if HAVE_SEMAPHORE_H
#  include <semaphore.h>
#elif HAVE_SYS_SEMAPHORE_H
/* Needed on some versions of HP-UX */
#  include <sys/semaphore.h>
#endif
#include <fcntl.h>
#include <errno.h>
int main()
{
   int err;
   sem_t s;
   err = sem_init( &s, 1, 1 );
   if ( err == -1 )
   {
      if ( errno != 0 )
         return errno;
      return -1;
   }

   sem_destroy( &s );
	return 0;
}
        ]])
      ],[ac_cv_sem_init_works=yes],[ac_cv_sem_init_works=no],
      [AC_CHECK_FUNCS(sem_init)])
    ])

  if test $ac_cv_sem_init_works = yes; then
    AC_DEFINE([HAVE_SEM_INIT], 1,
             [Define if sem_init works properly.])
  fi
])

