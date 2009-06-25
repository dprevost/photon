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

#include "ProcessLock.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

PHOTON_COMMON_EXPORT
int g_timeOutinMilliSecs = 5;

struct timeval g_timeOut = { 0, 5000 };

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psocTestLockPidValue( psocProcessLock * pLock, pid_t pid )
{
   PSO_PRE_CONDITION( pLock != NULL );
   PSO_INV_CONDITION( pLock->initialized == PSOC_LOCK_SIGNATURE );
   PSO_PRE_CONDITION( pid != 0 );

   return pLock->pid == pid;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined (PSO_USE_POSIX_SEMAPHORE)

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psocIsItLocked( psocProcessLock * pLock )
{
   int rc, val;

   PSO_PRE_CONDITION( pLock != NULL );
   PSO_INV_CONDITION( pLock->initialized == PSOC_LOCK_SIGNATURE );

   rc = sem_getvalue( &pLock->semaphore.sem, &val );
   return val <= 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Initialize the lock */
bool psocInitProcessLock( psocProcessLock * pLock )
{
   int err = 0;
   int pshared = 1; /* Shared between processes */

   PSO_PRE_CONDITION( pLock != NULL );

   if ( pLock->initialized == PSOC_LOCK_SIGNATURE ) return true;
   
   memset( &pLock->semaphore.sem, 0, sizeof(sem_t) );

   pLock->pid = 0;
   pLock->initialized = PSOC_LOCK_SIGNATURE;

   do {
      err = sem_init( &pLock->semaphore.sem, pshared, 1 );
   } while ( err == -1 && errno == EINTR );

   if ( err != 0 ) {
      pLock->initialized = 0;
      fprintf( stderr, "Lock:Init failed with errno = %d\n", errno );
      return false;
   }

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Remove the lock. */
bool psocFiniProcessLock( psocProcessLock * pLock )
{
   int err = 0;

   PSO_PRE_CONDITION( pLock != NULL );
   PSO_INV_CONDITION( pLock->initialized == PSOC_LOCK_SIGNATURE );

   do {
      err = sem_destroy( &pLock->semaphore.sem );
   } while ( err == -1 && errno == EINTR );

   if ( err == 0 ) {
      pLock->pid = 0;
      pLock->initialized = 0;
      return true;
   }

   fprintf( "sem_destroy error, errno = %d\n", errno );
   
   return false;
}
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#else /* PSO_USE_POSIX_SEMAPHORE */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psocIsItLocked( psocProcessLock * pLock )
{
   PSO_PRE_CONDITION( pLock != NULL );
   PSO_INV_CONDITION( pLock->initialized == PSOC_LOCK_SIGNATURE );

   return pLock->lock != 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Initialize the lock */
bool psocInitProcessLock( psocProcessLock * pLock )
{
   PSO_PRE_CONDITION( pLock != NULL );

   if ( pLock->initialized != PSOC_LOCK_SIGNATURE ) {
      pLock->lock = 0;
      pLock->pid  = 0;
      pLock->initialized = PSOC_LOCK_SIGNATURE;
   }
   
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Remove the lock - used for POSIX semaphores. */
bool psocFiniProcessLock( psocProcessLock * pLock )
{
   PSO_PRE_CONDITION( pLock != NULL );
   PSO_INV_CONDITION( pLock->initialized == PSOC_LOCK_SIGNATURE );

   pLock->pid  = 0;
   pLock->lock = 0;
   pLock->initialized = 0;
   
   return true;
}
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSO_USE_POSIX_SEMAPHORE */

