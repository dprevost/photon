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

#include <builtins.h>

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline void
vdscAcquireProcessLock( vdscProcessLock * pLock,
                        pid_t             pid_locker )
{
   int out = 0;

   VDS_PRE_CONDITION( pLock != NULL );
   VDS_INV_CONDITION( pLock->initialized == VDSC_LOCK_SIGNATURE );
   VDS_PRE_CONDITION( pid_locker != 0 );

   for (;;) {
      if ( __lwarx( &pLock->lock ) == 0 ) {
         out = __stwcx( &pLock->lock, 1 );
      }
      if ( out == 1 ) {
         __isync();
         pLock->pid = pid_locker;
         return;
      }
      nanosleep( &g_timeOut, NULL );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline bool
vdscTryAcquireProcessLock( vdscProcessLock * pLock,
                           pid_t             pid_locker,
                           unsigned int      milliSecs )
{
   int out = 0;

   VDS_PRE_CONDITION( pLock != NULL );
   VDS_INV_CONDITION( pLock->initialized == VDSC_LOCK_SIGNATURE );
   VDS_PRE_CONDITION( pid_locker != 0 );

   if ( __lwarx( &pLock->lock ) == 0 ) {
      out = __stwcx( &pLock->lock, 1 );
   }
   /* __isync() (full memory barrier) as recommended by IBM manuals */
   if ( out == 1 ) __isync();
   else {
      int iterations = milliSecs/g_timeOutinMilliSecs;
      int i;
      
      for ( i = 0; i < iterations; ++i ) {
         nanosleep( &g_timeOut, NULL );
         if ( __lwarx( &pLock->lock ) == 0 ) {
            out = __stwcx( &pLock->lock, 1 );
         }
         /* __isync() (full memory barrier) as recommended by IBM manuals */
         if ( out == 1 ) {
            __isync();
            break;
         }
      }
   }

   /* 
    * If the condition is true, the time slice was not enough, 
    * we did not get the lock.
    */
   if ( out == 0 ) return false;   

   /* There is a "race condition" when saving the pid of the caller
    * this way -> it is possible to have lock out-of-synch with
    * the actual condition of the object.
    */
   pLock->pid = pid_locker;

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline void
vdscReleaseProcessLock( vdscProcessLock * pLock )
{
   VDS_PRE_CONDITION( pLock != NULL );
   VDS_INV_CONDITION( pLock->initialized == VDSC_LOCK_SIGNATURE );

   pLock->pid  = 0;
   /*
    * __eieio - store(write) memory barrier, to make sure that all 
    * changes (stores) are complete before we reset the lock.
    */
	__eieio();
   pLock->lock = 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSC_ARCH_PROCESS_LOCK_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

