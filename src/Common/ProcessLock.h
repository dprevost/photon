/*
 * Copyright (C) 2006 Daniel Prevost <dprevost@users.sourceforge.net>
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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifndef VDSC_PROCESS_LOCK_H
#define VDSC_PROCESS_LOCK_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
/*
 * Different mechanisms can be used to provide synchronization for our
 * objects in shared memory. Here are a few tidbits to explain some of
 * the issues when using specific mechanisms.
 *
 * POSIX semaphores:
 *
 * - The number of semaphores is limited to SEM_VALUE_MAX. Not sure if 
 *   that could create a problem on some systems.
 *
 * - When (re)starting the watchdog, the semaphores should be initialized
 *   for all existing objects (with locks) in the backing store.
 *
 * - In a similar fashion, when shutting down the watchdog, all semaphores
 *   should be destroyed.
 *
 * - There is a race condition for setting the pid in the lock object,
 *   VDS_ProcessLock::lock (this does not occur in the inline assembler 
 *   since we use the pid as the lock value in the atomic operation). 
 *   To be able to handle this, a special attribute in the ContextObject
 *   must be set to indicate that a lock is going to be attempted. This 
 *   way, if a process crash, the watchdog will have enough information 
 *   to discover if an object was locked by the crashee.
 *
 */
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Common.h"

BEGIN_C_DECLS

/*
 * We include all the header files (if present) even if we won't use them
 * on a specific platform/compiler (this way we can test say SYSV semaphores
 * on systems where POSIX semaphores would have been used instead).
 *
 * Notes:
 *
 * 1) I need a 2.6 kernel to try the futex...
 * 2) Could we use the kernel spinlocks? (including stuff from the header
 *    files of the kernel is not really recommended but...).
 */

#if HAVE_SEMAPHORE_H
#  include <semaphore.h>
#endif
#if HAVE_SYS_IPC_H && HAVE_SYS_SEM_H
#  include <sys/ipc.h>
#  include <sys/sem.h>
#  if ! HAVE_STRUCT_SEMUN
union semun {
   int val;                    /* value for SETVAL */
   struct semid_ds *buf;       /* buffer for IPC_STAT, IPC_SET */
   unsigned short int *array;  /* array for GETALL, SETALL */
   struct seminfo *__buf;      /* buffer for IPC_INFO */
};
#  endif
#endif
#if HAVE_LINUX_FUTEX_H
#  include <linux/futex.h>
#endif

#define VDSC_LOCK_SIGNATURE ((unsigned int)0x174a0c46 )

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* 
 * Choose how we will implement our locking.
 */

#if defined (WIN32)
#  define VDS_USE_TRY_ACQUIRE
#elif defined (  __GNUC__ )
#  if defined (__i386) ||  defined (__i386__) || defined(__sparc)
#    define VDS_USE_TRY_ACQUIRE
#  endif
#elif defined ( __HP_aCC )
#  define VDS_USE_HP_LOCK
#endif
#if !defined (VDS_USE_TRY_ACQUIRE) && !defined( VDS_USE_HP_LOCK )
#  define VDS_USE_POSIX_SEMAPHORE
#endif

/* Override the macro for testing purposes. Change the #if 0 to
 * # if 1 to uncomment and replace the macro
 */

#if 0
#  undef VDS_USE_TRY_ACQUIRE
#  undef VDS_USE_POSIX_SEMAPHORE
   /* replace by an appropriate macro for the test */
#  define VDS_???  
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * The next struct eliminates a problem when using POSIX semaphores on at
 * least Tru64 (and possibly other RISC-based platforms) by forcing the 
 * semaphore structure to be 64-bit aligned.
 */
#if defined(VDS_USE_POSIX_SEMAPHORE)
union sem_align
{
   long junk;
   sem_t sem;
};
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Some sanity checks - do not remove, please. */

#if defined(VDS_USE_POSIX_SEMAPHORE) && !defined(HAVE_SEM_INIT)
#  error "sem_init() does not work! Help!"
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* unsigned long is ok for all OSes so far... but ... */

typedef volatile unsigned int vds_lock_T;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* This value is perfectly alright for locks allocated on the stack 
 * or on the heap. It is therefore perfect for the "os monitor lock"
 * (to make non-reentrant calls reentrant), for the signal handler 
 * classes, etc.
 * However it should not be used for shared memory. The lockValue 
 * used in shared memory must be distinctive enough to enable a cleanup
 * if a process crashes.
 */
#define VDS_LOCK_VALUE ( (vds_lock_T) 0xff )

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

VDSF_COMMON_EXPORT
extern int g_timeOutinMilliSecs;

VDSF_COMMON_EXPORT
extern struct timespec g_timeOut;

typedef int pid_t;

typedef struct vdscProcessLock
{
   unsigned int initialized;

#if defined (VDS_USE_HP_LOCK)

   /**
    *  HP (PA-RISC) requires that the variable used for locking be aligned
    *  on a 16-byte boundary.
    */
   char lockArray[16];
   vds_lock_T* pLock;
   
#else
   /** Might be used to hold the pid and not for locking */
   vds_lock_T lock;
#endif

#if defined (VDS_USE_POSIX_SEMAPHORE)
   sem_align  semaphore;
#endif

   /*
    * The pid and the counter are used together to determine
    * if a lock is currently held by a non-existing process (crashed).
    */
   pid_t pid;
   
   int counter;

} vdscProcessLock;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Initialize the lock - used for POSIX semaphore since sem_init()
 * may return an error.
 */
VDSF_COMMON_EXPORT
int vdscInitProcessLock( vdscProcessLock* pLock );

/**
 *  Uninitialize the lock (it will remove the lock for POSIX semaphores).
 */
VDSF_COMMON_EXPORT
int vdscFiniProcessLock( vdscProcessLock* pLock );

/** Acquire lock ownership (loop forever) - this is dangerous for
 * deadlocks.
 */
static inline
void vdscAcquireProcessLock( vdscProcessLock* pLock,
                             vds_lock_T lockValue );

/** Attempt to acquire the lock for nMilliSecs - fails if it can't 
 *  Returns -1 on failure.  
 */
static inline
int vdscTryAcquireProcessLock ( vdscProcessLock* pLock,
                                vds_lock_T   lockValue,
                                unsigned int milliSecs );   

/** Release lock. */
static inline
void vdscReleaseProcessLock ( vdscProcessLock* pLock );

/**
 * Test the underlying value of the pid.
 * Returns a boolean value (1 if the pids are the same, 0 otherwise).
 */
static inline
int vdscTestLockPidValue( vdscProcessLock* pLock, pid_t pid );

/**
 * Test to see if the lock is on.
 * Returns a boolean value (1 if the lock is indeed locked, 0 otherwise).
 */
static inline
int vdscIsItLocked( vdscProcessLock* pLock );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "ProcessLock.inl"

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSC_PROCESS_LOCK_H */
