/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of vdsf (Virtual Data Space Framework).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation and appearing in the file COPYING included in the
 * packaging of this library.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Common/Common.h"
#include "Common/Timer.h"
#include "Tests/PrintError.h"

const bool expectedToPass = true;

#define NS_PER_SEC     1000000000
#define NS_PER_US            1000
#define HALF_SEC_IN_US     500000
#define TEST_KLOOP            100 /* in thousands */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
#if !defined(WIN32)
   unsigned long sec = 0, nanoSec = 0;
   struct timespec sleepTime = { 0, 10000 };
   int errCode;
   vdscTimer timer;

   vdscInitTimer( &timer );   
   vdscBeginTimer( &timer );

   errCode = nanosleep( &sleepTime, NULL );
   if ( errCode != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   vdscEndTimer( &timer );
   vdscCalculateTimer( &timer, &sec, &nanoSec );

   fprintf( stderr, "Sec = %u, uSec = %u\n", (unsigned int)sec, 
                                             (unsigned int)(nanoSec/1000) );
   if ( sec == 0 && nanoSec < 10000 ) {
      fprintf( stderr, "Timer returns invalid time!\n" );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
#endif
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

