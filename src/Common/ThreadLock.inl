/* -*- c -*- */
/* :mode=c:  - For jedit, previous line for emacs */
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

inline void
psocAcquireThreadLock( psocThreadLock * pLock )
{
#if ! defined (WIN32)
   int status;
#endif

   PSO_PRE_CONDITION( pLock != NULL );
   PSO_INV_CONDITION( pLock->initialized == PSOC_THREADLOCK_SIGNATURE );

#if defined (WIN32)
   /*
    * The msdn docs say that this call might throw an exception but that the
    * exception should not be handle (from what I understand, the exception
    * would be thrown if the code is badly written... so we're safe ;-).
    */
   EnterCriticalSection( &pLock->mutex );
#else
   status = pthread_mutex_lock( &pLock->mutex );

   /*
    * The post-condition here is simply a reflection that for this call
    * to fail, something else WE did is really wrong (at least, based on my
    * reading of the possible error codes/errno - the only way it can fails
    * is if the mutex was not properly initialized or the current thread
    * already own the lock.
    *
    * Note: this would not apply to other types of mutexes (recursive for
    * example) or the trylock() version...
    */
   PSO_POST_CONDITION( status == 0 );

#endif

}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline bool
psocTryAcquireThreadLock( psocThreadLock * pLock,
                          unsigned int     milliSecs )
{
   int status;
   
   PSO_PRE_CONDITION( pLock != NULL );
   PSO_INV_CONDITION( pLock->initialized == PSOC_THREADLOCK_SIGNATURE );

#if defined (WIN32)
   status = TryEnterCriticalSection( &pLock->mutex );

   if ( status == TRUE ) {
      return true;
   }
   else {
      int iterations = milliSecs/g_timeOutinMilliSecs;
      int i;
      
      for ( i = 0; i < iterations; ++i ) {
         Sleep( g_timeOutinMilliSecs );

         status = TryEnterCriticalSection( &pLock->mutex );
         if ( status == TRUE ) return true;
      }
   }

   return false;

#else
   status = pthread_mutex_trylock( &pLock->mutex );

   if ( status == EBUSY ) { /* or EAGAIN ? */
      int iterations = milliSecs/g_timeOutinMilliSecs;
      int i;
      
      for ( i = 0; i < iterations; ++i ) {
         psocLockSleep( &g_timeOut );

         status = pthread_mutex_trylock( &pLock->mutex );

         if ( status != EBUSY ) break;
      }
   }

   if ( status == EBUSY ) return false;
      
   PSO_POST_CONDITION( status == 0 );

   return true;
#endif  
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline void
psocReleaseThreadLock( psocThreadLock * pLock )
{
   int status;
   
   PSO_PRE_CONDITION( pLock != NULL );
   PSO_INV_CONDITION( pLock->initialized == PSOC_THREADLOCK_SIGNATURE );

#if defined (WIN32)
   status = 0; /* To avoid a warning */
   LeaveCriticalSection( &pLock->mutex );
#else
   status = pthread_mutex_unlock( &pLock->mutex );

   PSO_POST_CONDITION( status == 0 );

#endif

}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

