/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@users.sourceforge.net>
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
#include "API/ListReaders.h"
#include "Tests/PrintError.h"

const bool expectedToPass = true;

#define MAX_READERS 50000

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   int i, sum1 = 0, sum2 = 0;

   vdsaReader * readers, * dummy, * first;
   int * readerIsIn;
   vdsaListReaders list;

   srand( 0x123456 );
   
   vdsaListReadersInit( &list );
   
   readers = (vdsaReader *)malloc( MAX_READERS*sizeof(vdsaReader) );
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
         vdsaListReadersPut( &list, &readers[i] );
         readerIsIn[i] = 1;
         sum1++;
      }
   }
   
   if ( sum1 != list.currentSize ) {
      fprintf( stderr, "%d %d\n", sum1, list.currentSize );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Test our loop */
   if ( vdsaListReadersPeakFirst( &list, &dummy ) != true ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   first = dummy;
   do {
      if ( dummy->address != (void*) dummy ) {
         ERROR_EXIT( expectedToPass, NULL, ; );
      }
      sum2++;
   } while ( vdsaListReadersPeakNext( &list, dummy, &dummy ) );
   
   if ( sum2 != list.currentSize ) {
      fprintf( stderr, "%d %d\n", sum2, list.currentSize );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( sum2 != sum1 ) {
      fprintf( stderr, "%d %d\n", sum2, sum1 );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* inverse our selection ! (to test removals) */
   for ( i = 0; i < MAX_READERS; ++i ) {
      if ( readerIsIn[i] == 0 ) {
         vdsaListReadersPut( &list, &readers[i] );
         readerIsIn[i] = 1;
      }
      else {
         vdsaListReadersRemove( &list, &readers[i] );
         readerIsIn[i] = 0;
      }
   }

   if ( (MAX_READERS-sum1) != list.currentSize ) {
      fprintf( stderr, "%d %d\n", sum1, list.currentSize );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Retest our loop */
   sum2 = 0;
   if ( vdsaListReadersPeakFirst( &list, &dummy ) != true ) {
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
   } while ( vdsaListReadersPeakNext( &list, dummy, &dummy ) );
   
   if ( sum2 != list.currentSize ) {
      fprintf( stderr, "%d %d\n", sum2, list.currentSize );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */