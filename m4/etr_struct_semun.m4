dnl @synopsis ETR_STRUCT_SEMUN
dnl
dnl This macro checks to see if sys/sem.h defines struct semun. Some
dnl systems do, some systems don't. Your code must be able to deal with
dnl this possibility; if HAVE_STRUCT_SEMUM isn't defined for a given
dnl system, you have to define this structure before you can call
dnl functions like semctl().
dnl
dnl You should call ETR_SYSV_IPC before this macro, to separate the
dnl check for System V IPC headers from the check for struct semun.
dnl
dnl @category Misc
dnl @author Warren Young <warren@etr-usa.com>
dnl @version 2001-05-28
dnl @license AllPermissive

AC_DEFUN([ETR_STRUCT_SEMUN],
[
AC_CACHE_CHECK([for struct semun], ac_cv_struct_semun, [
        AC_TRY_COMPILE(
                [
                        #include <sys/types.h>
                        #include <sys/ipc.h>
                        #include <sys/sem.h>
                ],
                [ struct semun s; ],
                ac_cv_struct_semun=yes,
                ac_cv_struct_semun=no)
])

        if test x"$ac_cv_struct_semun" = "xyes"
        then
                AC_DEFINE(HAVE_STRUCT_SEMUN, 1,
                        [ Define if your system's sys/sem.h file defines struct semun ])
        fi
]) dnl ETR_STRUCT_SEMUN
