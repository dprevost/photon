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

#include "Engine/Hash.h"
#include "Engine/hash_fnv.h"
#include "Engine/SessionContext.h"
#include "Engine/MemoryObject.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * The array length (for the hash) will double or shrink by a factor two
 * using precomputed lengths (all the lengths are prime numbers - using
 * prime numbers help reduces the risk of collision.
 *
 * To make it simple, we use the first prime number less than 2^n (except 
 * for 29 to make the jump from 13 to 61 smoother).
 *
 * The list of prime numbers was obtained from http://primes.utm.edu/,
 * specifically the page http://primes.utm.edu/lists/2small/0bit.html
 * (which list the first 10 prime numbers less than 2^n for n up to 400).
 */
#if SIZEOF_VOID_P == 4
size_t g_psnArrayLengths[PSN_PRIME_NUMBER_ARRAY_LENGTH] = 
{
   13,     
   29,
   61,
   127,
   251,
   509,
   1021,
   2039,
   4093,
   8191,
   0x00004000 - 3,  /* 2^14 - 3  */
   0x00008000 - 19, /* 2^15 - 19 */
   0x00010000 - 15, /* etc...    */
   0x00020000 - 1,
   0x00040000 - 5,
   0x00080000 - 1,
   0x00100000 - 3,
   0x00200000 - 9,
   0x00400000 - 3,
   0x00800000 - 15,
   0x01000000 - 3,
   0x02000000 - 39,
   0x04000000 - 5,
   0x08000000 - 39,
   0x10000000 - 57,
   0x20000000 - 3,
   0x40000000 - 35,
   0x80000000 - 1
};
#else
size_t g_psnArrayLengths[PSN_PRIME_NUMBER_ARRAY_LENGTH] = 
{
   13,     
   29,
   61,
   127,
   251,
   509,
   1021,
   2039,
   4093,
   8191,
   0x0000000000004000 - 3,  /* 2^14 - 3 */
   0x0000000000008000 - 19,
   0x0000000000010000 - 15,
   0x0000000000020000 - 1,
   0x0000000000040000 - 5,
   0x0000000000080000 - 1,
   0x0000000000100000 - 3,
   0x0000000000200000 - 9,
   0x0000000000400000 - 3,
   0x0000000000800000 - 15,
   0x0000000001000000 - 3,
   0x0000000002000000 - 39,
   0x0000000004000000 - 5,
   0x0000000008000000 - 39,
   0x0000000010000000 - 57,
   0x0000000020000000 - 3,
   0x0000000040000000 - 35,
   0x0000000080000000 - 1,
   0x0000000100000000 - 5,
   0x0000000200000000 - 9,
   0x0000000400000000 - 41,
   0x0000000800000000 - 31,
   0x0000001000000000 - 5,
   0x0000002000000000 - 25,
   0x0000004000000000 - 45,
   0x0000008000000000 - 7,
   0x0000010000000000 - 87,
   0x0000020000000000 - 21,
   0x0000040000000000 - 11,
   0x0000080000000000 - 57,
   0x0000100000000000 - 17,
   0x0000200000000000 - 55,
   0x0000400000000000 - 21,
   0x0000800000000000 - 115,
   0x0001000000000000 - 59,
   0x0002000000000000 - 81,
   0x0004000000000000 - 27,
   0x0008000000000000 - 129,
   0x0010000000000000 - 47,
   0x0020000000000000 - 111,
   0x0040000000000000 - 33,
   0x0080000000000000 - 55,
   0x0100000000000000 - 5,
   0x0200000000000000 - 13,
   0x0400000000000000 - 27,
   0x0800000000000000 - 55,
   0x1000000000000000 - 93,
   0x2000000000000000 - 1,
   0x4000000000000000 - 57,
   0x8000000000000000 - 25,

};

#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define HASH_ARRAY_MIN_SIZE 13   /* a prime number */

/** In % */
static unsigned int g_maxLoadFactor = 300;
static unsigned int g_minLoadFactor = 100;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- 
 *
 * Static inline functions are first.
 * 
 * --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline 
size_t calculateItemLength( size_t keyLength,
                            size_t dataLength )
{
   size_t len;
   
   len = offsetof(psnHashItem, key) + keyLength;
   len = ((len-1)/PSC_ALIGNMENT_STRUCT + 1)*PSC_ALIGNMENT_STRUCT;
   
   len += dataLength;
   
   return len;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Just in case a new function is used instead, one of these days */
static inline u_long
hash_it (const unsigned char * str, size_t len)
{
   return fnv_buf( (void *)str, len, FNV1_INIT );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline 
psnHashResizeEnum isItTimeToResize( psnHash * pHash )
{
   unsigned int loadFactor = 100 * pHash->numberOfItems / 
      g_psnArrayLengths[pHash->lengthIndex];

  if ( loadFactor >= g_maxLoadFactor ) return PSN_HASH_TIME_TO_GROW;
  if ( pHash->lengthIndex > pHash->lengthIndexMinimum ) {
     if ( loadFactor <= g_minLoadFactor ) return PSN_HASH_TIME_TO_SHRINK;
  }
  
  return PSN_HASH_NO_RESIZE;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- 
 *
 * Static non-inline functions.
 * 
 * --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static bool findKey( psnHash            * pHash,
                     ptrdiff_t           * pArray,
                     const unsigned char * pKey,
                     size_t                keyLength,
                     psnHashItem       ** ppItem,
                     psnHashItem       ** ppPreviousItem,
                     size_t              * pBucket )
{
   ptrdiff_t currentOffset, nextOffset;
   psnHashItem* pItem;

   *pBucket = hash_it( pKey, keyLength ) % g_psnArrayLengths[pHash->lengthIndex];
   currentOffset = pArray[*pBucket];
   
   *ppPreviousItem = NULL;

   while ( currentOffset != PSN_NULL_OFFSET ) {
      GET_PTR( pItem, currentOffset, psnHashItem );
      nextOffset = pItem->nextItem;
     
      if ( keyLength == pItem->keyLength ) {
         if ( memcmp( pKey, pItem->key, keyLength ) == 0 ) {
            *ppItem = pItem;
            return true;
         }
      }

      /* Move to the next item in our bucket */
      currentOffset = nextOffset;
      *ppPreviousItem = pItem;
   }
   
   /* Nothing was found, return false */
   *ppItem = NULL;
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- 
 *
 * Functions declared in Hash.h (alphabetic order).
 * 
 * --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum vdsErrors psnHashCopy( psnHash           * pOldHash,
                             psnHash           * pNewHash,
                             psnSessionContext * pContext )
{
   ptrdiff_t * pOldArray, * pNewArray;
   size_t i;
   ptrdiff_t currentOffset, tempOffset, newBucket;
   psnHashItem * pOldItem, * pNewItem, * previousItem, * tempItem;
   psnMemObject * pMemObject;
   size_t itemLength;
   
   VDS_PRE_CONDITION( pOldHash != NULL );
   VDS_PRE_CONDITION( pNewHash != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_INV_CONDITION( pOldHash->initialized == PSN_HASH_SIGNATURE );
   VDS_INV_CONDITION( pNewHash->initialized == PSN_HASH_SIGNATURE );

   GET_PTR( pOldArray, pOldHash->arrayOffset, ptrdiff_t );
   VDS_INV_CONDITION( pOldArray != NULL );
   GET_PTR( pNewArray, pNewHash->arrayOffset, ptrdiff_t );
   VDS_INV_CONDITION( pNewArray != NULL );

   GET_PTR( pMemObject, pNewHash->memObjOffset, psnMemObject );

   if ( pOldHash->lengthIndex == pNewHash->lengthIndex ) {
      /* Much simpler path... hashing is done for us */
      for ( i = 0; i < g_psnArrayLengths[pOldHash->lengthIndex]; ++i ) {

         currentOffset = pOldArray[i];
         previousItem = NULL;
         while ( currentOffset != PSN_NULL_OFFSET ) {
            GET_PTR( pOldItem, currentOffset, psnHashItem );

            itemLength = calculateItemLength( pOldItem->keyLength,
                                              pOldItem->dataLength );
            pNewItem = (psnHashItem*) psnMalloc( pMemObject, 
                                                   itemLength, 
                                                   pContext );
            if ( pNewItem == NULL ) return VDS_NOT_ENOUGH_VDS_MEMORY;

            /*
             * We copy everything and we reset the offset of interest
             * to proper values (the nextSameKey offset should always be 
             * PSN_NULL_OFFSET since the oldHash is for a read-only object).
             */
            memcpy( pNewItem, pOldItem, itemLength );
            pNewItem->dataOffset = SET_OFFSET(pNewItem) + itemLength - 
                                   pOldItem->dataLength;
            /* Set the chain */
            if ( previousItem == NULL ) {
               pNewArray[i] = SET_OFFSET(pNewItem);
            }
            else {
               previousItem->nextItem = SET_OFFSET(pNewItem);
            }
            previousItem = pNewItem;
            
            currentOffset = pOldItem->nextItem;
         }
      }
   }
   else {
      for ( i = 0; i < g_psnArrayLengths[pOldHash->lengthIndex]; ++i ) {
         currentOffset = pOldArray[i];
      
         while ( currentOffset != PSN_NULL_OFFSET ) {
            GET_PTR( pOldItem, currentOffset, psnHashItem );

            itemLength = calculateItemLength( pOldItem->keyLength,
                                              pOldItem->dataLength );
            pNewItem = (psnHashItem*) psnMalloc( pMemObject, 
                                                   itemLength, 
                                                   pContext );
            if ( pNewItem == NULL ) return VDS_NOT_ENOUGH_VDS_MEMORY;

            /*
             * We copy everything and we reset the offset of interest
             * to proper values (the nextSameKey offset should always be 
             * PSN_NULL_OFFSET since the oldHash is for a read-only object).
             */
            memcpy( pNewItem, pOldItem, itemLength );
            pNewItem->dataOffset = SET_OFFSET(pNewItem) + itemLength - 
                                   pOldItem->dataLength;

            /* Set the chain */
            newBucket = hash_it( pNewItem->key, pNewItem->keyLength ) % 
                        g_psnArrayLengths[pNewHash->lengthIndex];
            pNewItem->bucket = newBucket;
            if ( pNewArray[newBucket] == PSN_NULL_OFFSET ) {
               pNewArray[newBucket] = SET_OFFSET(pNewItem);
            }
            else {
               tempOffset = pNewArray[newBucket];
               GET_PTR( tempItem, tempOffset, psnHashItem );
               while ( tempItem->nextItem != PSN_NULL_OFFSET ) {
                  tempOffset = tempItem->nextItem;
                  GET_PTR( tempItem, tempOffset, psnHashItem );
               }
               tempItem->nextItem = SET_OFFSET(pNewItem);
            }
            
            /* Move to the next item in our bucket */
            currentOffset = pOldItem->nextItem;
         }
      }
   }

   pNewHash->totalDataSizeInBytes = pOldHash->totalDataSizeInBytes;
   pNewHash->numberOfItems        = pOldHash->numberOfItems;
   pNewHash->enumResize           = pOldHash->enumResize;

   return VDS_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psnHashDelWithItem( psnHash           * pHash,
                          psnHashItem       * pItem,
                          psnSessionContext * pContext )
{
   ptrdiff_t * pArray;
   psnHashItem * pNewItem = NULL, * previousItem = NULL;
   ptrdiff_t nextOffset;
   psnMemObject * pMemObject;
   size_t bucket;
   
   VDS_PRE_CONDITION( pHash    != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pItem    != NULL );
   VDS_INV_CONDITION( pHash->initialized == PSN_HASH_SIGNATURE );
   
   GET_PTR( pArray, pHash->arrayOffset, ptrdiff_t );
   VDS_INV_CONDITION( pArray != NULL );

   GET_PTR( pMemObject, pHash->memObjOffset, psnMemObject );
   bucket = pItem->bucket;
   
   nextOffset = pArray[bucket];
   while ( nextOffset != PSN_NULL_OFFSET ) {
      previousItem = pNewItem;
      GET_PTR( pNewItem, nextOffset, psnHashItem );
      if ( pNewItem == pItem ) break;
      nextOffset = pNewItem->nextItem;
   }
   VDS_INV_CONDITION( pNewItem == pItem );

   nextOffset = pItem->nextItem;
      
   pHash->totalDataSizeInBytes -= pItem->dataLength;
   psnFree( pMemObject, 
             (unsigned char*)pItem, 
             calculateItemLength(pItem->keyLength,pItem->dataLength),
             pContext );
                
   if ( previousItem == NULL ) {
      pArray[bucket] = nextOffset;
   }
   else {
      previousItem->nextItem = nextOffset;
   }
   
   pHash->numberOfItems--;

   pHash->enumResize = isItTimeToResize( pHash );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Note: there is no "same key" here since this function is used for doing
 * changes to a temp. copy of read-only objects - no transaction.
 */
bool psnHashDelWithKey( psnHash            * pHash,
                         const unsigned char * pKey, 
                         size_t                keyLength,
                         psnSessionContext  * pContext  )
{
   size_t bucket = 0;
   ptrdiff_t * pArray;
   bool keyFound = false;
   psnHashItem * previousItem = NULL, * pItem = NULL;
   ptrdiff_t nextOffset;
   psnMemObject * pMemObject;
  
   VDS_PRE_CONDITION( pHash     != NULL );
   VDS_PRE_CONDITION( pContext  != NULL );
   VDS_PRE_CONDITION( pKey      != NULL );
   VDS_PRE_CONDITION( keyLength > 0 );
   VDS_INV_CONDITION( pHash->initialized == PSN_HASH_SIGNATURE );
   
   GET_PTR( pArray, pHash->arrayOffset, ptrdiff_t );
   VDS_INV_CONDITION( pArray != NULL );

   GET_PTR( pMemObject, pHash->memObjOffset, psnMemObject );
   
   keyFound = findKey( pHash, 
                       pArray, 
                       pKey, 
                       keyLength, 
                       &pItem, 
                       &previousItem, 
                       &bucket );
   
   if ( keyFound ) {
      nextOffset  = pItem->nextItem;
      
      pHash->totalDataSizeInBytes -= pItem->dataLength;
      psnFree( pMemObject, 
                (unsigned char*)pItem, 
                calculateItemLength(pItem->keyLength,pItem->dataLength),
                pContext );
                
      if ( previousItem == NULL ) {
         pArray[bucket] = nextOffset;
      }
      else {
         previousItem->nextItem = nextOffset;
      }
      
      pHash->numberOfItems--;
      pHash->enumResize = isItTimeToResize( pHash );

      return true;
   }

   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psnHashEmpty( psnHash           * pHash,
                    psnSessionContext * pContext )
{
   ptrdiff_t* pArray, currentOffset, nextOffset;
   size_t i;
   psnHashItem* pItem;
   psnMemObject * pMemObject;
   
   VDS_PRE_CONDITION( pHash != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_INV_CONDITION( pHash->initialized == PSN_HASH_SIGNATURE );
   
   GET_PTR( pArray, pHash->arrayOffset, ptrdiff_t );
   VDS_INV_CONDITION( pArray != NULL );

   GET_PTR( pMemObject, pHash->memObjOffset, psnMemObject );

   for ( i = 0; i < g_psnArrayLengths[pHash->lengthIndex]; ++i ) {
      currentOffset = pArray[i];
      
      while ( currentOffset != PSN_NULL_OFFSET ) {
         GET_PTR( pItem, currentOffset, psnHashItem );
         nextOffset = pItem->nextItem;
         
         psnFree( pMemObject, 
                   (unsigned char*) pItem, 
                   calculateItemLength(pItem->keyLength,pItem->dataLength), 
                   pContext );
         
         /* Move to the next item in our bucket */
         currentOffset = nextOffset;
      }
      
      pArray[i] = PSN_NULL_OFFSET;
   }
   
   pHash->numberOfItems = 0;
   pHash->totalDataSizeInBytes = 0;
   pHash->enumResize = isItTimeToResize( pHash );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psnHashFini( psnHash * pHash )
{
   VDS_PRE_CONDITION( pHash != NULL );
   VDS_INV_CONDITION( pHash->initialized == PSN_HASH_SIGNATURE );
   
   pHash->initialized = 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnHashGet( psnHash            * pHash,
                  const unsigned char * pKey,
                  size_t                keyLength,
                  psnHashItem       ** ppItem,
                  size_t              * pBucket,
                  psnSessionContext  * pContext )
{
   bool keyFound = false;
   ptrdiff_t* pArray;
   psnHashItem* pItem, *dummy;
   
   VDS_PRE_CONDITION( pHash    != NULL );
   VDS_PRE_CONDITION( pKey     != NULL );
   VDS_PRE_CONDITION( ppItem   != NULL );
   VDS_PRE_CONDITION( pBucket  != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( keyLength > 0 );
   VDS_INV_CONDITION( pHash->initialized == PSN_HASH_SIGNATURE );
   
   GET_PTR( pArray, pHash->arrayOffset, ptrdiff_t );
   VDS_INV_CONDITION( pArray != NULL );

   keyFound = findKey( pHash, pArray, pKey, keyLength, 
                       &pItem, &dummy, pBucket );

   if ( keyFound ) *ppItem = pItem;

   return keyFound;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined(WIN32)
/* unreachable code */
#  pragma warning(disable:4702) 
#endif

bool psnHashGetFirst( psnHash  * pHash,
                       ptrdiff_t * pFirstItemOffset )
{
   ptrdiff_t* pArray, currentOffset;
   bool SHOULD_NOT_REACHED_THIS = true;
   size_t i;
   
   VDS_PRE_CONDITION( pHash != NULL );
   VDS_PRE_CONDITION( pFirstItemOffset != NULL );
   
   VDS_INV_CONDITION( pHash->initialized == PSN_HASH_SIGNATURE );
   
   GET_PTR( pArray, pHash->arrayOffset, ptrdiff_t );
   VDS_INV_CONDITION( pArray != NULL );

   if ( pHash->numberOfItems == 0 ) return false;
   
   /* 
    * Note: the first item has to be the first non-empty pArray[i],
    * this makes the search easier.
    */
   for ( i = 0; i < g_psnArrayLengths[pHash->lengthIndex]; ++i ) {
      currentOffset = pArray[i];
      
      if (currentOffset != PSN_NULL_OFFSET ) {
         *pFirstItemOffset = currentOffset;
         return true;
      }
   }

   VDS_POST_CONDITION( SHOULD_NOT_REACHED_THIS == false );
   
   return false; /* Should never occur */
}

#if defined (WIN32)
#  pragma warning(default:4702) /* unreachable code */
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnHashGetNext( psnHash  * pHash,
                      ptrdiff_t   previousOffset,
                      ptrdiff_t * pNextItemOffset )
{
   ptrdiff_t* pArray, currentOffset;
   size_t i;
   psnHashItem* pItem;
   
   VDS_PRE_CONDITION( pHash != NULL );
   VDS_PRE_CONDITION( pNextItemOffset != NULL );
   VDS_PRE_CONDITION( previousOffset != PSN_NULL_OFFSET );
   VDS_INV_CONDITION( pHash->initialized == PSN_HASH_SIGNATURE );
   
   GET_PTR( pArray, pHash->arrayOffset, ptrdiff_t );
   VDS_INV_CONDITION( pArray != NULL );

   GET_PTR( pItem, previousOffset, psnHashItem );
   if ( pItem->nextItem != PSN_NULL_OFFSET ) {
      /* We found the next one in the linked list. */
      *pNextItemOffset = pItem->nextItem;
      return true;
   }
   
   /* 
    * Note: the next item has to be the first non-empty pArray[i] beyond
    * the current bucket (previousBucket). 
    */
   for ( i = pItem->bucket+1; i < g_psnArrayLengths[pHash->lengthIndex]; ++i ) {
      currentOffset = pArray[i];
      
      if (currentOffset != PSN_NULL_OFFSET ) {
         *pNextItemOffset = currentOffset;
         return true;
      }
   }
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum vdsErrors psnHashInit( psnHash           * pHash,
                             ptrdiff_t            memObjOffset,
                             size_t               reservedSize, 
                             psnSessionContext * pContext )
{
   size_t len, numBuckets;
   ptrdiff_t* ptr;
   unsigned int i;
   psnMemObject * pMemObject;
   
   VDS_PRE_CONDITION( pHash != NULL );
   VDS_PRE_CONDITION( pContext != NULL );

   pHash->numberOfItems = 0;
   pHash->totalDataSizeInBytes = 0;
   pHash->enumResize = PSN_HASH_NO_RESIZE;
   
   GET_PTR( pMemObject, memObjOffset, psnMemObject );
   /*
    * reservedSize... In real life what it means is that we cannot shrink 
    * the array to a point where we would need to increase it in order
    * to hold reservedSize items.
    *
    * Since a ratio (load factor) of 1.5-2 is considered optimal (based on
    * research done on the web...) we will use a load factor of 1.75 to 
    * calculate the number of buckets.
    */
   numBuckets = reservedSize * 100 / 175;
   
   /* Which one of our lengths is closer but larger than numBuckets? */
   pHash->lengthIndex = pHash->lengthIndexMinimum = 0;
   for ( i = 1; i < PSN_PRIME_NUMBER_ARRAY_LENGTH; ++i ) {
      if ( g_psnArrayLengths[i] > numBuckets ) {
         pHash->lengthIndex = i - 1;
         pHash->lengthIndexMinimum = i - 1;
         break;
      }
   }
   
   len = g_psnArrayLengths[pHash->lengthIndex] * sizeof(ptrdiff_t);
   
   ptr = (ptrdiff_t*) psnMalloc( pMemObject, len, pContext );
   if ( ptr != NULL ) {
      for ( i = 0; i < g_psnArrayLengths[pHash->lengthIndex]; ++i) {
         ptr[i] = PSN_NULL_OFFSET;
      }
      
      pHash->arrayOffset = SET_OFFSET( ptr );
      pHash->initialized = PSN_HASH_SIGNATURE;
      pHash->memObjOffset = memObjOffset;
   
      return VDS_OK;
   }
   
   return VDS_NOT_ENOUGH_VDS_MEMORY;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum vdsErrors psnHashInsert( psnHash            * pHash,
                               const unsigned char * pKey,
                               size_t                keyLength,
                               const void          * pData,
                               size_t                dataLength,
                               psnHashItem       ** ppNewItem,
                               psnSessionContext  * pContext )
{
   ptrdiff_t* pArray;   
   size_t bucket = 0;
   bool   keyFound = false;
   psnHashItem* pItem, *previousItem = NULL;
   size_t itemLength;
   psnMemObject * pMemObject;
   
   VDS_PRE_CONDITION( pHash     != NULL );
   VDS_PRE_CONDITION( pContext  != NULL );
   VDS_PRE_CONDITION( pKey      != NULL );
   VDS_PRE_CONDITION( pData     != NULL );
   VDS_PRE_CONDITION( ppNewItem != NULL );
   VDS_PRE_CONDITION( keyLength  > 0 );
   VDS_PRE_CONDITION( dataLength > 0 );

   VDS_INV_CONDITION( pHash->initialized == PSN_HASH_SIGNATURE );
   
   GET_PTR( pArray, pHash->arrayOffset, ptrdiff_t );
   VDS_INV_CONDITION( pArray != NULL );

   keyFound = findKey( pHash, pArray, pKey, keyLength, 
                       &pItem, &previousItem, &bucket );

   if ( keyFound ) return VDS_ITEM_ALREADY_PRESENT;

   GET_PTR( pMemObject, pHash->memObjOffset, psnMemObject );
   
   /* The whole item is allocated in one step, header+data, to minimize */
   /* overheads of the memory allocator */
   itemLength = calculateItemLength( keyLength, dataLength );
   pItem = (psnHashItem*) psnMalloc( pMemObject, itemLength, pContext );
   if ( pItem == NULL ) return VDS_NOT_ENOUGH_VDS_MEMORY;
   
   pItem->nextItem = PSN_NULL_OFFSET;
   pItem->bucket = bucket;
   
   /* keyLength must be set before calling getData() */   
   pItem->keyLength = keyLength;
   pItem->dataLength = dataLength;
   pItem->dataOffset = SET_OFFSET(pItem) + itemLength - dataLength;
   pItem->nextSameKey = PSN_NULL_OFFSET;
   
   memcpy( pItem->key,     pKey, keyLength );
   memcpy( GET_PTR_FAST(pItem->dataOffset, unsigned char), pData, dataLength );

   pHash->totalDataSizeInBytes += dataLength;
   pHash->numberOfItems++;

   pHash->enumResize = isItTimeToResize( pHash );
   
   if ( previousItem == NULL ) {
      pArray[bucket] = SET_OFFSET(pItem);
   }
   else {
      previousItem->nextItem = SET_OFFSET(pItem);
   }
   
   *ppNewItem = pItem;

   return VDS_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum vdsErrors psnHashInsertAt( psnHash            * pHash,
                                 size_t                bucket,
                                 const unsigned char * pKey,
                                 size_t                keyLength,
                                 const void          * pData,
                                 size_t                dataLength,
                                 psnHashItem       ** ppNewItem,
                                 psnSessionContext  * pContext )
{
   ptrdiff_t* pArray;   
   psnHashItem* pItem, *previousItem = NULL;
   size_t itemLength;
   psnMemObject * pMemObject;
   ptrdiff_t currentOffset;
   
   VDS_PRE_CONDITION( pHash     != NULL );
   VDS_PRE_CONDITION( pContext  != NULL );
   VDS_PRE_CONDITION( pKey      != NULL );
   VDS_PRE_CONDITION( pData     != NULL );
   VDS_PRE_CONDITION( ppNewItem != NULL );
   VDS_PRE_CONDITION( keyLength  > 0 );
   VDS_PRE_CONDITION( dataLength > 0 );
   VDS_PRE_CONDITION( bucket < g_psnArrayLengths[pHash->lengthIndex] );

   VDS_INV_CONDITION( pHash->initialized == PSN_HASH_SIGNATURE );
   
   GET_PTR( pArray, pHash->arrayOffset, ptrdiff_t );
   VDS_INV_CONDITION( pArray != NULL );

   currentOffset = pArray[bucket];

   while ( currentOffset != PSN_NULL_OFFSET ) {
      GET_PTR( previousItem, currentOffset, psnHashItem );
      currentOffset = previousItem->nextItem;     
   }

   GET_PTR( pMemObject, pHash->memObjOffset, psnMemObject );
   
   /* The whole item is allocated in one step, header+data, to minimize */
   /* overheads of the memory allocator */
   itemLength = calculateItemLength( keyLength, dataLength );
   pItem = (psnHashItem*) psnMalloc( pMemObject, itemLength, pContext );
   if ( pItem == NULL ) return VDS_NOT_ENOUGH_VDS_MEMORY;
   
   pItem->nextItem = PSN_NULL_OFFSET;
   pItem->bucket = bucket;
   
   /* keyLength must be set before calling getData() */   
   pItem->keyLength = keyLength;
   pItem->dataLength = dataLength;
   pItem->dataOffset = SET_OFFSET(pItem) + itemLength - dataLength;
   pItem->nextSameKey = PSN_NULL_OFFSET;
    
   memcpy( pItem->key,     pKey, keyLength );
   memcpy( GET_PTR_FAST(pItem->dataOffset, unsigned char), pData, dataLength );

   pHash->totalDataSizeInBytes += dataLength;
   pHash->numberOfItems++;
   pHash->enumResize = isItTimeToResize( pHash );

   if ( previousItem == NULL ) {
      pArray[bucket] = SET_OFFSET(pItem);
   }
   else {
      previousItem->nextItem = SET_OFFSET(pItem);
   }
   
   *ppNewItem = pItem;

   return VDS_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum vdsErrors psnHashResize( psnHash           * pHash,
                               psnSessionContext * pContext )
{
   int newIndexLength;
   ptrdiff_t * ptr, * pArray;
   size_t len, i;
   ptrdiff_t currentOffset, nextOffset, newBucket, newOffset;
   psnHashItem * pItem, * pNewItem;
   psnMemObject * pMemObject;
  
   VDS_PRE_CONDITION( pHash != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_INV_CONDITION( pHash->initialized == PSN_HASH_SIGNATURE );
   
   GET_PTR( pArray, pHash->arrayOffset, ptrdiff_t );
   VDS_INV_CONDITION( pArray != NULL );

   if ( pHash->enumResize == PSN_HASH_NO_RESIZE ) return VDS_OK;

   GET_PTR( pMemObject, pHash->memObjOffset, psnMemObject );

   if ( pHash->enumResize == PSN_HASH_TIME_TO_GROW ) {
      newIndexLength = pHash->lengthIndex + 1;
   }
   else {
      newIndexLength = pHash->lengthIndex - 1;     
   }
   
   len = g_psnArrayLengths[newIndexLength] * sizeof(ptrdiff_t);
   
   ptr = (ptrdiff_t*) psnMalloc( pMemObject, len, pContext );
   if ( ptr == NULL ) return VDS_NOT_ENOUGH_VDS_MEMORY;

   for ( i = 0; i < g_psnArrayLengths[newIndexLength]; ++i) {
      ptr[i] = PSN_NULL_OFFSET;
   }
   
   for ( i = 0; i < g_psnArrayLengths[pHash->lengthIndex]; ++i ) {
      currentOffset = pArray[i];
      
      while ( currentOffset != PSN_NULL_OFFSET ) {
         GET_PTR( pItem, currentOffset, psnHashItem );
         nextOffset = pItem->nextItem;
         pItem->nextItem = PSN_NULL_OFFSET;
         
         newBucket = hash_it( pItem->key, pItem->keyLength ) % 
                     g_psnArrayLengths[newIndexLength];
         pItem->bucket = newBucket;
         if ( ptr[newBucket] == PSN_NULL_OFFSET ) {
            ptr[newBucket] = currentOffset;
         }
         else {
            newOffset = ptr[newBucket];
            GET_PTR( pNewItem, newOffset, psnHashItem );
            while ( pNewItem->nextItem != PSN_NULL_OFFSET ) {
               newOffset = pNewItem->nextItem;
               GET_PTR( pNewItem, newOffset, psnHashItem );
            }
            pNewItem->nextItem = currentOffset;
         }
         
         /* Move to the next item in our bucket */
         currentOffset = nextOffset;
      }
   }
   
   len = g_psnArrayLengths[pHash->lengthIndex]*sizeof(ptrdiff_t);

   pHash->lengthIndex = newIndexLength;
   pHash->arrayOffset = SET_OFFSET( ptr );

   psnFree( pMemObject,
             (unsigned char*)pArray,
             len, 
             pContext );
   
   pHash->enumResize = PSN_HASH_NO_RESIZE;

   return VDS_OK;   
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Warning: this function should only be used when working with a temp copy
 * of read-only maps. For read-write containers, do not use as you cannot
 * rollback!
 */

enum vdsErrors 
psnHashUpdate( psnHash            * pHash,
                const unsigned char * pKey,
                size_t                keyLength,
                const void          * pData,
                size_t                dataLength,
                psnSessionContext  * pContext )
{
   size_t bucket = 0;
   bool keyFound = false;
   ptrdiff_t * pArray;
   size_t newItemLength, oldItemLength;
   psnHashItem * pOldItem, * previousItem = NULL, * pNewItem = NULL;
   psnMemObject * pMemObject;

   VDS_PRE_CONDITION( pHash    != NULL );
   VDS_PRE_CONDITION( pKey     != NULL );
   VDS_PRE_CONDITION( pData    != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( keyLength  > 0 );
   VDS_PRE_CONDITION( dataLength > 0 );
   VDS_INV_CONDITION( pHash->initialized == PSN_HASH_SIGNATURE );

   GET_PTR( pArray, pHash->arrayOffset, ptrdiff_t );
   VDS_INV_CONDITION( pArray != NULL );

   keyFound = findKey( pHash, pArray, pKey, keyLength,
                       &pOldItem, &previousItem, &bucket );

   if ( ! keyFound ) return VDS_NO_SUCH_ITEM;

   newItemLength = calculateItemLength( keyLength, dataLength );
   oldItemLength = calculateItemLength( keyLength, pOldItem->dataLength );

   if ( newItemLength == oldItemLength ) {
      /*
       * Less work! We can just copy the data. We do not care about crash 
       * recovery -> this a temp copy and it will be removed if the program
       * crashes.
       */
      memcpy( GET_PTR_FAST(pOldItem->dataOffset, void), pData, dataLength );
      pHash->totalDataSizeInBytes += (dataLength - pOldItem->dataLength);
      pOldItem->dataLength = dataLength;
   }
   else {
      pMemObject = GET_PTR_FAST( pHash->memObjOffset, psnMemObject );
      pNewItem = (psnHashItem*) 
         psnMalloc( pMemObject, newItemLength, pContext );
      if ( pNewItem == NULL ) return VDS_NOT_ENOUGH_VDS_MEMORY;
      
      /* initialize the new record */
      pNewItem->nextItem = pOldItem->nextItem;
      pNewItem->bucket = pOldItem->bucket;
      pNewItem->keyLength = keyLength;
      pNewItem->dataLength = dataLength;
      pNewItem->dataOffset = SET_OFFSET(pNewItem) + newItemLength - dataLength;
      pNewItem->nextSameKey = PSN_NULL_OFFSET;
   
      memcpy( pNewItem->key, pKey, keyLength );
      memcpy( GET_PTR_FAST(pNewItem->dataOffset, unsigned char), pData, dataLength );

      pHash->totalDataSizeInBytes += (dataLength - pOldItem->dataLength);

      if ( previousItem == NULL ) {
         pArray[bucket] = SET_OFFSET(pNewItem);
      }
      else {
         previousItem->nextItem = SET_OFFSET(pNewItem);
      }
      
      /* Eliminate the old record */
      psnFree( pMemObject, (unsigned char*)pOldItem, oldItemLength, pContext );
   }
   
    return VDS_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

