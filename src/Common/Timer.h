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

#ifndef PSOC_TIMER_H
#define PSOC_TIMER_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Common.h"

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This class provides wrappers for measuring timing characteristics 
 * without having to know the specifics of each operating system.
 *
 * Note on gettimeofday(): old SRV4 systems use a version with a single 
 * argument. On some old BSD, the timezone was returned in the second 
 * argument. Both of these issues could be problematic - this is why 
 * there is a test for this in autoconf.ac (if the test fails, the
 * AC_MSG_ERROR is used - it might be better to find some way to work
 * around the problem instead but on the other hand, this is really 
 * unlikely).
 *
 * Note for future: using clock_gettime() instead of gettimeofday() 
 * might be preferable (and be more accurate?).
 */

struct psocTimer
{
   struct timeval timeBegin;
   struct timeval timeEnd;

#if defined (WIN32)
   LARGE_INTEGER frequency;
   LARGE_INTEGER beginCount;
   LARGE_INTEGER endCount;

   /** For Win32, if the high resolution timer is not supported by the 
    *  hardware, than this is set to false. 
    *  The MSDN docs mention this but is it really possible? 
    */
   BOOL highResolution;

#endif

};

typedef struct psocTimer psocTimer;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined(WIN32)
static inline int gettimeofday( struct timeval * tv, void * tz )
{
   union timeUnion
   {
      FILETIME timeStruct;
      __int64  timeInt;
   } theTime;

   tz = tz;
   GetSystemTimeAsFileTime( &theTime.timeStruct );

   tv->tv_usec = (long) ((theTime.timeInt / 10) % 1000000);
   tv->tv_sec  = (long) ( theTime.timeInt / 10000000);

   return 0;
} 
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** Start the timer */
static inline void 
psocBeginTimer( psocTimer * pTimer )
{
#if defined (WIN32)
   if ( pTimer->highResolution == TRUE ) {
      QueryPerformanceCounter( &pTimer->beginCount );
      return;
   }
   else {
#endif
      gettimeofday( &pTimer->timeBegin, NULL );
#if defined (WIN32)
   }
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** Stop the timer */
static inline
void psocEndTimer( psocTimer * pTimer )
{
#if defined (WIN32)
   if ( pTimer->highResolution == TRUE ) {
      QueryPerformanceCounter( &pTimer->endCount );
      return;
   }
   else {
#endif
      gettimeofday( &pTimer->timeEnd, NULL );
#if defined (WIN32)
   }
#endif
}
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** Calculates - returns the time to the caller in seconds and nanosecs. */
PHOTON_COMMON_EXPORT
void psocCalculateTimer( psocTimer     * pTimer,
                         unsigned long * pSecs,
                         unsigned long * pnanoSecs );

PHOTON_COMMON_EXPORT
void psocInitTimer( psocTimer * pTimer );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

#endif /* PSOC_TIMER_H */

