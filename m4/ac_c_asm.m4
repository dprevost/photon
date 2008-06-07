dnl  Copyright (C) 2008 Daniel Prevost <dprevost@users.sourceforge.net>
dnl
dnl See the file COPYING_Macros for license information.
dnl
dnl This m4 macro test which version of the keyword asm is supported 
dnl if any. 
dnl

AC_DEFUN([TEST_ASM],[
  _AC_C_COMPILER
  _AC_C_ASM
  _AC_C___ASM
  _AC_C___ASM__

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

AC_DEFUN([_AC_C_COMPILER],
  [AC_CACHE_CHECK(
    [which C compiler is used], 
    [ac_cv_compiler],
    [AC_COMPILE_IFELSE(
      [AC_LANG_PROGRAM(
        [#ifndef __GNUC__
         #  error
         #endif],
        [[int main() { return 0; }
        ]])],
      [ac_cv_compiler=__GNUC__],
      [ac_cv_compiler=none])
    ])

  if test $ac_cv_compiler = yes; then
    AC_DEFINE([HAVE_COMPILER], $ac_cv_compiler,
             [Define if we can guess the compiler.])
  fi
])

------------------------------------

http://predef.sourceforge.net/precomp.html 


ibm c compiler
__xlc__ || __xlC__
#if defined(__IBMC__) || defined(__IBMCPP__)
void __fence (void); compiler barrier
_ldarx, __lwarx ??
__stdcx, __stwcx
__clear_lock_mp, __clear_lockd_mp
__check_lock_mp, __check_lockd_mp
__compare_and_swap, __compare_and_swaplp
__isync
__eieio, __iospace_eioio

----------------------------------------------

sun
__SUNPRO_C
__SUNPRO_CC
membar_ops
atomic_swap
atomic_cas

----------------------------
