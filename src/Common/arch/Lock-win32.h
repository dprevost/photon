/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of photon (photonsoftware.org).
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

#ifndef PSC_ARCH_PROCESS_LOCK_H
#define PSC_ARCH_PROCESS_LOCK_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline void
pscAcquireProcessLock( pscProcessLock * pLock,
                       pid_t            pid_locker )
{
   int isItLocked = -1; /* Value of 0 indicates success */

   PSO_PRE_CONDITION( pLock != NULL );
   PSO_INV_CONDITION( pLock->initialized == PSC_LOCK_SIGNATURE );
   PSO_PRE_CONDITION( pid_locker != 0 );

   for (;;) {
      if ( pLock->lock == 0 ) {
         isItLocked = InterlockedExchange( (LPLONG)&pLock->lock, 0xff );
      }
      if ( isItLocked == 0 ) break;

      Sleep( g_timeOutinMilliSecs );
   }

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
pscTryAcquireProcessLock( pscProcessLock * pLock,
                          pid_t            pid_locker,
                          unsigned int     milliSecs )
{
   int isItLocked = -1; /* Value of 0 indicates success */

   PSO_PRE_CONDITION( pLock != NULL );
   PSO_INV_CONDITION( pLock->initialized == PSC_LOCK_SIGNATURE );
   PSO_PRE_CONDITION( pid_locker != 0 );

   if ( pLock->lock == 0 ) {
      isItLocked = InterlockedExchange( (LPLONG)&pLock->lock, 0xff );
   }

   if ( isItLocked != 0 ) {
      int iterations = milliSecs/g_timeOutinMilliSecs;
      int i;
      
      for ( i = 0; i < iterations; ++i ) {
         Sleep( g_timeOutinMilliSecs );
         if ( pLock->lock == 0 ) {
            isItLocked = InterlockedExchange( (LPLONG)&pLock->lock, 0xff );
         }
         if ( isItLocked == 0 ) break;
      }
   }

   /* 
    * If the condition is true, the time slice was not enough, 
    * we did not get the lock.
    */
   if ( isItLocked != 0 ) return false;

   /* There is a "race condition" when saving the pid of the caller
    * this way -> it is possible to have lock out-of-synch with
    * the actual condition of the object.
    */
   pLock->pid = pid_locker;

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline void
pscReleaseProcessLock( pscProcessLock * pLock )
{
   PSO_PRE_CONDITION( pLock != NULL );
   PSO_INV_CONDITION( pLock->initialized == PSC_LOCK_SIGNATURE );

   pLock->pid = 0;
   /* 
    * We might need to change this code to:
    *
    *    InterlockedExchange( (LPLONG)&pLock->lock, 0 );
    *
    * on some cpus (erratum of some Pentium PRO (1995-1997)) and the 
    * Winchips (1997 to 1999). 
    */
   pLock->lock = 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSC_ARCH_PROCESS_LOCK_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

