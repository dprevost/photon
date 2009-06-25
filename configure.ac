#############################################################################
#                                                                           #
# Copyright (C) 2006-2009 Daniel Prevost <dprevost@photonsoftware.org>      #
#                                                                           #
# This file is free software; as a special exception the author gives       #
# unlimited permission to copy and/or distribute it, with or without        #
# modifications, as long as this notice is preserved.                       #
#                                                                           #
# This program is distributed in the hope that it will be useful, but       #
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the    #
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  #
#                                                                           #
#############################################################################

#############################################################################
#                                                                           #
# To do:                                                                    #
#                                                                           #
#  - java, python, etc. APIS as optional (specially if we cannot find the   #
#    package, JAVA_HOME or similar.                                         #
#  - using --enable-kernel-headers can create a problem with some compilers #
#    since it is using inline assembler. This should be tested and we       #
#    should revert to using semaphores if assembler is not supported.       #
#                                                                           #
#############################################################################

AC_INIT([Photon Software],
        [0.5.0],
        [Daniel Prevost <dprevost@photonsoftware.org>],
        [photon])

AC_PREREQ(2.57)
AC_CONFIG_AUX_DIR(Config)
AC_CONFIG_LIBOBJ_DIR(src/Common)

AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_MACRO_DIR([m4])
AM_CONFIG_HEADER(src/config.h)

AC_AIX 
AC_ISC_POSIX 
AC_MINIX 

##################################
#                                #
# SECTION - checks for programs  #
#                                #
##################################

AC_PROG_CC
AC_DEFINE_UNQUOTED(MYCC, "$CC", The C Compiler)
AC_PROG_CXX
AC_DEFINE_UNQUOTED(MYCXX, "$CXX", The C++ Compiler)

##################################
#                                #
# SECTION - checks for libraries #
#                                #
##################################

AC_SEARCH_LIBS(sem_init, pthread rt posix4)

ACX_PTHREAD(
   [
      LIBS="$PTHREAD_LIBS $LIBS"
	   CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
	   CC="$PTHREAD_CC"
      AC_DEFINE(USE_PTHREAD, 1, [Using Posix threads])
	],
	[
	    AC_MSG_ERROR([Photon requires a Posix Thread library and I don'k know where it is.])
	])

AC_CHECK_PROG(found_pkg_config, pkg-config, [yes], [no] )
if test "x$found_pkg_config" != "xyes" ; then
  AC_MSG_ERROR([Photon requires pkg-config])
fi

PKG_CHECK_MODULES(LIBXML2, [libxml-2.0],,
   AC_MSG_ERROR([Photon requires libxml2 (including the development environment)])) 

AC_SUBST(LIBXML2_CFLAGS)
AC_SUBST(LIBXML2_LIBS)

#################################################
#                                               #
# Section - checks for compiler characteristics #
#                                               #
#################################################

AC_C_INLINE 

AC_C_ASM

AC_C_CC_TYPE
if test "$ac_cv_compiler_name" = "unknown"; then
  AC_MSG_NOTICE([This version of Photon does not for support your compiler])
  AC_MSG_ERROR([So, please contact us to add support for your compiler])
fi

##################################
#                                #
# Section - checks for functions #
#                                #
##################################

AC_FUNC_FORK 
AC_CHECK_FUNCS(memcpy access vsnprintf)

AC_FUNC_MKDIR
AC_CHECK_FUNCS(getpid _getpid fdatasync fsync msync mprotect)
AC_CHECK_FUNCS(strcpy bcopy stat mmap)
#AC_FUNC_MMAP 
AC_FUNC_SEM_INIT
AC_FUNC_STRERROR_R

AC_MSG_CHECKING(whether gettimeofday is broken)
gettimeofday_is_broken=no
AC_COMPILE_IFELSE(
  [AC_LANG_PROGRAM(
     [[#if TIME_WITH_SYS_TIME
       # include <sys/time.h>
       # include <time.h>
       #else
       # if HAVE_SYS_TIME_H
       #  include <sys/time.h>
       # else
       #  include <time.h>
       # endif
       #endif]],
     [[struct timeval tv;
       int ok;
       ok = gettimeofday( &tv, NULL );]])],,
    [gettimeofday_is_broken=yes])
if test "gettimeofday_is_broken" = "yes"; then
  AC_MSG_RESULT(yes)
  AC_MSG_ERROR([This version of gettimeofday is old... please contact us for support])
else
  AC_MSG_RESULT(no)
fi

dnl To test that the replacement functions do indeed work (or at least 
dnl compile...), you must run configure with --enable-replace-function-checks.

AC_ARG_ENABLE([replace-function-checks],
              AC_HELP_STRING([--enable-replace-function-checks],
                             [option for maintainers only for testing replacement functions (do not use it unless you know what you're doing) @<:@default=no@:>@]),
	      [replace_functions=$enableval],
	      [replace_functions=no])

if test "$replace_functions" = "yes"; then
  AC_LIBOBJ(memcmp)
  AC_LIBOBJ(memset)
  AC_LIBOBJ(localtime_r)
  AC_LIBOBJ(asctime_r)
  AC_LIBOBJ(ctime_r)
  AC_LIBOBJ(strnlen)
else
  AC_FUNC_MEMCMP
  # on autoconf 2.61, the macro (AC_FUNC_STRNLEN) pulls the new macro 
  # AC_USE_SYSTEM_EXTENSIONS which is not defined in previous autoconf 
  # versions. Three solutions:
  #   1) require 2.61
  #   2) drop support for AIX 4.3 and any other system with a broken strnlen.
  #   3) implement my own check
  # For the moment solution 2 it is.
  ####  AC_FUNC_STRNLEN
  AC_REPLACE_FUNCS(strnlen)
  AC_REPLACE_FUNCS(memset)
  AC_REPLACE_FUNCS(localtime_r)
  # There are 2 versions of asctime_r, with 2 or 3 arguments.
  AC_MSG_CHECKING(for asctime_r with 3 args)
  replace_asctime_r=no
  AC_COMPILE_IFELSE(
    [AC_LANG_PROGRAM([[#include <time.h>]],
                     [[  struct tm tm;
                         char buff[26], *res;
                         res = asctime_r(&tm, buff, 26);]])],,
    [replace_asctime_r=yes])
  if test "$replace_asctime_r" = "yes"; then
    AC_MSG_RESULT(no)
    AC_LIBOBJ(asctime_r)
  else
    AC_MSG_RESULT(yes) 
    AC_DEFINE(HAVE_ASCTIME_R_OK, 1, asctime_r is ok)
  fi
  # There are 2 versions of ctime_r, with 2 or 3 arguments.
  AC_MSG_CHECKING(for ctime_r with 3 args)
  replace_ctime_r=no
  AC_COMPILE_IFELSE(
    [AC_LANG_PROGRAM([[#include <time.h>]],
                     [[  time_t t;
                         char buff[26], *res;
                         res = ctime_r(&t, buff, 26);]])],,
    [replace_ctime_r=yes] )
  if test "$replace_ctime_r" = "yes"; then
    AC_MSG_RESULT(no)
    AC_LIBOBJ(ctime_r)
  else
    AC_MSG_RESULT(yes) 
    AC_DEFINE(HAVE_CTIME_R_OK, 1, ctime_r is ok)
  fi
fi

########################################
#                                      #
# Section - checks for system services #
#                                      #
########################################

AC_C_BIGENDIAN

if test "$build_alias" != ""; then
  AC_MSG_ERROR([This version of Photon does not support cross-compilation])
fi
if test "$target_alias" != ""; then
  AC_MSG_ERROR([This version of Photon does not support cross-compilation])
fi

# The ACX_THREAD m4 routine already called AC_CANONICAL_HOST so we can
# omit it here and remove a warning!
# AC_CANONICAL_HOST
AC_DEFINE_UNQUOTED(MYCPU, "$host_cpu", The host cpu)

architecture=all
case $host_cpu in
i386 | i486 | i586 | i686 )
             locking_dir=asm-i386  
             architecture=i386      ;;
x86_64)
          locking_dir=asm-x86_64
          architecture=x86_64       ;;
#alpha)       locking_dir=asm-alpha   ;;
sparc)       locking_dir=asm-sparc   
             architecture=sparc     ;;
#sparc64)     locking_dir=asm-sparc64 ;;
*) locking_dir=empty ;;
esac
AC_SUBST(architecture)

############################################################
#                                                          #
# SECTION - all the stuff needed to implement concurrency  #
#           (locks, memory barriers, etc)                  #
#                                                          #
############################################################

arch_include=empty

case $ac_cv_compiler_name in
GCC)
  AC_DEFINE(PSO_USE_GCC, 1, Using gcc)
  case $host_cpu in
  i386 | i486 | i586 | i686 )
    arch_include=ok 
    AC_DEFINE(PSO_USE_I386_GCC, 1, Using a gcc-like compiler on a i386) ;;
  x86_64)
    arch_include=ok 
    AC_DEFINE(PSO_USE_X86_64_GCC, 1, Using a gcc-like compiler on a x86_64) ;;
  #alpha)          ;;
  sparc)
    arch_include=ok
    AC_DEFINE(PSO_USE_SPARC_GCC, 1, Using a gcc-like compiler on a sparc) ;;
  #sparc64)      ;;
  powerpc)
    arch_include=ok
    AC_DEFINE(PSO_USE_PPC_GCC, 1, Using a gcc-like compiler on a PowerPC) ;;
  esac ;;
ICC)
  AC_DEFINE(PSO_USE_ICC, 1, Using icc)
  # Intel compiler on x86 (32 and 64 bits) understands gcc inline
  case $host_cpu in
  i386 | i486 | i586 | i686 )
    arch_include=ok 
    AC_DEFINE(PSO_USE_I386_GCC, 1, Using a gcc-like compiler on a i386) ;;
  x86_64)
    arch_include=ok 
    AC_DEFINE(PSO_USE_X86_64_GCC, 1, Using a gcc-like compiler on a x86_64) ;;
  esac ;;
XLC)
  AC_DEFINE(PSO_USE_XLC, 1, Using xlc)
  if test "$host_cpu" = "ppc"; then
    arch_include=ok
    AC_DEFINE(PSO_USE_PPC_XLC, 1, Using the xlc compiler on a Power PC)
  fi ;;
PathScale)
  AC_DEFINE(PSO_USE_PATHSCALE, 1, Using Pathscale)
  case $host_cpu in
  i386 | i486 | i586 | i686 )
    arch_include=ok 
    AC_DEFINE(PSO_USE_I386_GCC, 1, Using a gcc-like compiler on a i386) ;;
  x86_64)
    arch_include=ok 
    AC_DEFINE(PSO_USE_X86_64_GCC, 1, Using a gcc-like compiler on a x86_64) ;;
  esac ;;
*) ;;
esac

if test "$arch_include" = "empty"; then
#  if test "$ac_cv_sem_init_works" != "yes"; then
# AC_MSG_ERROR([Semaphores are unsupported and no locking primitives, don't know how to lock. Sorry!])
#  fi
  AC_MSG_ERROR([No locking primitives, don't know how to lock. Sorry!])
fi

####################
#                  #
# Section - output #
#                  #
####################

AC_CONFIG_SUBDIRS(src/Python)

AC_CONFIG_FILES([
   doc/doxyfile.photon_c
   doc/doxyfile.photon_cpp
   src/include/Doxygen/pso_c
   src/include/Doxygen/DataDefBuilderODBC
   src/include/Doxygen/DataDefBuilderUser
   src/include/Doxygen/DataDefinition
   src/include/Doxygen/KeyDefBuilderODBC
   src/include/Doxygen/KeyDefBuilderUser
   src/include/Doxygen/KeyDefinition
   src/include/Doxygen/psoException
   src/include/Doxygen/psoFastMapEditor
   src/include/Doxygen/psoFastMap
   src/include/Doxygen/psoFolder
   src/include/Doxygen/psoHashMap
   src/include/Doxygen/psoLifo
   src/include/Doxygen/psoProcess
   src/include/Doxygen/psoQueue
   src/include/Doxygen/psoSession
   src/Packages/buildDeb.sh
   src/Packages/control
   src/Packages/photon.spec
])
   
AC_CONFIG_FILES([
   Makefile
   doc/Makefile
   m4/Makefile
   src/Makefile
   src/API/Makefile
   src/API/Tests/Makefile
   src/API/Tests/Api/Makefile
   src/API/Tests/CommonObject/Makefile
   src/API/Tests/Connector/Makefile
   src/API/Tests/DataDefinition/Makefile
   src/API/Tests/FastMap/Makefile
   src/API/Tests/Folder/Makefile
   src/API/Tests/HashMap/Makefile
   src/API/Tests/KeyDefinition/Makefile
   src/API/Tests/Lifo/Makefile
   src/API/Tests/ListReaders/Makefile
   src/API/Tests/Process/Makefile
   src/API/Tests/Queue/Makefile
   src/API/Tests/Session/Makefile
   src/Common/Makefile
   src/Common/Tests/Makefile
   src/Common/Tests/DirAccess/Makefile
   src/Common/Tests/Error/Makefile
   src/Common/Tests/MemFile/Makefile
   src/Common/Tests/Optional/Makefile
   src/Common/Tests/Options/Makefile
   src/Common/Tests/ProcessLock/Makefile
   src/Common/Tests/ThreadLock/Makefile
   src/C++/Makefile
   src/C++/Tests/Makefile
   src/C++/Tests/DataDefinition/Makefile
   src/C++/Tests/FastMap/Makefile
   src/C++/Tests/Folder/Makefile
   src/C++/Tests/HashMap/Makefile
   src/C++/Tests/Lifo/Makefile
   src/C++/Tests/Process/Makefile
   src/C++/Tests/Queue/Makefile
   src/C++/Tests/Session/Makefile
   src/Errors/Makefile
   src/Java/Makefile
   src/Nucleus/Makefile
   src/Nucleus/Tests/Makefile
   src/Nucleus/Tests/BlockGroup/Makefile
   src/Nucleus/Tests/Cursor/Makefile
   src/Nucleus/Tests/FastMap/Makefile
   src/Nucleus/Tests/Folder/Makefile
   src/Nucleus/Tests/Hash/Makefile
   src/Nucleus/Tests/HashTx/Makefile
   src/Nucleus/Tests/HashMap/Makefile
   src/Nucleus/Tests/List/Makefile
   src/Nucleus/Tests/LogFile/Makefile
   src/Nucleus/Tests/MemAlloc/Makefile
   src/Nucleus/Tests/MemBitmap/Makefile
   src/Nucleus/Tests/MemObject/Makefile
   src/Nucleus/Tests/Optional/Makefile
   src/Nucleus/Tests/Process/Makefile
   src/Nucleus/Tests/ProcessManager/Makefile
   src/Nucleus/Tests/Queue/Makefile
   src/Nucleus/Tests/Session/Makefile
   src/Nucleus/Tests/TopFolder/Makefile
   src/Nucleus/Tests/Transaction/Makefile
   src/Quasar/Makefile
   src/Quasar/Tests/Makefile
   src/include/Makefile
   src/Tests/Makefile
   src/Tools/Makefile
   src/Tools/Shell/Makefile
   src/XML/Makefile
   src/XML/Tests/Makefile
])

#   src/Proto/Makefile
# src/Tools/xml2c/Makefile

AC_OUTPUT

