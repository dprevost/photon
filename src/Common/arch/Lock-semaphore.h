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

#ifndef PSOC_ARCH_PROCESS_LOCK_H
#define PSOC_ARCH_PROCESS_LOCK_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline void
psocAcquireProcessLock( psocProcessLock * pLock,
                        pid_t             pid_locker )
{
   int isItLocked = -1; /* Value of 0 indicates success */

   PSO_PRE_CONDITION( pLock != NULL );
   PSO_INV_CONDITION( pLock->initialized == PSOC_LOCK_SIGNATURE );
   PSO_PRE_CONDITION( pid_locker != 0 );

   do {
      /* We restart on interrupts. */
      isItLocked = sem_wait( &pLock->semaphore.sem );
   } while ( isItLocked == -1 && errno == EINTR );

   /* Failure to get the lock should not occured!!! */
   PSO_POST_CONDITION( isItLocked == 0 );

   /* There is a "race condition" when saving the pid of the caller
    * this way -> it is possible to have lock out-of-synch with
    * the actual condition of the object.
    */
   pLock->pid = pid_locker;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline bool
psocTryAcquireProcessLock( psocProcessLock * pLock,
                           pid_t             pid_locker,
                           unsigned int      milliSecs )
{
   int isItLocked = -1; /* Value of 0 indicates success */

   PSO_PRE_CONDITION( pLock != NULL );
   PSO_INV_CONDITION( pLock->initialized == PSOC_LOCK_SIGNATURE );
   PSO_PRE_CONDITION( pid_locker != 0 );

   errno = 0;
   do {
      /* We restart on interrupts. */
      isItLocked = sem_trywait( &pLock->semaphore.sem );
      if ( isItLocked == -1 && errno == EINTR) {
         fprintf( stderr, "acquire 2\n" ); 
      }
   } while ( isItLocked == -1 && errno == EINTR );
   if (  isItLocked == -1 && errno != EAGAIN ) {
      fprintf( stderr, "Lock:trywait failed with errno = %d\n", errno );
      return false;
   }

   if ( isItLocked != 0 ) {
      int iterations = milliSecs/g_timeOutinMilliSecs;
      int i;
      
      for ( i = 0; i < iterations; ++i ) {
         psocLockSleep( &g_timeOut );
         do {
            /* We restart on interrupts. */
            isItLocked = sem_trywait( &pLock->semaphore.sem );
         } while ( isItLocked == -1 && errno == EINTR );

         if (  isItLocked == -1 && errno != EAGAIN ) {
            fprintf( stderr, "Lock:trywait failed with errno = %d\n", errno );
            return false;
         }
         if ( i > 25 ) {
            int val;
            int ok = sem_getvalue( &pLock->semaphore.sem, &val ); 
            fprintf( stderr, "sem_getvalue = %d %d %d\n", val, ok, errno );
         }
         if ( isItLocked == 0 ) break;
      } /* end of for loop */
      
      /* We come here - the time slice was not enough, no luck getting the lock */
      if ( isItLocked != 0 ) {
         return false;   
      }
   }
 
   /* There is a "race condition" when saving the pid of the caller
    * this way -> it is possible to have lock out-of-synch with
    * the actual condition of the object.
    */
   pLock->pid = pid_locker;

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline void
psocReleaseProcessLock( psocProcessLock * pLock )
{
   PSO_PRE_CONDITION( pLock != NULL );
   PSO_INV_CONDITION( pLock->initialized == PSOC_LOCK_SIGNATURE );

   pLock->pid = 0;

   errno = 0;
   int val;
   int ok = sem_getvalue( &pLock->semaphore.sem, &val ); 
   if ( ok == 0 && val > 0 ) {
      fprintf( stderr, "release 2 %d\n", errno ); 
   }
   
   int isItLocked = -1;
   do {
      /* We restart on interrupts. */
      isItLocked = sem_post( &pLock->semaphore.sem );
      if ( isItLocked == -1 ) {
         fprintf( stderr, "release 1 %d\n", errno ); 
      }
   } while ( isItLocked == -1 && errno == EINTR );

   if (  isItLocked == -1 ) {
      fprintf( stderr, "Lock:post failed with errno = %d\n", errno );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSOC_ARCH_PROCESS_LOCK_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

