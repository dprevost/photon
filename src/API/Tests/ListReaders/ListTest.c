/*
 * Copyright (C) 2008-2009 Daniel Prevost <dprevost@photonsoftware.org>
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
#include "API/ListReaders.h"
#include "Tests/PrintError.h"

const bool expectedToPass = true;

#define MAX_READERS 50000

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int i;
   unsigned int sum1 = 0, sum2 = 0;

   psoaReader * readers, * dummy, * first;
   int * readerIsIn;
   psoaListReaders list;

   srand( 0x123456 );
   
   psoaListReadersInit( &list );
   
   readers = (psoaReader *)malloc( MAX_READERS*sizeof(psoaReader) );
   if ( readers == NULL ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   readerIsIn = (int *)malloc( MAX_READERS*sizeof(int) );
   if ( readerIsIn == NULL ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   for ( i = 0; i < MAX_READERS; ++i ) {
      readers[i].address = (void *) &readers[i];
      readerIsIn[i] = 0;
      if ( rand() > RAND_MAX/2 ) {
         psoaListReadersPut( &list, &readers[i] );
         readerIsIn[i] = 1;
         sum1++;
      }
   }
   
   if ( sum1 != list.currentSize ) {
      fprintf( stderr, "%d "PSO_SIZE_T_FORMAT"\n", sum1, list.currentSize );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Test our loop */
   if ( psoaListReadersPeakFirst( &list, &dummy ) != true ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   first = dummy;
   do {
      if ( dummy->address != (void*) dummy ) {
         ERROR_EXIT( expectedToPass, NULL, ; );
      }
      sum2++;
   } while ( psoaListReadersPeakNext( &list, dummy, &dummy ) );
   
   if ( sum2 != list.currentSize ) {
      fprintf( stderr, "%d "PSO_SIZE_T_FORMAT"\n", sum2, list.currentSize );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( sum2 != sum1 ) {
      fprintf( stderr, "%d %d\n", sum2, sum1 );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* inverse our selection ! (to test removals) */
   for ( i = 0; i < MAX_READERS; ++i ) {
      if ( readerIsIn[i] == 0 ) {
         psoaListReadersPut( &list, &readers[i] );
         readerIsIn[i] = 1;
      }
      else {
         psoaListReadersRemove( &list, &readers[i] );
         readerIsIn[i] = 0;
      }
   }

   if ( (MAX_READERS-sum1) != list.currentSize ) {
      fprintf( stderr, "%d "PSO_SIZE_T_FORMAT"\n", sum1, list.currentSize );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Retest our loop */
   sum2 = 0;
   if ( psoaListReadersPeakFirst( &list, &dummy ) != true ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( dummy == first ) {
      /* 
       * We cannot have the same first on the two loops since the 
       * selection was inversed.
       */
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   do {
      if ( dummy->address != (void*) dummy ) {
         ERROR_EXIT( expectedToPass, NULL, ; );
      }
      sum2++;
   } while ( psoaListReadersPeakNext( &list, dummy, &dummy ) );
   
   if ( sum2 != list.currentSize ) {
      fprintf( stderr, "%d "PSO_SIZE_T_FORMAT"\n", sum2, list.currentSize );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
