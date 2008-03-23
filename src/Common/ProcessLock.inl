/* -*- c -*- */
/* :mode=c:  - For jedit, previous line for emacs */
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

inline int  
vdscTestLockPidValue( vdscProcessLock* pLock, pid_t pid )
{
   VDS_PRE_CONDITION( pLock != NULL );
   VDS_INV_CONDITION( pLock->initialized == VDSC_LOCK_SIGNATURE );
   VDS_PRE_CONDITION( pid != 0 );

   return pLock->pid == pid;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline int   
vdscIsItLocked( vdscProcessLock* pLock )
{
#if defined (VDS_USE_POSIX_SEMAPHORE)
   int rc, val;
#endif

   VDS_PRE_CONDITION( pLock != NULL );
   VDS_INV_CONDITION( pLock->initialized == VDSC_LOCK_SIGNATURE );

#if defined(CONFIG_KERNEL_HEADERS)
   return spin_is_locked( &pLock->lock );
#elif defined(WIN32)
   return pLock->lock != 0;
#elif defined (VDS_USE_POSIX_SEMAPHORE)
   rc = sem_getvalue( &pLock->semaphore.sem, &val );
   return val <= 0;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline void
vdscAcquireProcessLock( vdscProcessLock* pLock,
                        pid_t            pid_locker )
{
   int isItLocked = -1; /* Value of 0 indicates success */

   VDS_PRE_CONDITION( pLock != NULL );
   VDS_INV_CONDITION( pLock->initialized == VDSC_LOCK_SIGNATURE );
   VDS_PRE_CONDITION( pid_locker != 0 );

#if defined(CONFIG_KERNEL_HEADERS)
   spin_lock(&pLock->lock);
   isItLocked = 0;
#elif defined(WIN32)
   for (;;) {
      if ( pLock->lock == 0 ) {
         isItLocked = InterlockedExchange( (LPLONG)&pLock->lock, 0xff );
      }
      if ( isItLocked == 0 )
         break;
      Sleep( g_timeOutinMilliSecs );
   }
#elif defined (VDS_USE_POSIX_SEMAPHORE)
   do {
      /* We restart on interrupts. */
      isItLocked = sem_wait( &pLock->semaphore.sem );
   } while ( isItLocked == -1 && errno == EINTR );
#endif

   /* Failure to get the lock should not occured!!! */
   VDS_POST_CONDITION( isItLocked == 0 );

   /* There is a "race condition" when saving the pid of the caller
    * this way -> it is possible to have lock out-of-synch with
    * the actual condition of the object.
    */
   pLock->pid = pid_locker;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline int
vdscTryAcquireProcessLock( vdscProcessLock* pLock,
                           pid_t            pid_locker,
                           unsigned int     milliSecs )
{
   int isItLocked = -1; /* Value of 0 indicates success */

   VDS_PRE_CONDITION( pLock != NULL );
   VDS_INV_CONDITION( pLock->initialized == VDSC_LOCK_SIGNATURE );
   VDS_PRE_CONDITION( pid_locker != 0 );

#if defined(CONFIG_KERNEL_HEADERS)
   if ( spin_can_lock( &pLock->lock ) ) {
      isItLocked = spin_trylock( &pLock->lock );
      isItLocked--;
   }
#elif defined (WIN32)
   if ( pLock->lock == 0 ) {
      isItLocked = InterlockedExchange( (LPLONG)&pLock->lock, 0xff );
   }
#elif defined (VDS_USE_POSIX_SEMAPHORE)
   errno = 0;
   do {
      /* We restart on interrupts. */
      isItLocked = sem_trywait( &pLock->semaphore.sem );
      if ( isItLocked == -1 && errno == EINTR)
         fprintf( stderr, "acquire 2\n" ); 
   } while ( isItLocked == -1 && errno == EINTR );
   if (  isItLocked == -1 && errno != EAGAIN ) {
      fprintf( stderr, "Lock:trywait failed with errno = %d\n", errno );
      return -1;
   }
#endif

   if ( isItLocked != 0 ) {
      int iterations = milliSecs/g_timeOutinMilliSecs;
      int i;
      
      for ( i = 0; i < iterations; ++i ) {
#if defined (WIN32)
         Sleep( g_timeOutinMilliSecs );
#else
         nanosleep( &g_timeOut, NULL );
#endif
#if defined(CONFIG_KERNEL_HEADERS)
         if ( spin_can_lock( &pLock->lock ) ) {
            isItLocked = spin_trylock( &pLock->lock );
            isItLocked--;
         }
#elif defined (WIN32)
         if ( pLock->lock == 0 ) {
            isItLocked = InterlockedExchange( (LPLONG)&pLock->lock, 0xff );
         }
#elif defined (VDS_USE_POSIX_SEMAPHORE)
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
#endif
         if ( isItLocked == 0 )
            break;
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
vdscReleaseProcessLock( vdscProcessLock* pLock )
{
   VDS_PRE_CONDITION( pLock != NULL );
   VDS_INV_CONDITION( pLock->initialized == VDSC_LOCK_SIGNATURE );

   pLock->pid = 0;

#if defined(CONFIG_KERNEL_HEADERS)
   spin_unlock( &pLock->lock );
#elif defined (WIN32)
   InterlockedExchange( (LPLONG)&pLock->lock, 0 );
#elif defined (VDS_USE_POSIX_SEMAPHORE)

   errno = 0;
   int val;
   int ok = sem_getvalue( &pLock->semaphore.sem, &val ); 
   if ( ok == 0 && val > 0 )
         fprintf( stderr, "release 2 %d\n", errno ); 

   int isItLocked = -1;
   do {
      /* We restart on interrupts. */
      isItLocked = sem_post( &pLock->semaphore.sem );
      if ( isItLocked == -1 )
         fprintf( stderr, "release 1 %d\n", errno ); 
   } while ( isItLocked == -1 && errno == EINTR );

   if (  isItLocked == -1 )
      fprintf( stderr, "Lock:post failed with errno = %d\n", errno );
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

