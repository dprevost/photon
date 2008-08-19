/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

/*
 * Some help on assembler for aix is available here:
 *
 * http://publib.boulder.ibm.com/infocenter/systems/index.jsp?topic=/com.ibm.aix.aixassem/doc/alangref/lwarx.htm
 */

#define POWER_PC_LOCK                                              \
__asm__ __volatile__ (                                             \
   "nop      \n"                                                   \
   "1:       \n\t"                                                 \
   "lwarx    %0,0,%2\n\t" /* Load and reserve */                   \
   "cmpwi    0,%0,0\n\t"  /* Is the lock value equal to zero */    \
   "bne-     2f\n\t"      /* No? Exit - the lock is taken */       \
   "stwcx.   %1,0,%2\n\t" /* Store the value in %1 at  %2 */       \
   "bne-     1b\n\t"      /* We lost the reservation but the */    \
                          /* lock was zero when we started so we */\
                          /* try again  without a potential */     \
                          /* timeout */                            \
   "isync\n"              /* Full memory barrier */                \
   "2: "                                                           \
   : "=&r"(oldLockValue)  /* modifier '&': the output is set */    \
                          /* before the input(s) are used - do */  \
                          /* not reuse an input reg. */            \
   : "r" (lockValue), "r"(&pLock->lock)                            \
   : "cr0", "memory" );   /* cr0: condition register 0 */             

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline void
pscAcquireProcessLock( pscProcessLock * pLock,
                       pid_t            pid_locker )
{
   int lockValue = 0xff, oldLockValue;
   volatile pso_lock_T * pLockLock;

   PSO_PRE_CONDITION( pLock != NULL );
   PSO_INV_CONDITION( pLock->initialized == PSC_LOCK_SIGNATURE );
   PSO_PRE_CONDITION( pid_locker != 0 );

   pLockLock = &pLock->lock;

   for (;;) {

      POWER_PC_LOCK

      if ( oldLockValue == 0 ) {
         pLock->pid = pid_locker;
         return;
      }
      nanosleep( &g_timeOut, NULL );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline bool
pscTryAcquireProcessLock( pscProcessLock * pLock,
                          pid_t            pid_locker,
                          unsigned int     milliSecs )
{
   int lockValue = 0xff, oldLockValue;
   pso_lock_T * pLockLock;

   PSO_PRE_CONDITION( pLock != NULL );
   PSO_INV_CONDITION( pLock->initialized == PSC_LOCK_SIGNATURE );
   PSO_PRE_CONDITION( pid_locker != 0 );

   pLockLock = &pLock->lock;
   POWER_PC_LOCK
   
   if ( oldLockValue != 0 ) {
      int iterations = milliSecs/g_timeOutinMilliSecs;
      int i;
      
      for ( i = 0; i < iterations; ++i ) {
         nanosleep( &g_timeOut, NULL );
         POWER_PC_LOCK
         if ( oldLockValue == 0 ) break;
      }
   }
 
   if ( oldLockValue != 0 ) return false;

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
    * eieio - store(write) memory barrier, to make sure that all 
    * changes (stores) are complete before we reset the lock.
    */
	__asm__ __volatile__("eieio": : :"memory");
   pLock->lock = 0;

}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#undef POWER_PC_LOCK

#endif /* PSC_ARCH_PROCESS_LOCK_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

