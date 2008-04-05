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

inline void
vdscAcquireThreadLock( vdscThreadLock* pLock )
{
#if ! defined (WIN32)
   int status;
#endif

   VDS_PRE_CONDITION( pLock != NULL );
   VDS_INV_CONDITION( pLock->initialized == VDSC_THREADLOCK_SIGNATURE );

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
   VDS_POST_CONDITION( status == 0 );

#endif

}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline int
vdscTryAcquireThreadLock( vdscThreadLock* pLock,
                          unsigned int milliSecs )
{
   int status;
   
   VDS_PRE_CONDITION( pLock != NULL );
   VDS_INV_CONDITION( pLock->initialized == VDSC_THREADLOCK_SIGNATURE );

#if defined (WIN32)
   status = TryEnterCriticalSection( &pLock->mutex );

   if ( status == TRUE ) {
      return 0;
   }
   else {
      int iterations = milliSecs/g_timeOutinMilliSecs;
      int i;
      
      for ( i = 0; i < iterations; ++i ) {
         Sleep( g_timeOutinMilliSecs );

         status = TryEnterCriticalSection( &pLock->mutex );
         if ( status == TRUE ) return 0;
      }
   }

   return -1;

#else
   status = pthread_mutex_trylock( &pLock->mutex );

   if ( status == EBUSY ) { /* or EAGAIN ? */
      int iterations = milliSecs/g_timeOutinMilliSecs;
      int i;
      
      for ( i = 0; i < iterations; ++i ) {
         nanosleep( &g_timeOut, NULL );

         status = pthread_mutex_trylock( &pLock->mutex );

         if ( status != EBUSY ) break;
      }
   }

   if ( status == EBUSY ) return -1;
      
   VDS_POST_CONDITION( status == 0 );

   return 0;
#endif  
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline void
vdscReleaseThreadLock( vdscThreadLock* pLock )
{
   int status;
   
   VDS_PRE_CONDITION( pLock != NULL );
   VDS_INV_CONDITION( pLock->initialized == VDSC_THREADLOCK_SIGNATURE );

#if defined (WIN32)
   status = 0; /* To avoid a warning */
   LeaveCriticalSection( &pLock->mutex );
#else
   status = pthread_mutex_unlock( &pLock->mutex );

   VDS_POST_CONDITION( status == 0 );

#endif

}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

