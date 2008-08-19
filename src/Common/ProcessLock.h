/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of photon (photonsoftware.org).
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

#ifndef PSC_PROCESS_LOCK_H
#define PSC_PROCESS_LOCK_H

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
 *   PSO_ProcessLock::lock (this does not occur in the inline assembler 
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

#define PSC_LOCK_SIGNATURE ((unsigned int)0x174a0c46 )

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* 
 * Choose how we will implement our locking.
 */

//#if !defined(CONFIG_KERNEL_HEADERS) && ! defined (WIN32)
//#  define PSO_USE_POSIX_SEMAPHORE
//#endif

/* Override the macro for testing purposes. Change the #if 0 to
 * # if 1 to uncomment and replace the macro
 */

#if 0
   /* replace by an appropriate macro for the test */
#  define PSO_USE_POSIX_SEMAPHORE 
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * The next struct eliminates a problem when using POSIX semaphores on at
 * least Tru64 (and possibly other RISC-based platforms) by forcing the 
 * semaphore structure to be 64-bit aligned.
 */
#if defined(PSO_USE_POSIX_SEMAPHORE)
union sem_align
{
   long junk;
   sem_t sem;
};
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Some sanity checks - do not remove, please. */

#if defined(PSO_USE_POSIX_SEMAPHORE) && !defined(HAVE_SEM_INIT)
#  error "sem_init() does not work! Help!"
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* unsigned int is ok for all OSes so far... but ... */

typedef volatile unsigned int pso_lock_T;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

VDSF_COMMON_EXPORT
extern int g_timeOutinMilliSecs;

VDSF_COMMON_EXPORT
extern struct timespec g_timeOut;

typedef struct pscProcessLock
{
   unsigned int initialized;

   pso_lock_T lock;

#if defined (PSO_USE_POSIX_SEMAPHORE)
   union sem_align  semaphore;
#endif

   /*
    * The pid and the counter are used together to determine
    * if a lock is currently held by a non-existing process (crashed).
    */
   pid_t pid;
   
   int counter;

} pscProcessLock;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Initialize the lock - used for POSIX semaphore since sem_init()
 * may return an error.
 */
VDSF_COMMON_EXPORT
bool pscInitProcessLock( pscProcessLock * pLock );

/**
 *  Uninitialize the lock (it will remove the lock for POSIX semaphores).
 */
VDSF_COMMON_EXPORT
bool pscFiniProcessLock( pscProcessLock * pLock );

/** Acquire lock ownership (loop forever) - this is dangerous for
 * deadlocks.
 */
static inline
void pscAcquireProcessLock( pscProcessLock * pLock,
                            pid_t            pidLocker );

/** Attempt to acquire the lock for nMilliSecs - fails if it can't 
 *  Returns -1 on failure.  
 */
static inline
bool pscTryAcquireProcessLock( pscProcessLock * pLock,
                               pid_t            pidLocker,
                               unsigned int     milliSecs );   

/** Release lock. */
static inline
void pscReleaseProcessLock( pscProcessLock * pLock );

/**
 * Test the underlying value of the pid.
 * Returns a boolean value (1 if the pids are the same, 0 otherwise).
 */
VDSF_COMMON_EXPORT
bool pscTestLockPidValue( pscProcessLock * pLock, pid_t pid );

/**
 * Test to see if the lock is on.
 * Returns a boolean value (1 if the lock is indeed locked, 0 otherwise).
 */
VDSF_COMMON_EXPORT
bool pscIsItLocked( pscProcessLock * pLock );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined (PSO_USE_POSIX_SEMAPHORE)
#  include "Common/arch/Lock-semaphore.h"
#elif defined (WIN32)
#  include "Common/arch/Lock-win32.h"
#elif defined (PSO_USE_I386_GCC)
#  include "Common/arch/Lock-i386-gcc.h"
#elif defined (PSO_USE_X86_64_GCC)
#  include "Common/arch/Lock-x86_64-gcc.h"
#elif defined (PSO_USE_SPARC_GCC)
#  include "Common/arch/Lock-sparc-gcc.h"
#elif defined (PSO_USE_PPC_GCC)
#  include "Common/arch/Lock-ppc-gcc.h"
#elif defined(PSO_USE_PPC_XLC)
#  include "Common/arch/Lock-ppc-xlc.h"
#else
#  error "Not implemented yet!"
#endif

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSC_PROCESS_LOCK_H */

