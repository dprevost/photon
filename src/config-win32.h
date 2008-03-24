/*
 * This is a modified version of config.h adapted to the
 * conditions on Win32 (not Windows 64!!!).
 *
 * It must be maintained manually, in synch with the generated
 * config.h.
 *
 * Important warning: some of these macros are probably not set correctly
 *                    and... it works anyway (because some functions say
 *                    mmap() are known issues and the proper win32 equivalent
 *                    is used instead.
 *                    I've tried to put the main macros requiring changes
 *                    (possibly) at the top of this file. YMMV.
 *                    Eventually, unused macros will be removed but not yet
 *                    (the first alpha release will occur in a few days and
 *                    I don't want to introduce bugs...)
 */

#pragma warning(disable:4514) /* unreferenced inline function has been removed */
#pragma warning(disable:4710) /* inline function was not inlined */

#define _WIN32_WINNT 0x0500

/*
 * DBC inserts contract validations (tests for NULL pointers, etc.) in the 
 * code in the form of macros (VDS_PRE_CONDITION and friends). These macros 
 * are defined in src\Common\Common.h.
 *
 * It is recommended to set USE_DBC to 1 on development systems and when
 * evaluating the system. But to set it 0 on production systems.
 */
#define USE_DBC 1

/* This define can be set indirectly by using configure with the option
   --enable-set-block-size=value */
#define VDSE_BLOCK_SHIFT 13

/* This define can be set by using configure with the option
   --enable-set-block-size=value */
#define VDSE_BLOCK_SIZE 8192

/* Version number of package */
#define VERSION "0.1.0"

/* Name of package */
#define PACKAGE "vdsf"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "Daniel Prevost dprevost@users.sourceforge.net"

/* Define to the full name of this package. */
#define PACKAGE_NAME "Virtual Data Space Framework"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "Virtual Data Space Framework 0.1.0"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "vdsf"

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.1.0"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* src/config.h.  Generated by configure.  */
/* src/config.h.in.  Generated from configure.ac by autoheader.  */

/* Define to 1 if you have the `access' function. */
#define HAVE_ACCESS 1
#define HAVE__ACCESS 1

/* asctime_r is ok */
/* #undef HAVE_ASCTIME_R_OK */

/* Define to 1 if you have the <assert.h> header file. */
#define HAVE_ASSERT_H 1

/* Define to 1 if you have the `bcopy' function. */
#define HAVE_BCOPY 1

/* define if bool is a built-in type */
#define HAVE_BOOL 

/* ctime_r is ok */
/* #undef HAVE_CTIME_R_OK */

/* Define to 1 if you have the <ctype.h> header file. */
#define HAVE_CTYPE_H 1

/* Define to 1 if you have the declaration of `strerror_r', and to 0 if you
   don't. */
/* #define HAVE_DECL_STRERROR_R 1 */

/* Define to 1 if you have the <dirent.h> header file, and it defines `DIR'.
   */
#define HAVE_DIRENT_H 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <errno.h> header file. */
#define HAVE_ERRNO_H 1

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the `fdatasync' function. */
#define HAVE_FDATASYNC 0

/* Define to 1 if you have the `fork' function. */
#define HAVE_FORK 1

/* Define to 1 if you have the `fsync' function. */
#define HAVE_FSYNC 0

/* Define to 1 if you have the `getpid' function. */
/* #define HAVE_GETPID 1 */

/* Define to 1 if you have the <inttypes.h> header file. */
/* #define HAVE_INTTYPES_H 1 */

/* Define to 1 if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define to 1 if you have the `localtime_r' function. */
/* #define HAVE_LOCALTIME_R 1 */

/* Define to 1 if you have the `memcpy' function. */
#define HAVE_MEMCPY 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `memset' function. */
#define HAVE_MEMSET 1

/* Define to 1 if you have the `mkdir' function. */
/* #define HAVE_MKDIR 1 */

/* Define to 1 if you have the `mmap' function. */
#define HAVE_MMAP 1

/* Define to 1 if you have the `mprotect' function. */
#define HAVE_MPROTECT 1

/* Define to 1 if you have the `msync' function. */
#define HAVE_MSYNC 1

/* Define to 1 if you have the `nanosleep' function. */
/* #define HAVE_NANOSLEEP 1 */

/* Define to 1 if you have the <ndir.h> header file, and it defines `DIR'. */
/* #undef HAVE_NDIR_H */

/* Define to 1 if you have the <new> header file. */
#define HAVE_NEW 1

/* Define to 1 if you have the <new.h> header file. */
#define HAVE_NEW_H 1

/* Define if you have POSIX threads libraries and header files. */
/* #undef HAVE_PTHREAD */

/* Define to 1 if the system has the type `ptrdiff_t'. */
#define HAVE_PTRDIFF_T 1

/* Define to 1 if you have the <semaphore.h> header file. */
/* #define HAVE_SEMAPHORE_H 1 */

/* Define if sem_init works properly. */
#define HAVE_SEM_INIT 1

/* Define to 1 if you have the <signal.h> header file. */
#define HAVE_SIGNAL_H 1

/* Define to 1 if you have the `stat' function. */
#define HAVE_STAT 1
/* stat is in fact _stat() */
#define HAVE__STAT 1

/* Define to 1 if you have the <stdint.h> header file. */
/* #define HAVE_STDINT_H 1*/

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `strcpy' function. */
#define HAVE_STRCPY 1

/* Define to 1 if you have the `strerror_r' function. */
/* #define HAVE_STRERROR_R 1 */

/* Define to 1 if you have the <strings.h> header file. */
/* #define HAVE_STRINGS_H 0 */

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strnlen' function. */
/* #define HAVE_STRNLEN 1 */

/* Define if your system's sys/sem.h file defines struct semun */
/* #undef HAVE_STRUCT_SEMUN */

/* Define to 1 if you have the <sys/dir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_DIR_H */

/* Define to 1 if you have the <sys/ipc.h> header file. */
/* #define HAVE_SYS_IPC_H 1 */

/* Define to 1 if you have the <sys/mman.h> header file. */
/* #define HAVE_SYS_MMAN_H 1 */

/* Define to 1 if you have the <sys/ndir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_NDIR_H */

/* Define to 1 if you have the <sys/param.h> header file. */
/* #define HAVE_SYS_PARAM_H 1 */

/* Define to 1 if you have the <sys/semaphore.h> header file. */
/* #undef HAVE_SYS_SEMAPHORE_H */

/* Define to 1 if you have the <sys/sem.h> header file. */
/* #define HAVE_SYS_SEM_H 1 */

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
/* #define HAVE_SYS_TIME_H 1 */

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <sys/wait.h> header file. */
/*#define HAVE_SYS_WAIT_H 1 */

/* Define to 1 if you have the <timers.h> header file. */
/* #undef HAVE_TIMERS_H */

/* Define to 1 if you have the <unistd.h> header file. */
/* #define HAVE_UNISTD_H 1 */

/* Define to 1 if you have the `vfork' function. */
/* #define HAVE_VFORK 1 */

/* Define to 1 if you have the <vfork.h> header file. */
/* #undef HAVE_VFORK_H */

/* Define to 1 if you have the `vsnprintf' function. */
/* #define HAVE_VSNPRINTF 1 */

/* Define to 1 if `fork' works. */
/* #define HAVE_WORKING_FORK 1 */

/* Define to 1 if `vfork' works. */
/* #define HAVE_WORKING_VFORK 1 */

/* Define to 1 if the system has the type `_Bool'. */
/* #define HAVE__BOOL 1 */

/* Define to 1 if you have the `_getpid' function. */
#define HAVE__GETPID 1

/* Define to 1 if you have the `_mkdir' function. */
#define HAVE__MKDIR 1

/* Define if mkdir takes only one argument. */
/* #undef MKDIR_TAKES_ONE_ARG */


/* Define to necessary symbol if this constant uses a non-standard name on
   your system. */
/* #undef PTHREAD_CREATE_JOINABLE */

/* Define as the return type of signal handlers (`int' or `void'). */
#define RETSIGTYPE void

/* The size of `void *', as computed by sizeof. */
#define SIZEOF_VOID_P 4

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define to 1 if strerror_r returns char *. */
/* #define STRERROR_R_CHAR_P 1 */

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
/* #define TIME_WITH_SYS_TIME 1 */

/* Using Posix threads */
/* #define USE_PTHREAD 1 */



/* Define to 1 if on AIX 3.
   System headers sometimes define this.
   We just want to avoid a redefinition error message.  */
#ifndef _ALL_SOURCE
/* # undef _ALL_SOURCE */
#endif

/* Define to 1 if on MINIX. */
/* #undef _MINIX */

/* Define to 2 if the system does not provide POSIX.1 features except with
   this defined. */
/* #undef _POSIX_1_SOURCE */

/* Define to 1 if you need to in order for `stat' and other things to work. */
/* #undef _POSIX_SOURCE */

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
#  define inline __inline
/* #undef inline */
#endif

/* Define to `long' if <sys/types.h> does not define. */
/* #undef off_t */

/* Define to `int' if <sys/types.h> does not define. */
/* #undef pid_t */
typedef int pid_t;

/* Define to `unsigned' if <sys/types.h> does not define. */
/* #undef size_t */

/* Define as `fork' if `vfork' does not work. */
/* #undef vfork */

/* Define to empty if the keyword `volatile' does not work. Warning: valid
   code using `volatile' can become incorrect without. Disable with care. */
/* #undef volatile */

/*
 * VC++ 6 does not support the C99 specs for printf (for size_t
 * and ptrdiff_t). Quite normal since this compiler is quite old.
 *
 * The online documentation for VS 2008 does not mention support
 * for these 2 formats either. This is stranger but oh well...
 */
/* # define HAVE_PRINTF_SIZE_T 1 */
/* # define HAVE_PRINTF_PTRDIFF_T 1 */

