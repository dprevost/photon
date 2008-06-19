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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "ProcessLock.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

VDSF_COMMON_EXPORT
int g_timeOutinMilliSecs = 5;

struct timespec g_timeOut = { 0, 5000000 };

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdscTestLockPidValue( vdscProcessLock * pLock, pid_t pid )
{
   VDS_PRE_CONDITION( pLock != NULL );
   VDS_INV_CONDITION( pLock->initialized == VDSC_LOCK_SIGNATURE );
   VDS_PRE_CONDITION( pid != 0 );

   return pLock->pid == pid;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined (VDS_USE_POSIX_SEMAPHORE)

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdscIsItLocked( vdscProcessLock * pLock )
{
#if defined (VDS_USE_POSIX_SEMAPHORE)
   int rc, val;
#endif

   VDS_PRE_CONDITION( pLock != NULL );
   VDS_INV_CONDITION( pLock->initialized == VDSC_LOCK_SIGNATURE );

   rc = sem_getvalue( &pLock->semaphore.sem, &val );
   return val <= 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Initialize the lock */
int vdscInitProcessLock( vdscProcessLock * pLock )
{
   int err = 0;
   int pshared = 1; /* Shared between processes */

   VDS_PRE_CONDITION( pLock != NULL );

   if ( pLock->initialized == VDSC_LOCK_SIGNATURE ) return 0;
   
   memset( &pLock->semaphore.sem, 0, sizeof(sem_t) );

   pLock->pid = 0;
   pLock->initialized = VDSC_LOCK_SIGNATURE;

   do {
      err = sem_init( &pLock->semaphore.sem, pshared, 1 );
   } while ( err == -1 && errno == EINTR );

   if ( err != 0 ) {
      pLock->initialized = 0;
      fprintf( stderr, "Lock:Init failed with errno = %d\n", errno );
   }

   return err;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Remove the lock. */
int vdscFiniProcessLock( vdscProcessLock * pLock )
{
   int err = 0;

   VDS_PRE_CONDITION( pLock != NULL );
   VDS_INV_CONDITION( pLock->initialized == VDSC_LOCK_SIGNATURE );

   do {
      err = sem_destroy( &pLock->semaphore.sem );
   } while ( err == -1 && errno == EINTR );

   pLock->pid = 0;
   if ( err == 0 ) pLock->initialized = 0;
   
   return err;
}
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#else /* VDS_USE_POSIX_SEMAPHORE */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdscIsItLocked( vdscProcessLock * pLock )
{
   VDS_PRE_CONDITION( pLock != NULL );
   VDS_INV_CONDITION( pLock->initialized == VDSC_LOCK_SIGNATURE );

   return pLock->lock != 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Initialize the lock */
int vdscInitProcessLock( vdscProcessLock * pLock )
{
   VDS_PRE_CONDITION( pLock != NULL );

   if ( pLock->initialized == VDSC_LOCK_SIGNATURE ) return 0;
   
   pLock->lock = 0;
   pLock->pid  = 0;
   pLock->initialized = VDSC_LOCK_SIGNATURE;

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Remove the lock - used for POSIX semaphores. */
int vdscFiniProcessLock( vdscProcessLock * pLock )
{
   VDS_PRE_CONDITION( pLock != NULL );
   VDS_INV_CONDITION( pLock->initialized == VDSC_LOCK_SIGNATURE );

   pLock->pid  = 0;
   pLock->lock = 0;
   pLock->initialized = 0;
   
   return 0;
}
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDS_USE_POSIX_SEMAPHORE */

