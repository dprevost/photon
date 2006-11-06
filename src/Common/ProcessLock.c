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

#include "ProcessLock.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int g_timeOutinMilliSecs = 5;

struct timespec g_timeOut = { 0, 5000000 };

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Initialize the lock - used for POSIX semaphore. */
int vdscInitProcessLock( vdscProcessLock* pLock )
{
   int err = 0;

#if defined (VDS_USE_POSIX_SEMAPHORE)
   int pshared = 1; /* Shared between processes */
#endif

   VDS_PRE_CONDITION( pLock != NULL );

   if ( pLock->initialized == VDSC_LOCK_SIGNATURE )
      return 0;
   
#if defined (VDS_USE_HP_LOCK)
   pLock->pLock = NULL;
#else
   pLock->lock = 0;
#endif

#if defined (VDS_USE_POSIX_SEMAPHORE)
   memset( &pLock->semaphore.sem, 0, sizeof(sem_t) );
#endif

#if defined (VDS_USE_HP_LOCK)
   memset( pLock->lockArray, 0, 16 );
   pLock->pLock = (volatile int *)((int)(pLock->lockArray)+15&~0xf);
#  if 0
   /* 64 bits ? */
   pLock->pLock = (volatile int *)((long)(pLock->lockArray)+15&~0xf);
#  endif
#endif

   pLock->initialized = VDSC_LOCK_SIGNATURE;

#if defined (VDS_USE_POSIX_SEMAPHORE)
   do
   {
      err = sem_init( &pLock->semaphore.sem, pshared, 1 );
   } while ( err == -1 && errno == EINTR );
   if ( err != 0 ) 
   {
      pLock->initialized = 0;
      fprintf( stderr, "Lock:Init failed with errno = %d\n", errno );
   }
#endif

   return err;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Remove the lock - used for POSIX semaphores. */
int vdscFiniProcessLock( vdscProcessLock* pLock )
{
   int err = 0;

   VDS_PRE_CONDITION( pLock != NULL );
   VDS_INV_CONDITION( pLock->initialized == VDSC_LOCK_SIGNATURE );

#if defined (VDS_USE_POSIX_SEMAPHORE)
   do
   {
      err = sem_destroy( &pLock->semaphore.sem );
   } while ( err == -1 && errno == EINTR );
#endif

   if ( err == 0 )
      pLock->initialized = 0;
   
   return err;
}
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
#if 0
void vdscInitGuard( vdscProcessLock* pLock, vds_lock_T lockValue )
{
   VDS_PRE_CONDITION( pLock != NULL );
   VDS_INV_CONDITION( pLock->initialized == VDSC_LOCK_SIGNATURE );

   vdscAcquireLock( pLock, lockValue );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdscFiniGuard( vdscProcessLock* pLock )
{
   VDS_PRE_CONDITION( pLock != NULL );
   VDS_INV_CONDITION( pLock->initialized == VDSC_LOCK_SIGNATURE );

   vdscReleaseLock( pLock );
}
#endif
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
