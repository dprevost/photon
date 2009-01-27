dnl  Copyright (C) 2008-2009 Daniel Prevost <dprevost@photonsoftware.org>
dnl
dnl See the file COPYING_Macros for license information.
dnl
dnl This m4 macro test which version of the keyword asm is supported 
dnl if any. 
dnl
#------------------------------------

#http://predef.sourceforge.net/precomp.html 


AC_DEFUN([AC_C_CC_TYPE],[

  ac_cv_compiler_name=unknown
 
  _AC_C_COMPILER_TEST(__xlc__)
  if test $ac_cv___xlc__ = yes; then
    ac_cv_compiler_name=xlc
  fi

  if test $ac_cv_compiler_name = unknown; then
    _AC_C_COMPILER_TEST(__INTEL_COMPILER)
    if test $ac_cv___INTEL_COMPILER = yes; then
      ac_cv_compiler_name=ICC
    fi
  fi

# no inline assembly + this is an embedded C compiler
#  if test $ac_cv_compiler_name = unknown; then
#    _AC_C_COMPILER_TEST(__ghs__)
#    if test $ac_cv___ghs__ = yes; then
#      ac_cv_compiler_name=GHS
#    fi
#  fi

  if test $ac_cv_compiler_name = unknown; then
    _AC_C_COMPILER_TEST(__PATHCC__)
    if test $ac_cv___PATHCC__ = yes; then
      ac_cv_compiler_name=PathScale
    fi
  fi

#  Comeau is pure c++, not c. 
#  if test $ac_cv_compiler_name = unknown; then
#    _AC_C_COMPILER_TEST(__COMO__)
#    if test $ac_cv___COMO__ = yes; then
#      ac_cv_compiler_name = Comeau
#    fi
#  fi

  # The test for gcc must come after the tests for some other
  # compilers since some defined __GNUC__ (Intel for example). 
  if test $ac_cv_compiler_name = unknown; then
    _AC_C_COMPILER_TEST(__GNUC__)
    if test $ac_cv___GNUC__ = yes; then
      ac_cv_compiler_name=GCC
    fi
  fi
  
  if test $ac_cv_compiler_name != unknown; then
    AC_DEFINE_UNQUOTED([COMPILER_NAME], "$ac_cv_compiler_name",
             [Define if we can guess the compiler.])
  fi

])

AC_DEFUN([_AC_C_COMPILER_TEST],
  [AC_CACHE_CHECK(
    [if $1 is defined by the compiler], 
    [ac_cv_$1],
    [AC_COMPILE_IFELSE(
      [AC_LANG_PROGRAM(
        [[
#ifndef $1
#  error
#endif]],
        [[int main() { return 0; }
        ]])],
      [ac_cv_$1=yes],
      [ac_cv_$1=no])
    ])
])


#ibm c compiler
#__xlc__ || __xlC__
###if defined(__IBMC__) || defined(__IBMCPP__)
#void __fence (void); compiler barrier
#_ldarx, __lwarx ??
#__stdcx, __stwcx
#__clear_lock_mp, __clear_lockd_mp
#__check_lock_mp, __check_lockd_mp
#__compare_and_swap, __compare_and_swaplp
#__isync
#__eieio, __iospace_eioio

#----------------------------------------------

#sun
#__SUNPRO_C
#__SUNPRO_CC
#membar_ops
#atomic_swap
#atomic_cas

#----------------------------
