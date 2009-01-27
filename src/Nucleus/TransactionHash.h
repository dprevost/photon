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

#ifndef PSON_TRANSACTION_HASH_H
#define PSON_TRANSACTION_HASH_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Nucleus/HashCommon.h"
#include "Nucleus/Engine.h"
#include "Nucleus/hash_fnv.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Descriptor/container for elements of this internal hash array. 
 */ 
struct psonTxMyHashItem 
{
   /** Next item in this bucket */
   ptrdiff_t     nextItem;
   
   size_t        bucket;
//   unsigned char key[1];
   psonMemObject * key;
};

typedef struct psonTxMyHashItem psonTxMyHashItem;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static unsigned int g_lowDensity  = 30;
static unsigned int g_highDensity = 50;

bool txHashResize( psonTx             * pTx,
                   psonSessionContext * pContext );

bool txHashFindKey( psonTx         * pTx,
                    psonMemObject ** pArray,
                    psonMemObject ** pkey,
                    size_t         * pRowNumber );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void txHashEmpty( psonTx * pTx )
{
   psonMemObject ** pArray;
   
   GET_PTR( pArray, pTx->listOfLocks.arrayOffset, psonMemObject * );

   memset( pArray, 0, 
      sizeof(void *) * g_psonArrayLengths[pTx->listOfLocks.lengthIndex] );

   pTx->listOfLocks.numberOfItems = 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
bool txHashGetFirst( psonTx         * pTx,
                     size_t         * pNewRowNumber, 
                     psonMemObject ** ppMemObj )
{
   psonMemObject ** pArray;
   size_t i;

   GET_PTR( pArray, pTx->listOfLocks.arrayOffset, psonMemObject * );

   if ( pTx->listOfLocks.numberOfItems == 0 ) return false;

   for ( i = 0; i < g_psonArrayLengths[pTx->listOfLocks.lengthIndex]; ++i ) {
      if ( pArray[i] != NULL ) {
         *ppMemObj = pArray[i];
         *pNewRowNumber = i;

         return true;
      }
   }
   
   return false; /* Should never occur */
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
bool txHashGetNext( psonTx         * pTx,
                    size_t           oldRowNumber,
                    size_t         * pNewRowNumber, 
                    psonMemObject ** ppMemObj )
{
   psonMemObject ** pArray;
   size_t i;

   GET_PTR( pArray, pTx->listOfLocks.arrayOffset, psonMemObject * );

   for ( i = oldRowNumber+1; i < g_psonArrayLengths[pTx->listOfLocks.lengthIndex]; ++i ) {
      if ( pArray[i] != NULL ) {
         *ppMemObj = pArray[i];
         *pNewRowNumber = i;

         return true;
      }
   }
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool txHashInit( psonTx             * pTx,
                 size_t               initialSize, 
                 psonSessionContext * pContext )
{
   size_t length, i;
   psonMemObject ** pArray;

   PSO_PRE_CONDITION( pTx      != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   
   initialSize = (100*initialSize)/g_lowDensity;

   /* Which one of our lengths is closer but larger than initialSize? */
   pTx->listOfLocks.lengthIndex = pTx->listOfLocks.lengthIndexMinimum = 0;
   for ( i = 1; i < PSON_PRIME_NUMBER_ARRAY_LENGTH; ++i ) {
      if ( g_psonArrayLengths[i] > initialSize ) {
         pTx->listOfLocks.lengthIndex = i - 1;
         pTx->listOfLocks.lengthIndexMinimum = i - 1;
         break;
      }
   }

   length = g_psonArrayLengths[pTx->listOfLocks.lengthIndex] * sizeof(void *);
   pArray = (psonMemObject **) psonMalloc( &pTx->memObject, length, pContext );
   if ( pArray == NULL ) {
      psocSetError( &pContext->errorHandler, 
                    g_psoErrorHandle, 
                    PSO_NOT_ENOUGH_PSO_MEMORY );
      return false;
   }

   memset( pArray, 0, length );
      
   pTx->listOfLocks.arrayOffset = SET_OFFSET(pArray);
   pTx->listOfLocks.numberOfItems = 0;

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool txHashInsert( psonTx             * pTx,
                   psonMemObject      * key,
                   psonSessionContext * pContext )
{
   psonMemObject ** pArray;
   bool ok;
   size_t rowNumber = 0;
   bool   keyFound = false;

   /* We resize first in order to get the "right" row number immediately */
   if ( ( ( (pTx->listOfLocks.numberOfItems + 1)*100)/
           g_psonArrayLengths[pTx->listOfLocks.lengthIndex]) >= g_highDensity ) {
      ok = txHashResize( pTx, pContext );
      if ( !ok ) return ok;
   }

   GET_PTR( pArray, pTx->listOfLocks.arrayOffset, psonMemObject * );
   
   keyFound = txHashFindKey( pTx, pArray, &key, &rowNumber );

   if ( keyFound ) return true;

   pTx->listOfLocks.numberOfItems++;

   pArray[rowNumber] = key;

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if 0

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void HashList::Close( psonSessionContext* pContext )
{
   EmptyList( pContext );

   /* EmptyList deallocates the rows but not the main array of */
   /* RowDescriptor* */
   psonTxMyHashItem* pArray;
   SET_PTR( pArray, pTx->listOfLocks.arrayOffset, psonTxMyHashItem );
   
   if ( pArray != NULL )
   {
      pArray->~psonTxMyHashItem();
      psonFree( pContext->pAllocator, pArray, &pContext->errorHandler );
   }
   pTx->listOfLocks.arrayOffset = NULL_OFFSET;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif

/* Just in case a new function is used instead, one of these days */
static inline u_long
hash_it( psonMemObject ** key, size_t len )
{
   return fnv_buf( (void *)key, len, FNV1_INIT );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool txHashFindKey( psonTx         * pTx,
                    psonMemObject ** pArray,
                    psonMemObject ** key, 
                    size_t         * pRowNumber )
{
   size_t rowNumber = hash_it( key, sizeof(void *) ) % 
                             g_psonArrayLengths[pTx->listOfLocks.lengthIndex];

   while (pArray[rowNumber] != NULL ) {

      if ( pArray[rowNumber] == *key ) {
         *pRowNumber = rowNumber;
         return true;
      }
      
      ++rowNumber; /* Advance to next possible emplacement */

      /* Go back to the beginning of the array (since the key might have been */
      /* hashed close to the end of the array, the requested location might */
      /* be at the beginning */
      if ( rowNumber == g_psonArrayLengths[pTx->listOfLocks.lengthIndex] )
         rowNumber = 0;
   }

   *pRowNumber = rowNumber;
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool txHashDelete( psonTx        * pTx,
                   psonMemObject * key )
{
   size_t rowNumber = 0;
   psonMemObject ** pArray;
   bool keyFound = false;
   
   GET_PTR( pArray, pTx->listOfLocks.arrayOffset, psonMemObject * );
   
   keyFound = txHashFindKey( pTx, pArray, &key, &rowNumber );

   if ( keyFound ) {

      size_t curr; /* Current entry we are checking */
      size_t hole; /* Location of the hole */
      size_t hashValue; /* Hash value for current entry */

      pArray[rowNumber] = NULL;
      pTx->listOfLocks.numberOfItems--;

      /* 
       * The algorithm for deletions in a hash array look complex but
       * is in fact pretty basic:
       * Suppose you're deleting entry 13 and you have a chain of 5
       * non-null entries, 13 to 17.
       * If we leave 13 empty and either one of the four others
       * hash to 13, then it won't be found in a search.
       * So, to be able to find this (say entry 14 hash to 14 but entry
       * 15 hash to 13) we have to move it into the empty slot.
       * Moving this guy to the empty slot creates a new empty slot
       * (15) and any subsequent entry in the chain must be checked
       * to make sure they don't hash to that new slot. If they do,
       * you have to move it... repeat the process until the end of
       * the chain of non-empty entries. 
       */
      /*
       * To complicate matter, the chain might wrape around the end of
       * the array (as in, 101, 102, 0, 1 if the array has 103 entries).
       */

      curr = rowNumber;

	  for (;;) { // while ( true ) {
         pArray[curr] = NULL;
         hole = curr;

         do {
            curr++;
            if ( curr == g_psonArrayLengths[pTx->listOfLocks.lengthIndex] )
               curr = 0;

            if( pArray[curr] == NULL ) return true;
            
            hashValue = hash_it( &pArray[curr], sizeof(void *) ) % 
                             g_psonArrayLengths[pTx->listOfLocks.lengthIndex];

         } while( ((hole < hashValue) && (hashValue <= curr)) ||
                  ((curr < hole)  && (hole < hashValue)) ||
                  ((hashValue <= curr)  && (curr < hole)) );

         /* To explain this while condition:
          *
          * Curr < Hole    Hash value is in region:
          *
          *  _________     __
          * |         | 
          * |         |    No move (condition 3)
          * |         |
          * | Current |    __     
          * |         |
          * |         |    Move curr into hole
          * |         |
          * |  Hole   |    __
          * |         |
          * |         |    No move (condition 2)
          * |_________|    __

          * Curr > Hole    Hash value is in region:
          *
          *  _________     __
          * |         | 
          * |         |    Move curr into hole
          * |         |
          * |  Hole   |    __     
          * |         |
          * |         |    No move (condition 1)
          * |         |
          * | Current |    __
          * |         |
          * |         |    Move curr into hole
          * |_________|    __ 
          */

         pArray[hole] = pArray[curr];

      } /* end of infinite while (true) loop */

   } /* end if ( keyFound )    */


   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool txHashResize( psonTx             * pTx,
                   psonSessionContext * pContext )
{
   size_t i, rowNumber = 0, len;
   psonMemObject ** pArray, **ptr;
   size_t newSize;
   
   GET_PTR( pArray, pTx->listOfLocks.arrayOffset, psonMemObject * );

   newSize = g_psonArrayLengths[pTx->listOfLocks.lengthIndex+1];

   len = newSize * sizeof(void *);

   ptr = (psonMemObject **) psonMalloc( &pTx->memObject, len, pContext );
   if ( ptr == NULL ) return false;

   memset( ptr, 0, len );

   for ( i = 0; i < g_psonArrayLengths[pTx->listOfLocks.lengthIndex]; ++i) {
      if ( pArray[i] != NULL ) {
         
         rowNumber =  hash_it( &pArray[i], sizeof(void *) ) % newSize;
         
         while (ptr[rowNumber] != NULL) {
            ++rowNumber;
            if ( rowNumber == newSize ) rowNumber = 0;
         }
         /* We found a valid spot */
         ptr[rowNumber] = pArray[i];
      }
   }

   len = g_psonArrayLengths[pTx->listOfLocks.lengthIndex];
   
   pTx->listOfLocks.lengthIndex++;
   pTx->listOfLocks.arrayOffset = SET_OFFSET( ptr );

   psonFree( &pTx->memObject, (unsigned char *)pArray, len, pContext );

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSON_TRANSACTION_HASH_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
