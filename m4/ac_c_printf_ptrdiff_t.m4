dnl @synopsis AC_C_PRINTF_PTRDIFF_T
dnl
dnl Whether the compiler recognizes the special flag (%zd) for printing 
dnl variables of type ptrdiff_t.
dnl
dnl @category C
dnl @author Daniel Prevost <dprevost@photonsoftware.org>
dnl @version 2008-03-24
dnl @license AllPermissive

AC_DEFUN([AC_C_PRINTF_PTRDIFF_T], [
  AC_CACHE_CHECK([whether the compiler understands printf flag for ptrdiff_t],
                 ac_cv_c_printf_ptrdiff_t, [
    AC_TRY_COMPILE([
      #include <stdio.h>
      #if HAVE_SYS_TYPES_H
      # include <sys/types.h>
      #endif
      #if STDC_HEADERS
      # include <stdlib.h>
      # include <stddef.h>
      #else
      # if HAVE_STDLIB_H
      #  include <stdlib.h>
      # endif
      #endif
      #if HAVE_INTTYPES_H
      # include <inttypes.h>
      #else
      # if HAVE_STDINT_H
      #  include <stdint.h>
      # endif
      #endif
      #if HAVE_UNISTD_H
      # include <unistd.h>
      #endif], 
      [ptrdiff_t num = 0; printf("%td", num);],
      ac_cv_c_printf_ptrdiff_t=yes,ac_cv_c_printf_ptrdiff_t=no)])

  if test $ac_cv_c_printf_ptrdiff_t = yes; then
    AC_DEFINE(HAVE_PRINTF_PTRDIFF_T, 1, [compiler understands printf flag for ptrdiff_t varaibles])
 fi
])

