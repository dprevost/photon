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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifndef PSOC_THREAD_LOCK_H
#define PSOC_THREAD_LOCK_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Common.h"

BEGIN_C_DECLS

#define PSOC_THREADLOCK_SIGNATURE ((unsigned int)0x9f009f91 )

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

PHOTON_COMMON_EXPORT
extern int g_timeOutinMilliSecs;

PHOTON_COMMON_EXPORT
extern struct timeval g_timeOut;

typedef struct psocThreadLock
{
   unsigned int initialized;

#if defined (WIN32)
   CRITICAL_SECTION mutex;
#else
   pthread_mutex_t mutex;
#endif

} psocThreadLock;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Initialize the lock - used for POSIX semaphore since sem_init()
 * may return an error. If isShared is true, the lock will be valid
 * for all processes that share it - otherwise the lock is only valid
 * for the process owning it (if the underlying platform specific 
 * mechanism allows this distinction).
 */
PHOTON_COMMON_EXPORT
bool psocInitThreadLock( psocThreadLock * pLock );

/**
 *  Uninitialize the lock (it will remove the lock for POSIX semaphores).
 */
PHOTON_COMMON_EXPORT
void psocFiniThreadLock( psocThreadLock * pLock );

/** Acquire lock ownership (loop forever) - this is dangerous for
 * deadlocks.
 */
static inline
void psocAcquireThreadLock( psocThreadLock * pLock );

/** Attempt to acquire the lock for nMilliSecs - fails if it can't 
 *  Returns false on failure.  
 */
static inline
bool psocTryAcquireThreadLock ( psocThreadLock * pLock,
                                unsigned int     milliSecs );   

/** Release lock. */
static inline
void psocReleaseThreadLock ( psocThreadLock * pLock );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "ThreadLock.inl"

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSOC_THREAD_LOCK_H */

