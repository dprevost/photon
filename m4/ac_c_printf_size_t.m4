dnl @synopsis AC_C_PRINTF_SIZE_T
dnl
dnl Whether the compiler recognizes the special flag (%zd) for printing 
dnl variables of type size_t.
dnl
dnl @category C
dnl @author Daniel Prevost <dprevost@photonsoftware.org>
dnl @version 2008-03-24
dnl @license AllPermissive

AC_DEFUN([AC_C_PRINTF_SIZE_T], [
  AC_CACHE_CHECK([whether the compiler understands printf flag for size_t],
                 ac_cv_c_printf_size_t, [
    AC_TRY_COMPILE([#include <stdio.h>],
                   [size_t num = 0; printf("%zd", num);],
                   ac_cv_c_printf_size_t=yes,ac_cv_c_printf_size_t=no)])
  if test $ac_cv_c_printf_size_t = yes; then
    AC_DEFINE(HAVE_PRINTF_SIZE_T, 1, [compiler understands printf flag for size_t variables])
 fi
])
