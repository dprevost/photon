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

#ifndef VDSC_THREAD_LOCK_H
#define VDSC_THREAD_LOCK_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Common.h"

BEGIN_C_DECLS

#define VDSC_THREADLOCK_SIGNATURE ((unsigned int)0x9f009f91 )

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

VDSF_COMMON_EXPORT
extern int g_timeOutinMilliSecs;

VDSF_COMMON_EXPORT
extern struct timespec g_timeOut;

typedef struct vdscThreadLock
{
   unsigned int initialized;

#if defined (WIN32)
   CRITICAL_SECTION mutex;
#else
   pthread_mutex_t mutex;
//   pthread_mutexattr_t attr;
#endif

} vdscThreadLock;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Initialize the lock - used for POSIX semaphore since sem_init()
 * may return an error. If isShared is true, the lock will be valid
 * for all processes that share it - otherwise the lock is only valid
 * for the process owning it (if the underlying platform specific 
 * mechanism allows this distinction).
 */
VDSF_COMMON_EXPORT
int vdscInitThreadLock( vdscThreadLock* pLock );

/**
 *  Uninitialize the lock (it will remove the lock for POSIX semaphores).
 */
VDSF_COMMON_EXPORT
void vdscFiniThreadLock( vdscThreadLock* pLock );

/** Acquire lock ownership (loop forever) - this is dangerous for
 * deadlocks.
 */
static inline
void vdscAcquireThreadLock( vdscThreadLock* pLock );

/** Attempt to acquire the lock for nMilliSecs - fails if it can't 
 *  Returns -1 on failure.  
 */
static inline
int vdscTryAcquireThreadLock ( vdscThreadLock* pLock,
                               unsigned int milliSecs );   

/** Release lock. */
static inline
void vdscReleaseThreadLock ( vdscThreadLock* pLock );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "ThreadLock.inl"

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSC_THREAD_LOCK_H */
