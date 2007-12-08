#############################################################################
#                                                                           #
# Copyright (C) 2006-2007 Daniel Prevost <dprevost@users.sourceforge.net>   #
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

AC_INIT([Virtual Data Space Framework],
        [0.1.0],
        [Daniel Prevost <dprevost@users.sourceforge.net>],
        [vdsf])

AC_PREREQ(2.57)
AC_CONFIG_AUX_DIR(Config)
AC_CONFIG_LIBOBJ_DIR(src/Common)

AM_INIT_AUTOMAKE()
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

AC_PROG_LIBTOOL
AC_ARG_VAR([PYTHON], [path to PYTHON])
AC_PATH_PROG([PYTHON], [python], [no])
if test "$PYTHON" = "no"; then
  AC_MSG_ERROR([python is required for the test suite])
fi

# Tell our makefiles if we can use doxygen and latex to regenarate some of 
# the documentation, as needed.
AC_CHECK_PROG([DOXY_IS_PRESENT], [doxygen], [yes], [no])
AM_CONDITIONAL([COND_USE_DOXYGEN], [test "$DOXY_IS_PRESENT" = yes])
AC_CHECK_PROG([LATEX_IS_PRESENT], [latex], [yes], [no])
AM_CONDITIONAL([COND_USE_LATEX], [test "$LATEX_IS_PRESENT" = yes])

# Java is not used yet (version 0.1)

#AC_ARG_WITH([java],
#              AC_HELP_STRING([--with-java],
#                             [will build the java API, if included @<:@default=yes@:>@]),
#	      [with_java=$withval],
#	      [with_java=yes])
#if test "$with_java" = "yes"; then
#  if test ! -d "$JAVA_HOME" ; then
#    AC_MSG_ERROR([JAVA_HOME env. variable must be set to build the Java API])
#  fi
#fi
#AM_CONDITIONAL([COND_USE_JAVA], [test "$with_java" = yes])

##################################
#                                #
# SECTION - checks for libraries #
#                                #
##################################

AC_SEARCH_LIBS(sem_init, pthread rt posix4)
AC_SEARCH_LIBS(nanosleep, rt posix4)

ACX_PTHREAD(
   [
      LIBS="$PTHREAD_LIBS $LIBS"
	   CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
	   CC="$PTHREAD_CC"
      AC_DEFINE(USE_PTHREAD, 1, [Using Posix threads])
	],
	[
	    AC_MSG_ERROR([VDSF requires a Posix Thread library and I don'k know where it is.])
	])

#####################################
#                                   #
# SECTION - checks for header files #
#                                   #
#####################################

AC_HEADER_STDC 
AC_HEADER_TIME 

AC_LANG_PUSH([C++])
AC_CHECK_HEADERS(new.h)
AC_CHECK_HEADERS(new)
AC_LANG_POP([C++])

AC_CHECK_HEADERS(assert.h ctype.h limits.h sys/param.h errno.h fcntl.h)
AC_CHECK_HEADERS(sys/time.h sys/mman.h signal.h sys/wait.h)
AC_CHECK_HEADERS(semaphore.h sys/semaphore.h sys/ipc.h sys/sem.h)
AC_CHECK_HEADERS(timers.h) # for Tru64
AC_HEADER_DIRENT 
AC_HEADER_STDBOOL

##############################
#                            #
# Section - checks for types #
#                            #
##############################

AC_TYPE_SIZE_T
AC_TYPE_OFF_T 
AC_TYPE_PID_T 
AC_CHECK_TYPES(ptrdiff_t)
AC_CXX_BOOL
AC_TYPE_SIGNAL 

ETR_STRUCT_SEMUN

AC_CHECK_SIZEOF(void *)

###################################
#                                 #
# Section - checks for structures #
# (do they miss data members?)    #
#                                 #
###################################

#################################################
#                                               #
# Section - checks for compiler characteristics #
#                                               #
#################################################

AC_C_VOLATILE 
AC_C_INLINE 

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
  AC_LIBOBJ(nanosleep)
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
  AC_REPLACE_FUNCS(nanosleep)
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

#########################################
#                                       #
# Section - i18n (internationalization) #
#                                       #
#########################################

AC_ARG_ENABLE([i18n],
              AC_HELP_STRING([--enable-i18n],
                             [use unicode internally to store names of objects @<:@default=yes@:>@]),
	      [use_i18n=$enableval],
	      [use_i18n=yes])
if test "$use_i18n" = "yes"; then
  AC_CHECK_HEADERS([wchar.h wctype.h],,[use_i18n=no])
  AC_CHECK_FUNCS([mbsrtowcs wcsrtombs],,[use_i18n=no])
fi
AC_MSG_CHECKING(if internationalization is supported)

if test "$use_i18n" = "yes"; then
  AC_DEFINE(VDS_SUPPORT_i18n, 1, [internationalization is supported])
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
fi

########################################
#                                      #
# Section - checks for system services #
#                                      #
########################################

AC_C_BIGENDIAN

if test "$build_alias" != ""; then
  AC_MSG_ERROR([This version of vdsf does not support cross-compilation])
fi
if test "$target_alias" != ""; then
  AC_MSG_ERROR([This version of vdsf does not support cross-compilation])
fi

# The ACX_THREAD m4 routine already called AC_CANONICAL_HOST so we can
# omit it here and remove a warning!
# AC_CANONICAL_HOST
AC_DEFINE_UNQUOTED(MYCPU, "$host_cpu", The host cpu)

architecture=all
case $host_cpu in
i386 | i486 | i586 | i686 )
             locking_dir=asm-i386  
             architecture=i386        ;;
x86_64)
          locking_dir=asm-x86_64
          architecture=x86_64         ;;
#alpha)       locking_dir=asm-alpha   ;;
#sparc)       locking_dir=asm-sparc   ;;
#sparc64)     locking_dir=asm-sparc64 ;;
*) locking_dir=empty ;;
esac
AC_SUBST(architecture)

##################################################
#                                                #
# Section - override VDSF build/compile defaults #
#                                                #
##################################################

AC_ARG_ENABLE([set-block-size],
              AC_HELP_STRING([--enable-set-block-size=some_value],
                             [set the size of vdsf blocks (values must be either 1k, 2k, 4k, 8k or 16k) @<:@default=8k@:>@]),
	      [block_size=$enableval],
	      [block_size=8k])
if test "$block_size" = "1k"; then
   AC_DEFINE(VDSE_BLOCK_SIZE,  1024)
   AC_DEFINE(VDSE_BLOCK_SHIFT,   10)
elif test "$block_size" = "2k"; then
   AC_DEFINE(VDSE_BLOCK_SIZE,  2048)
   AC_DEFINE(VDSE_BLOCK_SHIFT,   11)
elif test "$block_size" = "4k"; then
   AC_DEFINE(VDSE_BLOCK_SIZE,  4096)
   AC_DEFINE(VDSE_BLOCK_SHIFT,   12)
elif test "$block_size" = "8k"; then
   AC_DEFINE(VDSE_BLOCK_SIZE,  8192)
   AC_DEFINE(VDSE_BLOCK_SHIFT,   13)
elif test "$block_size" = "16k"; then
   AC_DEFINE(VDSE_BLOCK_SIZE,  16384, This define can be set by using configure with the option --enable-set-block-size=value)
   AC_DEFINE(VDSE_BLOCK_SHIFT,    14, This define can be set indirectly by using configure with the option --enable-set-block-size=value)
else
  AC_MSG_ERROR([Invalid value! Values allowed: 1k, 2k, 4k, 8k or 16k])
fi

###################################
#                                 #
# Section - automake conditionals #
#                                 #
###################################

AC_ARG_ENABLE([dbc],
              AC_HELP_STRING([--enable-dbc],
                             [add contract-validation checks to the code. You might want to disable this on production systems for performance reasons @<:@default=yes@:>@]),
	           [use_dbc=$enableval],
	           [use_dbc=yes])

AM_CONDITIONAL([COND_USE_DBC], [test "$use_dbc" = yes])

AC_ARG_ENABLE([extreme-dbc],
              AC_HELP_STRING([--enable-extreme-dbc],
                             [add even more contract-validation checks to the code. You might want to enable this to resolve a hard-to-find bug @<:@default=no@:>@]),
	           [use_extreme_dbc=$enableval],
	           [use_extreme_dbc=no])

AM_CONDITIONAL([COND_USE_EXTREME_DBC], [test "$use_extreme_dbc" = yes])
AM_CONDITIONAL([COND_USE_DBC], 
	            [test "$use_dbc" = yes || "$use_extreme_dbc" = yes])

AM_CONDITIONAL([COND_INCLUDE_DEMO], [test "$want_demo" = yes])

############################################################
#                                                          #
# SECTION - all the stuff needed to implement concurrency  #
#           (locks, memory barriers, etc)                  #
#                                                          #
############################################################

#AC_ARG_ENABLE([kernel-headers],
#              AC_HELP_STRING([--enable-kernel-headers],
#                             [Enable the use of the linux-kernel modified headers to implement locking @<:@default=yes@:>@]),
#              [use_headers=$enableval],
#              [use_headers=yes])
use_headers=yes

if test "$locking_dir" = "empty"; then
  AC_MSG_ERROR([Modified kernel headers unsupported on this cpu])
  AM_CONDITIONAL([COND_USE_KERNEL_HEADERS], [test "$locking_dir" != "empty"])
  if test "$ac_cv_sem_init_works" != "yes"; then
    AC_MSG_ERROR([Semaphores are unsupported, don't know how to lock. Sorry!])
  fi
else
  AC_CONFIG_LINKS(src/Locking/asm:src/Locking/$locking_dir)
  AM_CONDITIONAL([COND_USE_KERNEL_HEADERS], [test "$use_headers" = yes])
fi

####################
#                  #
# Section - output #
#                  #
####################

AC_CONFIG_FILES([
   doc/doxyfile.vds_c
   src/include/doxyfile.vds_c
   src/Packages/buildDeb.sh
   src/Packages/control
   src/Packages/vdsf.spec
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
   src/API/Tests/Folder/Makefile
   src/API/Tests/HashMap/Makefile
   src/API/Tests/Optional/Makefile
   src/API/Tests/Process/Makefile
   src/API/Tests/Queue/Makefile
   src/API/Tests/Session/Makefile
   src/BuildTools/Makefile
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
   src/Engine/Makefile
   src/Engine/Tests/Makefile
   src/Engine/Tests/BlockGroup/Makefile
   src/Engine/Tests/Folder/Makefile
   src/Engine/Tests/Hash/Makefile
   src/Engine/Tests/HashMap/Makefile
   src/Engine/Tests/List/Makefile
   src/Engine/Tests/LogFile/Makefile
   src/Engine/Tests/MemAlloc/Makefile
   src/Engine/Tests/MemBitmap/Makefile
   src/Engine/Tests/MemObject/Makefile
   src/Engine/Tests/Optional/Makefile
   src/Engine/Tests/Process/Makefile
   src/Engine/Tests/ProcessManager/Makefile
   src/Engine/Tests/Queue/Makefile
   src/Engine/Tests/Session/Makefile
   src/Engine/Tests/Transaction/Makefile
   src/include/Makefile
   src/Tests/Makefile
   src/Watchdog/Makefile
   src/Watchdog/Tests/Makefile
])

AC_OUTPUT
