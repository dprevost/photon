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
   unsigned int out = 0xff;

   VDS_PRE_CONDITION( pLock != NULL );
   VDS_INV_CONDITION( pLock->initialized == VDSC_LOCK_SIGNATURE );
   VDS_PRE_CONDITION( pid_locker != 0 );

   for (;;) {
      if ( pLock->lock == 0 ) {
         __asm__ __volatile__ (
            "xchgl %0, (%1)"
            : "=r"(out)
            : "r"(&pLock->lock), "0"(out)
            : "memory" );
      }
      if ( out == 0 ) break;
      nanosleep( &g_timeOut, NULL );
   }

   /* Failure to get the lock should not occured!!! */
   VDS_POST_CONDITION( out == 0 );

   /* There is a "race condition" when saving the pid of the caller
    * this way -> it is possible to have lock out-of-synch with
    * the actual condition of the object.
    */
   pLock->pid = pid_locker;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline bool
vdscTryAcquireProcessLock( vdscProcessLock * pLock,
                           pid_t             pid_locker,
                           unsigned int      milliSecs )
{
   unsigned int out = 0xff;

   VDS_PRE_CONDITION( pLock != NULL );
   VDS_INV_CONDITION( pLock->initialized == VDSC_LOCK_SIGNATURE );
   VDS_PRE_CONDITION( pid_locker != 0 );

   if ( pLock->lock == 0 ) {
      __asm__ __volatile__ (
         "xchgl %0, (%1)"
         : "=r"(out)
         : "r"(&pLock->lock), "0"(out)
         : "memory" );
   }

   if ( out > 0 ) {
      int iterations = milliSecs/g_timeOutinMilliSecs;
      int i;
      
      for ( i = 0; i < iterations; ++i ) {
         nanosleep( &g_timeOut, NULL );
         if ( pLock->lock == 0 ) {
            __asm__ __volatile__ (
               "xchgl %0, (%1)"
               : "=r"(out)
               : "r"(&pLock->lock), "0"(out)
               : "memory" );
         }
         if ( out == 0 ) break;
      }
   }
 
   /* 
    * If the condition is true, the time slice was not enough, 
    * we did not get the lock.
    */
   if ( out > 0 ) return false;

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

   pLock->pid = 0;

   /* 
    * We might need to change the template to "lock; movl $0, %0" on 
    * some cpus (erratum of some Pentium PRO (1995-1997)) and the 
    * Winchips (1997 to 1999). 
    */ 
   __asm__ __volatile__ (
      "movl $0, %0"
      :"=m"(pLock->lock)
      :
      :"memory" );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSC_ARCH_PROCESS_LOCK_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

