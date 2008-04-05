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

/* Initialize the lock - used for POSIX semaphore. */
int vdscInitProcessLock( vdscProcessLock* pLock )
{
   int err = 0;

#if defined (VDS_USE_POSIX_SEMAPHORE)
   int pshared = 1; /* Shared between processes */
#endif

   VDS_PRE_CONDITION( pLock != NULL );

   if ( pLock->initialized == VDSC_LOCK_SIGNATURE ) return 0;
   
#if defined(CONFIG_KERNEL_HEADERS)
   spin_lock_init(&pLock->lock);
#elif defined(WIN32)
   pLock->lock = 0;
#elif defined (VDS_USE_POSIX_SEMAPHORE)
   memset( &pLock->semaphore.sem, 0, sizeof(sem_t) );
#endif

   pLock->pid = 0;
   pLock->initialized = VDSC_LOCK_SIGNATURE;

#if defined (VDS_USE_POSIX_SEMAPHORE)
   do {
      err = sem_init( &pLock->semaphore.sem, pshared, 1 );
   } while ( err == -1 && errno == EINTR );

   if ( err != 0 ) {
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
   do {
      err = sem_destroy( &pLock->semaphore.sem );
   } while ( err == -1 && errno == EINTR );
#endif

   pLock->pid = 0;
   
   if ( err == 0 ) pLock->initialized = 0;
   
   return err;
}
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

