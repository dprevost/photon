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

#ifndef VDSC_ARCH_PROCESS_LOCK_H
#define VDSC_ARCH_PROCESS_LOCK_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline void
vdscAcquireProcessLock( vdscProcessLock * pLock,
                        pid_t             pid_locker )
{
   int isItLocked = -1; /* Value of 0 indicates success */

   VDS_PRE_CONDITION( pLock != NULL );
   VDS_INV_CONDITION( pLock->initialized == VDSC_LOCK_SIGNATURE );
   VDS_PRE_CONDITION( pid_locker != 0 );

   do {
      /* We restart on interrupts. */
      isItLocked = sem_wait( &pLock->semaphore.sem );
   } while ( isItLocked == -1 && errno == EINTR );

   /* Failure to get the lock should not occured!!! */
   VDS_POST_CONDITION( isItLocked == 0 );

   /* There is a "race condition" when saving the pid of the caller
    * this way -> it is possible to have lock out-of-synch with
    * the actual condition of the object.
    */
   pLock->pid = pid_locker;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline int
vdscTryAcquireProcessLock( vdscProcessLock * pLock,
                           pid_t             pid_locker,
                           unsigned int      milliSecs )
{
   int isItLocked = -1; /* Value of 0 indicates success */

   VDS_PRE_CONDITION( pLock != NULL );
   VDS_INV_CONDITION( pLock->initialized == VDSC_LOCK_SIGNATURE );
   VDS_PRE_CONDITION( pid_locker != 0 );

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
      return -1;
   }

   if ( isItLocked != 0 ) {
      int iterations = milliSecs/g_timeOutinMilliSecs;
      int i;
      
      for ( i = 0; i < iterations; ++i ) {
         nanosleep( &g_timeOut, NULL );
         do {
            /* We restart on interrupts. */
            isItLocked = sem_trywait( &pLock->semaphore.sem );
         } while ( isItLocked == -1 && errno == EINTR );

         if (  isItLocked == -1 && errno != EAGAIN ) {
            fprintf( stderr, "Lock:trywait failed with errno = %d\n", errno );
            return -1;
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
         return -1;   
      }
   }
 
   /* There is a "race condition" when saving the pid of the caller
    * this way -> it is possible to have lock out-of-synch with
    * the actual condition of the object.
    */
   pLock->pid = pid_locker;

   return isItLocked;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline void
vdscReleaseProcessLock( vdscProcessLock * pLock )
{
   VDS_PRE_CONDITION( pLock != NULL );
   VDS_INV_CONDITION( pLock->initialized == VDSC_LOCK_SIGNATURE );

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

#endif /* VDSC_ARCH_PROCESS_LOCK_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

