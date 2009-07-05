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

#include "Common/Common.h"
#include "Common/Timer.h"

#define NS_PER_SEC     1000000000
#define NS_PER_US            1000
#define HALF_SEC_IN_US     500000
#define TEST_LOOP          300000
#define TENTH_SEC_IN_US    100000    /* (1/10 sec in usecs) */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test1( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   unsigned long sec, nanoSec, sum;
   psocTimer timer;
   
   unsigned long loop, innerLoop = TEST_LOOP;
   int dum, i, outerLoop = 15;

   sec = 0;
   nanoSec = 0;
   loop = 0;
   dum = 0;
   psocInitTimer( &timer );
   
   /*
    * The first thing we must do is to calibrate the loop. The problem
    * we have to "solve" is to make sure we cover all code paths in
    * psocCalculateTimer(), specially this if condition:
    *   "if ( timeEnd.tv_usec < timeBegin.tv_usec )"
    *
    * To do that, we ajust the inner loop to be multiples of ~0.10 sec 
    * and run the outer loop multiple times. This gives us a good chance 
    * of hitting the if condition. 
    *
    * Note that this does not always work perfectly in real life. The 
    * inner loops end up not always being multiple of 0.1 secs in the 
    * tests I've ran (but it was done on a laptop with a Centrino - it is 
    * likely that the frequency of the cpu was increased by the heavy 
    * workload of these tests).
    */
   psocBeginTimer( &timer );

   while ( loop < innerLoop ) {
      dum = (dum+1)*loop;
      dum = dum - loop;
      dum = dum + 2 - dum /2;
      loop++;
   }
      
   psocEndTimer( &timer );
   psocCalculateTimer( &timer, &sec, &nanoSec );
   fprintf( stderr, "Calibration: Sec = %lu, nanoSec = %lu\n", sec, nanoSec );

   /*
    * Newer version of gcc seem to strip the loop away since dum is not
    * used anywhere. This seems to fix the problem (declaring dum
    * as volatile is likely a better solution).
    */
   if (dum == 0 ) fprintf(stderr, "%d\n", dum );
   
   assert_false( sec == 0 && nanoSec == 0 );
   sum = NS_PER_SEC*sec + nanoSec;
   sum = sum / NS_PER_US;    /* in micro-seconds */
   if ( sum == 0 ) sum = 1;

   /* 
    * Note: the 2 factors "1000" in the next line are to avoid integer
    * overflow on 32 bits machine. The side effect is to diminush the 
    * precision of the calculations to a millisec.
    */
   innerLoop = ((innerLoop/1000)*TENTH_SEC_IN_US/sum)*1000;

   /* The test itself */

   for ( i = 0; i < outerLoop; ++i ) {
      sec = 0;
      nanoSec = 0;
      loop = 0;
      dum = 0;
      
      psocBeginTimer( &timer );

      while ( loop < innerLoop*(i+1) ) {
         dum = (dum+1)*loop;
         dum = dum - loop;
         dum = dum + 2 - dum /2;
         loop++;
      }
      
      psocEndTimer( &timer );
      psocCalculateTimer( &timer, &sec, &nanoSec );

      /*
       * Newer version of gcc seem to strip the loop away since dum is not
       * used anywhere. This seems to fix the problem.
       */
      if (dum == 0 ) fprintf(stderr, "%d\n", dum );

      assert_false( sec == 0 && nanoSec == 0 );
      
      fprintf( stderr, "Sec = %lu, nanoSec = %09lu\n", sec, nanoSec );
   }
   
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test( test1 ),
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

