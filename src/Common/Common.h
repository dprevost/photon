/*
 * Copyright (C) 2006-2009 Daniel Prevost <dprevost@photonsoftware.org>
 *
 * This file is part of Photon (photonsoftware.org).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 or version 3 as published by the 
 * Free Software Foundation and appearing in the file COPYING.GPL2 and 
 * COPYING.GPL3 included in the packaging of this software.
 *
 * Licensees holding a valid Photon Commercial license can use this file 
 * in accordance with the terms of their license.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

/**
 * \defgroup psocCommon The Group of all modules in Common
 *
 * Modules in this group provides wrappers for C library function and 
 * low-level functionalities used by the different parts that makes Photon.
 *
 */

#ifndef PSOC_COMMON_H
#define PSOC_COMMON_H

#include "config.h"
#if defined WIN32
#  ifndef FD_SETSIZE
#    define FD_SETSIZE 100
#  endif
#  include <Winsock2.h>
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
#  ifdef BUILD_PHOTON_COMMON
#    define PHOTON_COMMON_EXPORT __declspec ( dllexport )
#  else
#    define PHOTON_COMMON_EXPORT __declspec ( dllimport )
#  endif
#else
#  define PHOTON_COMMON_EXPORT
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
#include <stdlib.h>
#include <stddef.h>

#if HAVE_SYS_TYPES_H
#  include <sys/types.h>
#endif
#if HAVE_SYS_STAT_H
#  include <sys/stat.h>
#endif
#if HAVE_STDARG_H
#  include <stdarg.h>
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

#if ! defined(WIN32)
#  include <pthread.h>
#endif

# if HAVE_SYS_TIME_H
#  include <sys/time.h>
#endif

#if HAVE_TIME_H
#  include <time.h>
#endif

#if HAVE_ASSERT_H
#  include <assert.h>
#endif

#if HAVE_SYS_MMAN_H
#  include <sys/mman.h>
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

#if HAVE_GOOGLE_CMOCKERY_H
#  include <setjmp.h>
#  include <google/cmockery.h>
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
#  define PSO_DIR_SEPARATOR "\\"
#else
#  define PSO_DIR_SEPARATOR "/"
#endif

#if HAVE_FCNTL_H
#  include <fcntl.h>
#endif

//#if HAVE__STAT   /* Win32 */
//#  define stat(a, b) _stat(a, b)
//#endif

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

//#if HAVE__MKDIR   /* plain Windows 32 */
//#  include <direct.h>
//#  define mkdir(a, b) _mkdir(a)
//#endif

//#if HAVE_GETPID
//#elif HAVE__GETPID  /* Windows 32 */
//#  define getpid() _getpid()
//#else
//#error "Don't know how to get the pid on this system."
//#endif

/* fdatasync is part of th posix1b standard... but... might not
 * be implemented everywhere. On WIN32, _commit will do...
 */
//#if !HAVE_FDATASYNC
//#  if HAVE_FSYNC
/* Not sure if there are platforms with fsync and without fdatasync */
//#    define  fdatasync(fd) fsync(fd)
//#  elif defined (WIN32)
/* should we use FlushFileBuffers() instead? */
//#    define fdatasync(fd) _commit(fd)
//#  else
//#    error "Don't know how to synch file(s) to disk."
//#  endif
//#endif

//#if !HAVE_MMAP
//#  if !defined (WIN32)
//#    error "Don't know how to mmap files to memory."
//#  endif
//#endif

#if !HAVE_PTRDIFF_T
typedef size_t ptrdiff_t;
#endif

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

/**
 * A wrapper for our sleep function. This way, we can modify it if needed
 * on some platforms.
 */
static inline
void psocLockSleep(const struct timeval * timeOut)
{
   struct timeval req;
   
   req.tv_sec  = timeOut->tv_sec;
   req.tv_usec = timeOut->tv_usec;
   select( 1, 0, 0, 0, &req );
}


#ifndef HAVE_LOCALTIME_R
PHOTON_COMMON_EXPORT
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

#ifdef USE_DBC
#  if !HAVE_ASSERT_H
#    error "assert.h is needed to implement Design By Contract"
#  endif

#  if HAVE_GOOGLE_CMOCKERY_H
#    define PSO_PRE_CONDITION(x) \
       mock_assert((int)(x), #x, __FILE__, __LINE__); 
#    define PSO_POST_CONDITION(x) \
       mock_assert((int)(x), #x, __FILE__, __LINE__); 
#    define PSO_INV_CONDITION(x) \
       mock_assert((int)(x), #x, __FILE__, __LINE__); 

#  else
   /*
    * Some issue on Win32. The tests always generate a popup because 
    * of abort(). So, I'll replace abort() with _exit(). 
    * We use the exit code of abort.
    */
#    if defined(WIN32)
#      define ABORT() exit(3)
#    else
#      define ABORT() abort()
#    endif
#    define PSO_PRE_CONDITION(x) \
     if ( ! (x) ) { \
      fprintf( stderr, "%s (\"%s\") failed in file %s at line %d\n", \
         "DBC: precondition", #x, __FILE__, __LINE__ ); \
      ABORT(); \
   }
#    define PSO_POST_CONDITION(x) \
   if ( ! (x) ) { \
      fprintf( stderr, "%s (\"%s\") failed in file %s at line %d\n", \
         "DBC: postcondition", #x, __FILE__, __LINE__ ); \
      ABORT(); \
   }
#    define PSO_INV_CONDITION(x) \
   if ( ! (x) ) { \
      fprintf( stderr, "%s (\"%s\") failed in file %s at line %d\n", \
         "DBC: invariant", #x, __FILE__, __LINE__ ); \
      ABORT(); \
   }
#  endif
#else
#  define PSO_PRE_CONDITION(x) 
#  define PSO_POST_CONDITION(x)
#  define PSO_INV_CONDITION(x)
#endif

/*
 * Define our own boolean type. This might be overkill to some extent
 * since many C compilers will handle the new bool type of C. But...
 *
 * Note: the bool used to be defined as an enum. But... this created a 
 * conflict for c++ code. Using defines and a typedef eliminated this issue.
 */

#ifndef __cplusplus
#define HAVE__BOOL 1
#  if !defined (HAVE__BOOL)
#    define false 0
#    define true  1
typedef int bool;
#  endif
#endif

#if defined (WIN32)
#  define PSO_INVALID_HANDLE (NULL)
#else
#  define PSO_INVALID_HANDLE (-1)
#endif

#if ! defined (MAP_FAILED)
#  if defined (WIN32)
#    define PSO_MAP_FAILED NULL
#  else
#    define PSO_MAP_FAILED ((void *) -1)
#  endif
#else
#  define PSO_MAP_FAILED MAP_FAILED
#endif

/****************************************************************/

/* Defined fixed types floating points */

#if SIZEOF_FLOAT == 4
typedef float psoFloat32;
#else
#error "Don't know how to handle 32-bits floating point"
#endif

#if SIZEOF_DOUBLE == 8
typedef double psoFloat64;
#elif SIZEOF_LONG_DOUBLE == 8
typedef long double psoFloat64;
#error "Don't know how to handle 64-bits floating point"
#endif

/****************************************************************/

struct psocTestAlignmentStruct
{
   unsigned char c;
   struct psocTestAlignment2
   {
      double d;
   } two;
};

#define PSOC_ALIGNMENT_STRUCT offsetof(struct psocTestAlignmentStruct, two)

struct psocTestAlignmentChar
{
   unsigned char c1;
   unsigned char c2;
};

#define PSOC_ALIGNMENT_CHAR offsetof(struct psocTestAlignmentChar, c2)

struct psocTestAlignmentCharArray
{
   unsigned char c1;
   unsigned char array[11];
};

#define PSOC_ALIGNMENT_CHAR_ARRAY offsetof(struct psocTestAlignmentCharArray, array)

struct psocTestAlignmentInt16
{
   unsigned char c;
   uint16_t i16;
};

#define PSOC_ALIGNMENT_INT16 offsetof(struct psocTestAlignmentInt16, i16)

struct psocTestAlignmentInt32
{
   unsigned char c;
   uint32_t i32;
};

#define PSOC_ALIGNMENT_INT32 offsetof(struct psocTestAlignmentInt32, i32)

struct psocTestAlignmentInt64
{
   unsigned char c;
   uint64_t i64;
};

#define PSOC_ALIGNMENT_INT64 offsetof(struct psocTestAlignmentInt64, i64)

struct psocTestAlignmentBool
{
   unsigned char c;
   bool b;
};

#define PSOC_ALIGNMENT_BOOL offsetof(struct psocTestAlignmentBool, b)

/****************************************************************/
/*
 * Some compilers might not support the C99 specs for printf (for size_t
 * and ptrdiff_t) so a m4 macro was written to test for this.
 *
 * The online documentation for VS 2008 does not mention support
 * for these format so I can't based
 */
#if HAVE_PRINTF_SIZE_T
#  define PSO_SIZE_T_FORMAT    "%zd"
#else
#  if SIZEOF_VOID_P == 4
#    define PSO_SIZE_T_FORMAT    "%d"
#  else
#    define PSO_SIZE_T_FORMAT    "%ld"
#  endif
#endif

#if HAVE_PRINTF_PTRDIFF_T
#  define PSO_PTRDIFF_T_FORMAT "%td"
#else
#  if SIZEOF_VOID_P == 4
#    define PSO_PTRDIFF_T_FORMAT "%d"
#  else
#    define PSO_PTRDIFF_T_FORMAT "%ld"
#  endif
#endif

END_C_DECLS

#endif /* PSOC_COMMON_H */
