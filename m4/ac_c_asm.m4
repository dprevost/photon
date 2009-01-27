dnl  Copyright (C) 2008-2009 Daniel Prevost <dprevost@photonsoftware.org>
dnl
dnl See the file COPYING_Macros for license information.
dnl
dnl This m4 macro test which version of the keyword asm is supported 
dnl if any. 
dnl

AC_DEFUN([AC_C_ASM],[

  _AC_C___ASM__
  _AC_C_ASM
  _AC_C___ASM
  
])

AC_DEFUN([_AC_C_ASM],
  [AC_CACHE_CHECK(
    [whether asm is supported], 
    [ac_cv_asm],
    [AC_COMPILE_IFELSE(
      [AC_LANG_PROGRAM(
        [],
        [[int main() {
            int err;
            asm ("": : :"memory");
            return 0;
          }
        ]])],
      [ac_cv_asm=yes],
      [ac_cv_asm=no])
    ])

  if test $ac_cv_asm = yes; then
    AC_DEFINE([HAVE_ASM], 1,
             [Define if asm is supported by the compiler.])
  fi
])

AC_DEFUN([_AC_C___ASM],[
  AC_CACHE_CHECK(
    [whether __asm is supported], 
    [ac_cv___asm],
    [AC_COMPILE_IFELSE(
      [AC_LANG_PROGRAM(
        [],
        [[int main() {
            int err;
            _asm ("": : :"memory");
            return 0;
          }
        ]])],
      [ac_cv___asm=yes],
      [ac_cv___asm=no])
    ])

  if test $ac_cv___asm = yes; then
    AC_DEFINE([HAVE___ASM], 1,
             [Define if __asm is supported by the compiler.])
  fi
])

AC_DEFUN([_AC_C___ASM__],[
  AC_CACHE_CHECK(
    [whether __asm__ is supported], 
    [ac_cv___asm__],
    [AC_COMPILE_IFELSE(
      [AC_LANG_PROGRAM(
        [],
        [[int main() {
            int err;
            __asm__ ("": : :"memory");
            return 0;
          }
        ]])],
      [ac_cv___asm__=yes],
      [ac_cv___asm__=no])
    ])

  if test $ac_cv___asm__ = yes; then
    AC_DEFINE([HAVE___ASM__], 1,
             [Define if __asm__ is supported by the compiler.])
  fi
])

