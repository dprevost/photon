/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of the vdsf (Virtual Data Space Framework) Library.
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation and appearing in the file COPYING included in the
 * packaging of this library.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

/**
 * \defgroup vdscCommon The Group of all modules in Common
 *
 * Modules in this group provides wrappers for C library function and 
 * low-level functionalities used by the different parts that makes VDSF.
 *
 */

#if defined WIN32
#  define FD_SETSIZE 100
#endif

#ifndef VDSC_COMMON_H
#define VDSC_COMMON_H

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif
#if defined WIN32
#  include "config-win32.h"
   /* 
    * The pragma is to hide a warning in Microsoft include files
    * with VC++ 6. Don't know about other versions yet.
    */
#  pragma warning(disable:4115)
#  include <windows.h>
#  pragma warning(default:4115)
#  include <winbase.h>
#  include <io.h>
#  include <process.h> 
#endif

#if defined WIN32
#  ifdef BUILD_VDSF_COMMON
#    define VDSF_COMMON_EXPORT __declspec ( dllexport )
#  else
#    define VDSF_COMMON_EXPORT __declspec ( dllimport )
#  endif
#else
#  define VDSF_COMMON_EXPORT
#endif

#if  ! defined ( BEGIN_C_DECLS )
#  ifdef __cplusplus
#    define BEGIN_C_DECLS extern "C" {
#    define END_C_DECLS   }
#  else /* !__cplusplus */
#    define BEGIN_C_DECLS
#    define END_C_DECLS
#  endif /* __cplusplus */
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Standard tests - they do not require an autoconf macro */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include <stdio.h>
#if HAVE_SYS_TYPES_H
#  include <sys/types.h>
#endif
#if HAVE_SYS_STAT_H
#  include <sys/stat.h>
#endif
#if STDC_HEADERS
#  include <stdlib.h>
#  include <stddef.h>
#  include <stdarg.h>
#else
#  if HAVE_STDLIB_H
#    include <stdlib.h>
#  endif
#endif
#if HAVE_STRING_H
#  if !STDC_HEADERS && HAVE_MEMORY_H
#    include <memory.h>
#  endif
#  include <string.h>
#endif
#if HAVE_STRINGS_H
#  include <strings.h>
#endif

#if HAVE_INTTYPES_H
#  include <inttypes.h>
#elif HAVE_STDINT_H
#  include <stdint.h>
#else
#  if defined(WIN32)
typedef DWORD uint32_t;
#  endif
#endif

#if HAVE_UNISTD_H
#  include <unistd.h>
#endif
#if HAVE_STDBOOL_H
#  include <stdbool.h>
#endif

#if HAVE_SYS_WAIT_H
# include <sys/wait.h>
#endif
#ifndef WEXITSTATUS
# define WEXITSTATUS(stat_val) ((unsigned)(stat_val) >> 8)
#endif
#ifndef WIFEXITED
# define WIFEXITED(stat_val) (((stat_val) & 255) == 0)
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* End of standard tests that do not require an autoconf macro */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if USE_PTHREAD
# include <pthread.h>
#else
#  if ! defined(WIN32)
#    error Need to include Posix Thread somehow!!!
#  endif
#endif

#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#if HAVE_ASSERT_H
#  include <assert.h>
#endif

#if HAVE_SYS_MMAN_H
#  include <sys/mman.h>
#endif

#if !STDC_HEADERS
#  if !HAVE_MEMCPY
#    define memcpy(d, s, n)   bcopy ((s), (d), (n))
#    define memmove(d, s, n)  bcopy ((s), (d), (n))
#  endif
#endif

#if HAVE_CTYPE_H
#  include <ctype.h>
#endif
#if HAVE_LIMITS_H
#  include <limits.h>
#endif
#if HAVE_SYS_PARAM_H
#  include <sys/param.h>
#endif

#ifndef PATH_MAX
#  if defined MAXPATHLEN 
#    define PATH_MAX MAXPATHLEN 
#  elif defined _POSIX_PATH_MAX
#    define PATH_MAX _POSIX_PATH_MAX
#  elif defined _MAX_PATH
#    define PATH_MAX  _MAX_PATH
#  else
#    define PATH_MAX 1024
# endif
#endif

#if HAVE_ERRNO_H
#  include <errno.h>
#endif

#if defined WIN32
#  define VDS_DIR_SEPARATOR "\\"
#else
#  define VDS_DIR_SEPARATOR "/"
#endif

#if HAVE_FCNTL_H
#  include <fcntl.h>
#endif

#if HAVE__STAT   /* Win32 */
#  define stat(a, b) _stat(a, b)
#endif

#ifdef HAVE_ACCESS
#  ifdef HAVE__ACCESS
#    define access(a,b) _access(a,b)
#    ifndef F_OK
#      define F_OK 00
#      define R_OK 04
#      define W_OK 06
#    endif
#  endif
#else
#  error Need to write a wrapper for access()!!!
#endif

/* From Alexandre Duret-Lutz <adl@gnu.org> */

#if HAVE_MKDIR
#  if MKDIR_TAKES_ONE_ARG   /* MinGW32 */
#    define mkdir(a, b) mkdir(a)
#  endif
#else
#  if HAVE__MKDIR   /* plain Windows 32 */
#    include <direct.h>
#    define mkdir(a, b) _mkdir(a)
#  else
#    error "Don't know how to create a directory on this system."
#  endif
#endif

#if HAVE_GETPID
#elif HAVE__GETPID  /* Windows 32 */
#  define getpid() _getpid()
#else
#error "Don't know how to get the pid on this system."
#endif

/* fdatasync is part of th posix1b standard... but... might not
 * be implemented everywhere. On WIN32, _commit will do...
 */
#if !HAVE_FDATASYNC
#  if HAVE_FSYNC
/* Not sure if there are platforms with fsync and without fdatasync */
#    define  fdatasync(fd) fsync(fd)
#  elif defined (WIN32)
/* should we use FlushFileBuffers() instead? */
#    define fdatasync(fd) _commit(fd)
#  else
#    error "Don't know how to synch file(s) to disk."
#  endif
#endif

#if !HAVE_STRCPY
#  if HAVE_BCOPY
#    define strcpy(d, s)  bcopy (s, d, 1+strlen (s))
#  else
#    error no strcpy or bcopy
#  endif
#endif

#if !HAVE_MMAP
#  if !defined (WIN32)
#    error "Don't know how to mmap files to memory."
#  endif
#endif

#if !HAVE_PTRDIFF_T
typedef size_t ptrdiff_t;
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- *
 *
 * i18n - internationalization is optional (it can be suppressed when
 *        running configure by using --disable-i18n) and will default to 
 *        single byte chars if some features are missing.
 *
 *        configure will check for missing header files and missing 
 *        functions. As additional functions will be used, they must
 *        be added to configure.ac in the i18n section.
 *
 * --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Simple if. If all the functions/headers we need are present AND the 
 * option was not turned off when running configure... we have i18n! 
 */

#if VDS_SUPPORT_i18n

#  include <wchar.h>
#  include <wctype.h>

typedef wchar_t vdsChar_T;

#  define vds_isalnum(c) iswalnum(c)
#  define vds_tolower(c) towlower(c)

#  define VDS_BACKSLASH  L'\\'
#  define VDS_SLASH      L'/'
#  define VDS_SPACE      L' '
#  define VDS_UNDERSCORE L'_'
#  define VDS_HYPHEN     L'-'

#else /* VDS_SUPPORT I18N */

typedef char vdsChar_T;

#  define vds_isalnum(c) isalnum(c)
#  define vds_tolower(c) tolower(c)

#  define VDS_BACKSLASH  '\\'
#  define VDS_SLASH      '/'
#  define VDS_SPACE      ' '
#  define VDS_UNDERSCORE '_'
#  define VDS_HYPHEN     '-'

#endif /* VDS_SUPPORT I18N */

BEGIN_C_DECLS

/*
 * Prototypes for missing prototypes
 */

#if HAVE_STRERROR_R
#  if ! HAVE_DECL_STRERROR_R
#    if STRERROR_R_CHAR_P
char* strerror_r(int errnum, char *buf, size_t n);
#    else
int strerror_r(int errnum, char *buf, size_t n);
#    endif
#  endif
#endif

/*
 * Prototypes for missing functions
 */

#ifndef HAVE_MEMCMP
extern int memcmp( const void* str1, const void* str2, size_t count);
#endif

#ifndef HAVE_MEMSET
extern void *memset( void* dest, int val, size_t len);
#endif

#ifndef HAVE_STRNLEN
extern size_t strnlen(const char *s, size_t maxlen);
#endif

#ifndef HAVE_NANOSLEEP
struct timespec 
{
  time_t tv_sec;
  long tv_nsec;
};

VDSF_COMMON_EXPORT
extern int nanosleep(const struct timespec * pRequest, 
                     struct timespec       * pRemain );
#endif

#ifndef HAVE_LOCALTIME_R
VDSF_COMMON_EXPORT
extern struct tm *localtime_r( const time_t * timep,
                               struct tm *		tm );
#endif

#ifdef HAVE_ASCTIME_R_OK
#  define new_asctime_r(a, b, c) asctime_r ((a), (b), (c))
#else
extern char *new_asctime_r( const struct tm *, char *, int );
#endif

#ifdef HAVE_CTIME_R_OK
#  define new_ctime_r(a, b, c) ctime_r ( (a), (b), (c))
#else
extern char *new_ctime_r( const time_t *timep, char *buf, int buflen );
#endif

/****************************************************************/

#ifdef USE_EXTREME_DBC
#  ifndef USE_DBC
#    define USE_DBC
#  endif
#endif

#ifdef USE_DBC
#  if !HAVE_ASSERT_H
#    error "assert.h is needed to implement Design By Contract"
#  endif
/*
 * Some issue on Win32. The tests always generate a popup because 
 * of abort(). So, I'll replace abort() with _exit(). 
 * We use the exit code of abort.
 */
#  if defined(WIN32)
#    define ABORT() exit(3)
#  else
#    define ABORT() abort()
#  endif
#  define VDS_PRE_CONDITION(x) \
   if ( ! (x) ) { \
      fprintf( stderr, "%s (\"%s\") failed in file %s at line %d\n", \
         "DBC: precondition", #x, __FILE__, __LINE__ ); \
      ABORT(); \
   }
#  define VDS_POST_CONDITION(x) \
   if ( ! (x) ) { \
      fprintf( stderr, "%s (\"%s\") failed in file %s at line %d\n", \
         "DBC: postcondition", #x, __FILE__, __LINE__ ); \
      ABORT(); \
   }
#  define VDS_INV_CONDITION(x) \
   if ( ! (x) ) { \
      fprintf( stderr, "%s (\"%s\") failed in file %s at line %d\n", \
         "DBC: invariant", #x, __FILE__, __LINE__ ); \
      ABORT(); \
   }
#else
#  define VDS_PRE_CONDITION(x) 
#  define VDS_POST_CONDITION(x)
#  define VDS_INV_CONDITION(x)
#endif

/*
 * Define our own boolean type. This might be overkill to some extent
 * since many C compilers will handle the new bool type of C. But...
 *
 * Note: the bool used to be defined as an enum. But... this created a 
 * conflict for c++ code. Using defines and a typedef eliminated this issue.
 */

#ifndef __cplusplus
#  if !defined (HAVE__BOOL)
#    define false 0
#    define true  1
typedef int bool;
#  endif
#endif

#if defined (WIN32)
#  define VDS_INVALID_HANDLE (NULL)
#else
#  define VDS_INVALID_HANDLE (-1)
#endif

#if ! defined (MAP_FAILED)
#  if defined (WIN32)
#    define VDS_MAP_FAILED NULL
#  else
#    define VDS_MAP_FAILED ((void *) -1)
#  endif
#else
#  define VDS_MAP_FAILED MAP_FAILED
#endif

struct vdstTestAlignment
{
   unsigned char c;
   struct vdstTestAlignment2
   {
      double d;
   } two;
};

#define VDST_STRUCT_ALIGNMENT offsetof(struct vdstTestAlignment, two)

/****************************************************************/
/*
 * Some compilers might not support the C99 specs for printf (for size_t
 * and ptrdiff_t) so a m4 macro was written to test for this.
 *
 * The online documentation for VS 2008 does not mention support
 * for these format so I can't based
 */
#if HAVE_PRINTF_SIZE_T
#  define VDSF_SIZE_T_FORMAT    "%zd"
#else
#  if SIZEOF_VOID_P == 4
#    define VDSF_SIZE_T_FORMAT    "%d"
#  else
#    define VDSF_SIZE_T_FORMAT    "%ld"
#  endif
#endif

#if HAVE_PRINTF_PTRDIFF_T
#  define VDSF_PTRDIFF_T_FORMAT "%td"
#else
#  if SIZEOF_VOID_P == 4
#    define VDSF_PTRDIFF_T_FORMAT "%d"
#  else
#    define VDSF_PTRDIFF_T_FORMAT "%ld"
#  endif
#endif

END_C_DECLS

#endif /* VDSC_COMMON_H */
