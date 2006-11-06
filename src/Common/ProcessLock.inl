/* -*- c -*- */
/* :mode=c:  - For jedit, previous line for emacs */
/*
 * Copyright (C) 2006 Daniel Prevost <dprevost@users.sourceforge.net>
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
/*
 * linux in kernel 2.5.? started to implement futex (fast user mutexes).
 * Our locking could be reimplemented with them eventually.
 */

#if defined (VDS_USE_TRY_ACQUIRE)
#  if defined (WIN32)

#    define TRY_ACQUIRE( LOCK, VALUE, ISITLOCKED ) \
   ISITLOCKED = InterlockedExchange( &LOCK, &VALUE );

#  elif defined (  __GNUC__ )
   /*
    * With gcc, inline assembler provides the locking
    */
#    if defined (__i386) || defined (__i386__) 
#      define TRY_ACQUIRE( LOCK, VALUE, ISITLOCKED ) \
   unsigned int out;                                 \
   __asm__ __volatile__ (                            \
         "xchg %2, (%1);                             \
          movl %2, %0     "                          \
         :"=r"(out)                                  \
         :"r"(&LOCK), "r"(VALUE) : "memory" );                  \
   if ( out  == 0 )                                  \
      ISITLOCKED = 0;

#      if defined (CONFIG_X86_OOSTORE)
#        define UNLOCK( LOCK )                       \
   __asm__ __volatile__ (                            \
      "lock; movl $0, %0 "                           \
      :"=m"(LOCK)                                    \
      :  /* No input regs */                         \
      :"memory" );
#      else
#        define UNLOCK( LOCK )                       \
   __asm__ __volatile__ (                            \
      "movl $0, %0 "                                 \
      :"=m"(LOCK)                                    \
      :  /* No input regs */                         \
      :"memory" );
#      endif

#    elif defined(__sparc) 
#    define TRY_ACQUIRE( LOCK, VALUE, ISITLOCKED )   \
        unsigned int out;                            \
        __asm__ __volatile__("ldstub [%1], %0"       \
                             : "=r" (out)            \
                             : "r" (&LOCK)           \
                             : "memory" );           \
   if ( out  == 0 ) \
      ISITLOCKED = 0;

#    endif /* on the cpu types */
#  endif /* __GNUC__ */
#endif /* VDS_USE_TRY_ACQUIRE */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * We write to the highest byte of the lock word to invalidate the caches on
 * SMP systems. See the following document from HP for more details:
 * http://h21007.www2.hp.com/dspp/files/unprotected/itanium/spinlocks.pdf
 */
#if defined (VDS_USE_HP_LOCK)
#  define HP_LOCK( LOCK, ISITLOCKED )                \
   *((volatile char *)LOCK) = 0;                     \
   _asm("LDCWS", 0, 0, LOCK, ISITLOCKED );

/*
 * The next define is used to find where in the lockArray should we store
 * the pid of the process which owns the lock (remember that the lock word 
 * must be 16-byte aligned - therefore pLock can point to 4 different
 * places in the lockArray). This pid is always in the word following the
 * lock word (unless we wrap around and it is at position 0).
 */
#  define HP_LOCK_PID_LOCATION ( ((int)((char*)pLock->pLock-pLock->lockArray)+4) % 16 )

/*
 * The next define is used to find where in the lockArray should we store
 * the pid of the process which is attempting to acquire the lock.
 * Not sure yet if this will be useful in attempting to recover from a
 * a crashed process (if a crash occurs between obtaining a lock and 
 * setting lockArray[HP_LOCK_PID_LOCATION] - our "race" condition) 
 * since the data here is not protected by a lock (if a second process
 * was also trying to get the lock it might have overwritten this value
 * just before the lock was obtained by the first process).
 * IOW, if the value of lockArrayHP_TRY_PID_LOCATION] is the value of our
 * chashed process... ok - but if not, the lock might still be held by that 
 * process.
 */
#  define HP_TRY_PID_LOCATION ( ((int)((char*)pLock->pLock-pLock->lockArray)+8) % 16 )

#endif 

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline int  
vdscTestLockPidValue( vdscProcessLock* pLock, pid_t pid )
{
   VDS_PRE_CONDITION( pLock != NULL );
   VDS_INV_CONDITION( pLock->initialized == VDSC_LOCK_SIGNATURE );
   VDS_PRE_CONDITION( pid != 0 );

#if defined (VDS_USE_HP_LOCK)
   return (pid_t) pLock->lockArray[HP_LOCK_PID_LOCATION] == pid;
#else
   return pLock->lock == (unsigned) pid;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline int   
vdscIsItLocked( vdscProcessLock* pLock )
{
   VDS_PRE_CONDITION( pLock != NULL );
   VDS_INV_CONDITION( pLock->initialized == VDSC_LOCK_SIGNATURE );

#if defined (VDS_USE_HP_LOCK)
   return *(pLock->pLock) == 1;
#else
   return pLock->lock != 0;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline void
vdscAcquireProcessLock( vdscProcessLock* pLock,
                        vds_lock_T lockValue )
{
   VDS_PRE_CONDITION( pLock != NULL );
   VDS_INV_CONDITION( pLock->initialized == VDSC_LOCK_SIGNATURE );
   VDS_PRE_CONDITION( lockValue != 0 );

   vds_lock_T tempValue = lockValue;
   
/*   fprintf( stderr, " lockValue1: %d %d\n", pLock->lock, saved ); */
   int isItLocked = -1;

#if defined (VDS_USE_HP_LOCK)
   register pid_t* pTryPID = (pid_t*) &pLock->lockArray[HP_TRY_PID_LOCATION];

   while ( 1 )
   {
      if ( *pLock->pLock == 1 )
      {
         *pTryPID = lockValue;
         HP_LOCK( pLock->pLock, isItLocked );
      }
      if ( isItLocked == 0 )
         break;
      nanosleep( &g_timeOut, NULL );
   }
#elif defined (VDS_USE_TRY_ACQUIRE)
   while ( 1 )
   {
      if ( pLock->lock == 0 )
      {
         TRY_ACQUIRE( pLock->lock, tempValue, isItLocked );
      }
      if ( isItLocked == 0 )
         break;
      nanosleep( &g_timeOut, NULL );
      tempValue = lockValue;
   }
#elif defined (VDS_USE_POSIX_SEMAPHORE)
   do
   {
      /* We restart on interrupts. */
      isItLocked = sem_wait( &pLock->semaphore.sem );
   } while ( isItLocked == -1 && errno == EINTR );
#endif

/*   
fprintf( stderr, " lockValue2: %d %d %d\n", pLock->lock, saved, isItLocked );
*/

   /* Failure to get the lock should not occured!!! */
   VDS_POST_CONDITION( isItLocked == 0 );

   /* There is a "race condition" when saving the pid of the caller
    * this way -> it is possible to have lock out-of-synch with
    * the actual condition of the object.
    */
#if defined (VDS_USE_HP_LOCK)
   *( (pid_t*) &pLock->lockArray[HP_LOCK_PID_LOCATION] ) = lockValue;
   
#elif defined (VDS_USE_POSIX_SEMAPHORE)
   int val;
   int ok = sem_getvalue( &pLock->semaphore.sem, &val ); 
   if ( ok == 0 && val > 0 )
      fprintf( stderr, "acquire 3 %d\n", errno );
   pLock->lock = lockValue;
   
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline int
vdscTryAcquireProcessLock( vdscProcessLock* pLock,
                           vds_lock_T   lockValue,
                           unsigned int milliSecs )
{
   VDS_PRE_CONDITION( pLock != NULL );
   VDS_INV_CONDITION( pLock->initialized == VDSC_LOCK_SIGNATURE );
   VDS_PRE_CONDITION( lockValue != 0 );

   vds_lock_T tempValue = lockValue;

   int isItLocked = -1;

#if defined (VDS_USE_HP_LOCK)
   register pid_t* pTryPID = (pid_t*) &pLock->lockArray[HP_TRY_PID_LOCATION];
   if ( *pLock->pLock == 1 )
   {
      *pTryPID = lockValue;
      HP_LOCK( pLock->pLock, isItLocked );
   }

#elif defined (VDS_USE_TRY_ACQUIRE)
   if ( pLock->lock == 0 )
   {
      TRY_ACQUIRE( pLock->lock, tempValue, isItLocked );
   }
#elif defined (VDS_USE_POSIX_SEMAPHORE)
   errno = 0;
   do
   {
      /* We restart on interrupts. */
      isItLocked = sem_trywait( &pLock->semaphore.sem );
      if ( isItLocked == -1 && errno == EINTR)
         fprintf( stderr, "acquire 2\n" ); 
   } while ( isItLocked == -1 && errno == EINTR );
   if (  isItLocked == -1 && errno != EAGAIN )
   {
      fprintf( stderr, "Lock:trywait failed with errno = %d\n", errno );
      return -1;
   }
#endif

   if ( isItLocked != 0 )
   {
      int iterations = milliSecs/g_timeOutinMilliSecs;
      int i;
      
      for ( i = 0; i < iterations; ++i )
      {
/*
  if ( isItLocked != 0 )
  fprintf( stderr, "acquire 1\n" ); 
*/
         nanosleep( &g_timeOut, NULL );
#if defined (VDS_USE_HP_LOCK)
         if ( *pLock->pLock == 1 )
         {
            *pTryPID = lockValue;
            HP_LOCK( pLock->pLock, isItLocked );
         }
#elif defined (VDS_USE_TRY_ACQUIRE)
         if ( pLock->lock == 0 )
         {
            tempValue = lockValue;
            TRY_ACQUIRE( pLock->lock, tempValue, isItLocked );
         }
#elif defined (VDS_USE_POSIX_SEMAPHORE)
         do
         {
            /* We restart on interrupts. */
            isItLocked = sem_trywait( &pLock->semaphore.sem );
         } while ( isItLocked == -1 && errno == EINTR );
         if (  isItLocked == -1 && errno != EAGAIN )
         {
            fprintf( stderr, "Lock:trywait failed with errno = %d\n", errno );
            return -1;
         }
         if ( i > 25 )
         {
            int val;
            int ok = sem_getvalue( &pLock->semaphore.sem, &val ); 
            fprintf( stderr, "sem_getvalue = %d %d %d\n", val, ok, errno );
         }
#endif
         if ( isItLocked == 0 )
            break;
      }
      if ( isItLocked != 0 )
      {
         return -1;   
      }
      
   }

   /* There is a "race condition" when saving the pid of the caller
    * this way -> it is possible to have lock out-of-synch with
    * the actual condition of the object.
    */

#if defined (VDS_USE_HP_LOCK)
   if ( isItLocked == 0 )
   {
      *( (pid_t*) &pLock->lockArray[HP_LOCK_PID_LOCATION] ) = lockValue;
   }
   
#elif defined (VDS_USE_POSIX_SEMAPHORE)
   if ( isItLocked == 0 )
   {
      int val;
      int ok = sem_getvalue( &pLock->semaphore.sem, &val ); 
      if ( ok == 0 && val > 0 )
         fprintf( stderr, "acquire 3 %d\n", errno );
      pLock->lock = lockValue;
   }
   
#endif

   return isItLocked;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline void
vdscReleaseProcessLock( vdscProcessLock* pLock )
{
   VDS_PRE_CONDITION( pLock != NULL );
   VDS_INV_CONDITION( pLock->initialized == VDSC_LOCK_SIGNATURE );

#if defined (VDS_USE_HP_LOCK)
   *( (pid_t*) &pLock->lockArray[HP_LOCK_PID_LOCATION] ) = -1;

   _flush_globals();
   pLock->pLock = 1;
   
   *( (pid_t*) &pLock->lockArray[HP_LOCK_PID_LOCATION] ) = -1;

#elif defined (VDS_USE_TRY_ACQUIRE)
   UNLOCK( pLock->lock );
#elif defined (VDS_USE_POSIX_SEMAPHORE)

   pLock->lock = 0;

   errno = 0;
   int val;
   int ok = sem_getvalue( &pLock->semaphore.sem, &val ); 
   if ( ok == 0 && val > 0 )
         fprintf( stderr, "release 2 %d\n", errno ); 

   int isItLocked = -1;
   do
   {
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
