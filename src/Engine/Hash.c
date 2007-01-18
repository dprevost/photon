/*
 * Copyright (C) 2006-2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

#include "Hash.h"
#include "SessionContext.h"
#include "MemoryObject.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define HASH_ARRAY_MIN_SIZE 13   /* a prime number */

/** In % */
static unsigned int g_maxLoadFactor = 300;
static unsigned int g_minLoadFactor = 100;

/**
 * The array lenght (for the hash) will double or shrink by a factor two
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
#  define PRIME_NUMBER_ARRAY_LENGTH 28
static size_t g_arrayLengths[PRIME_NUMBER_ARRAY_LENGTH] = 
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
#  define PRIME_NUMBER_ARRAY_LENGTH 60
static size_t g_arrayLengths[PRIME_NUMBER_ARRAY_LENGTH] = 
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
   
   len = offsetof(vdseHashItem, key) + keyLength;
   len = ((len-1)/VDST_STRUCT_ALIGNMENT + 1)*VDST_STRUCT_ALIGNMENT;
   
   len += dataLength;
   
   return len;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline 
unsigned char* getData( vdseHashItem* pItem )
{
   size_t len;
   
   len = offsetof(vdseHashItem, key) + pItem->keyLength;
   len = ((len-1)/VDST_STRUCT_ALIGNMENT + 1)*VDST_STRUCT_ALIGNMENT;
   
   return (unsigned char*)pItem + len;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline u_long
hash_pjw (const unsigned char *str, size_t len)
{
   u_long hash = 0, g;
   size_t i;
   
   for ( i = 0; i < len; i++ )
   {
      const unsigned char temp = str[i];
      hash = (hash << 4) + (temp * 13);

      g = hash & 0xf0000000;
      if (g)
      {
         hash ^= (g >> 24);
         hash ^= g;
      }
   }

   return hash;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline 
vdseHashResizeEnum isItTimeToResize( vdseHash* pHash )
{
   unsigned int loadFactor = 100 * pHash->numberOfItems / 
      g_arrayLengths[pHash->lengthIndex];

  if ( loadFactor >= g_maxLoadFactor )
     return VDSE_HASH_TIME_TO_GROW;
  if ( pHash->lengthIndex > pHash->lengthIndexMinimum ) 
     if ( loadFactor <= g_minLoadFactor ) 
        return VDSE_HASH_TIME_TO_SHRINK;
  return VDSE_HASH_NO_RESIZE;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- 
 *
 * Static non-inline functions.
 * 
 * --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static bool findKey( vdseHash*            pHash,
                     ptrdiff_t*           pArray,
                     const unsigned char* pKey,
                     size_t               keyLength,
                     vdseHashItem**       ppItem,
                     vdseHashItem**       ppPreviousItem,
                     size_t*              pBucket )
{
   ptrdiff_t currentOffset, nextOffset;
   vdseHashItem* pItem;

   *pBucket = hash_pjw( pKey, keyLength ) % g_arrayLengths[pHash->lengthIndex];
   currentOffset = pArray[*pBucket];
   
   *ppPreviousItem = NULL;

   while ( currentOffset != NULL_OFFSET )
   {
      pItem = GET_PTR( currentOffset, vdseHashItem );
      nextOffset = pItem->nextItem;
     
      if ( keyLength == pItem->keyLength )
      {
         if ( memcmp( pKey, pItem->key, keyLength ) == 0 )
         {
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

enum ListErrors 
vdseHashDelete( vdseHash*            pHash,
                const unsigned char* pKey, 
                size_t               keyLength,
                vdseSessionContext*  pContext  )
{
   size_t bucket = 0;
   ptrdiff_t* pArray;
   bool keyFound = false;
   vdseHashItem* pItem, *previousItem = NULL;
   
   VDS_PRE_CONDITION( pHash != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_INV_CONDITION( pHash->initialized == VDSE_HASH_SIGNATURE );
   
   SET_PTR( pArray, pHash->arrayOffset, ptrdiff_t );
   VDS_INV_CONDITION( pArray != NULL );

   keyFound = findKey( pHash, pArray, pKey, keyLength, 
                       &pItem, &previousItem, &bucket );
   if ( keyFound )   
   {
      pHash->totalDataSizeInBytes -= pItem->dataLength;
      vdseFree( (vdseMemObject*)pContext->pCurrentMemObject, 
                (unsigned char*)pItem, 
                calculateItemLength(pItem->keyLength,pItem->dataLength),
                pContext );
      if ( previousItem == NULL )
         pArray[bucket] = NULL_OFFSET;
      else
         previousItem->nextItem = NULL_OFFSET;
      
      pHash->numberOfItems--;

      pHash->enumResize = isItTimeToResize( pHash );

      return LIST_OK;

   }

   return LIST_KEY_NOT_FOUND;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseHashEmpty( vdseHash*           pHash,
                    vdseSessionContext* pContext )
{
   ptrdiff_t* pArray, currentOffset, nextOffset;
   size_t i;
   vdseHashItem* pItem;
   
   VDS_PRE_CONDITION( pHash != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_INV_CONDITION( pHash->initialized == VDSE_HASH_SIGNATURE );
   
   SET_PTR( pArray, pHash->arrayOffset, ptrdiff_t );
   VDS_INV_CONDITION( pArray != NULL );

   for ( i = 0; i < g_arrayLengths[pHash->lengthIndex]; ++i )
   {
      currentOffset = pArray[i];
      
      while ( currentOffset != NULL_OFFSET )
      {
         pItem = GET_PTR( currentOffset, vdseHashItem );
         nextOffset = pItem->nextItem;
         
         vdseFree( (vdseMemObject*)pContext->pCurrentMemObject, 
                   (unsigned char*) pItem, 
                   calculateItemLength(pItem->keyLength,pItem->dataLength), 
                   pContext );
         
         /* Move to the next item in our bucket */
         currentOffset = nextOffset;
      }
      
      pArray[i] = NULL_OFFSET;
   }
   
   pHash->numberOfItems = 0;
   pHash->totalDataSizeInBytes = 0;
   pHash->enumResize = VDSE_HASH_TIME_TO_SHRINK;

   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseHashFini( vdseHash*           pHash,
                   vdseSessionContext* pContext )
{
   ptrdiff_t* pArray;

   VDS_PRE_CONDITION( pHash != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_INV_CONDITION( pHash->initialized == VDSE_HASH_SIGNATURE );
   
   vdseHashEmpty( pHash, pContext );

   /* EmptyList deallocates the rows but not the main array of */
   /* RowDescriptor* */
   if ( pHash->arrayOffset != NULL_OFFSET )
   {
      pArray = GET_PTR( pHash->arrayOffset, ptrdiff_t );
      vdseFree( (vdseMemObject*)pContext->pCurrentMemObject, 
                (unsigned char*) pArray,
                g_arrayLengths[pHash->lengthIndex]*sizeof(ptrdiff_t),
                pContext );
      pHash->arrayOffset = NULL_OFFSET;
   }
   pHash->initialized = 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum ListErrors 
vdseHashGet( vdseHash*            pHash,
             const unsigned char* pKey,
             size_t               keyLength,
             void **              ppData,
             size_t*              pDataLength,
             vdseSessionContext*  pContext,
             size_t*              pBucket )
{
   size_t bucket;
   bool   keyFound = false;
   ptrdiff_t* pArray;
   vdseHashItem* pItem, *dummy;
   
   VDS_PRE_CONDITION( pHash       != NULL );
   VDS_PRE_CONDITION( pKey        != NULL );
   VDS_PRE_CONDITION( ppData      != NULL );
   VDS_PRE_CONDITION( pDataLength != NULL );
   VDS_PRE_CONDITION( pContext    != NULL );
   VDS_PRE_CONDITION( keyLength > 0 );
   VDS_INV_CONDITION( pHash->initialized == VDSE_HASH_SIGNATURE );
   
   SET_PTR( pArray, pHash->arrayOffset, ptrdiff_t );
   VDS_INV_CONDITION( pArray != NULL );

   keyFound = findKey( pHash, pArray, pKey, keyLength, 
                       &pItem, &dummy, &bucket );
   if ( keyFound )
   {
      *ppData      = getData( pItem );
      *pDataLength = pItem->dataLength;
      if ( pBucket )
         *pBucket  = bucket;
      
      return LIST_OK;
   }
   return LIST_KEY_NOT_FOUND;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum ListErrors 
vdseHashGetFirst( vdseHash*  pHash,
                  size_t*    pBucket, 
                  ptrdiff_t* pFirstItemOffset )
{
   ptrdiff_t* pArray, currentOffset;
   bool SHOULD_NOT_REACHED_THIS = true;
   size_t i;
   
   VDS_PRE_CONDITION( pHash != NULL );
   VDS_PRE_CONDITION( pBucket != NULL );
   VDS_PRE_CONDITION( pFirstItemOffset != NULL );
   
   VDS_INV_CONDITION( pHash->initialized == VDSE_HASH_SIGNATURE );
   
   SET_PTR( pArray, pHash->arrayOffset, ptrdiff_t );
   VDS_INV_CONDITION( pArray != NULL );

   if ( pHash->numberOfItems == 0 )
      return LIST_EMPTY;
   
   /* 
    * Note: the first item has to be the first non-empty pArray[i],
    * this makes the search easier.
    */
   for ( i = 0; i < g_arrayLengths[pHash->lengthIndex]; ++i )
   {
      currentOffset = pArray[i];
      
      if (currentOffset != NULL_OFFSET )
      {
         *pBucket = i;
         *pFirstItemOffset = currentOffset;
         return LIST_OK;
      }
   }

   VDS_POST_CONDITION( SHOULD_NOT_REACHED_THIS == false );
   
   return LIST_EMPTY; /* Should never occur */
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum ListErrors 
vdseHashGetNext( vdseHash*  pHash,
                 size_t     previousBucket,
                 ptrdiff_t  previousOffset,
                 size_t*    pNextBucket, 
                 ptrdiff_t* pNextItemOffset )
{
   ptrdiff_t* pArray, currentOffset;
   size_t i;
   vdseHashItem* pItem;
   
   VDS_PRE_CONDITION( pHash != NULL );
   VDS_PRE_CONDITION( pNextBucket != NULL );
   VDS_PRE_CONDITION( pNextItemOffset != NULL );
   VDS_PRE_CONDITION( previousOffset != NULL_OFFSET );
   VDS_PRE_CONDITION( previousBucket < g_arrayLengths[pHash->lengthIndex]);
   VDS_INV_CONDITION( pHash->initialized == VDSE_HASH_SIGNATURE );
   
   SET_PTR( pArray, pHash->arrayOffset, ptrdiff_t );
   VDS_INV_CONDITION( pArray != NULL );

   pItem = GET_PTR( previousOffset, vdseHashItem );
   if ( pItem->nextItem != NULL_OFFSET )
   {
      /* We found the next one in the linked list. */
      *pNextBucket = previousBucket;
      *pNextItemOffset = pItem->nextItem;
      return LIST_OK;
   }
   
   /* 
    * Note: the next item has to be the first non-empty pArray[i] beyond
    * the current bucket (previousBucket). 
    */
   for ( i = previousBucket+1; i < g_arrayLengths[pHash->lengthIndex]; ++i )
   {
      currentOffset = pArray[i];
      
      if (currentOffset != NULL_OFFSET )
      {
         *pNextBucket = i;
         *pNextItemOffset = currentOffset;
         return LIST_OK;
      }      
   }
   
   return LIST_END_OF_LIST;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum ListErrors 
vdseHashInit( vdseHash*           pHash,
              size_t              reservedSize, 
              vdseSessionContext* pContext )
{
   enum ListErrors errCode = LIST_OK;
   size_t len, numBuckets;
   ptrdiff_t* ptr;
   unsigned int i;
   
   VDS_PRE_CONDITION( pHash != NULL );
   VDS_PRE_CONDITION( pContext != NULL );

   pHash->numberOfItems = 0;
   pHash->totalDataSizeInBytes = 0;
   pHash->enumResize = VDSE_HASH_NO_RESIZE;
   
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
   
   /* Which one of our lenghts is closer but larger than numBuckets? */
   pHash->lengthIndex = pHash->lengthIndexMinimum = 0;
   for ( i = 1; i < PRIME_NUMBER_ARRAY_LENGTH; ++i )
   {
      if ( g_arrayLengths[i] > numBuckets )
      {
         pHash->lengthIndex = i - 1;
         pHash->lengthIndexMinimum = i - 1;
         break;
      }
   }
   
   len = g_arrayLengths[pHash->lengthIndex] * sizeof(ptrdiff_t);
   
   ptr = (ptrdiff_t*) 
      vdseMalloc( (vdseMemObject*)pContext->pCurrentMemObject, 
                  len, 
                  pContext );
   if ( ptr == NULL )
      errCode = LIST_NO_MEMORY;
   else
   {
      for ( i = 0; i < g_arrayLengths[pHash->lengthIndex]; ++i)
         ptr[i] = NULL_OFFSET;
      
      pHash->arrayOffset = SET_OFFSET( ptr );
      pHash->initialized = VDSE_HASH_SIGNATURE;
   }
   
   return errCode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum ListErrors 
vdseHashInsert( vdseHash*            pHash,
                const unsigned char* pKey,
                size_t               keyLength,
                void*                pData,
                size_t               dataLength,
                ptrdiff_t*           pOffsetOfNewItem,
                vdseSessionContext*  pContext )
{
   ptrdiff_t* pArray;   
   size_t bucket = 0;
   bool   keyFound = false;
   vdseHashItem* pItem, *previousItem = NULL;

   VDS_PRE_CONDITION( pHash    != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pKey     != NULL );
   VDS_PRE_CONDITION( pData    != NULL );
   VDS_PRE_CONDITION( keyLength  > 0 );
   VDS_PRE_CONDITION( dataLength > 0 );
   VDS_PRE_CONDITION( pOffsetOfNewItem != NULL );

   VDS_INV_CONDITION( pHash->initialized == VDSE_HASH_SIGNATURE );
   
   SET_PTR( pArray, pHash->arrayOffset, ptrdiff_t );
   VDS_INV_CONDITION( pArray != NULL );

   keyFound = findKey( pHash, pArray, pKey, keyLength, 
                       &pItem, &previousItem, &bucket );

   if ( keyFound )
      return LIST_KEY_FOUND;

   /* The whole item is allocated in one step, header+data, to minimize */
   /* overheads of the memory allocator */
   pItem = (vdseHashItem*) 
      vdseMalloc( (vdseMemObject*)pContext->pCurrentMemObject, 
                  calculateItemLength( keyLength, dataLength ),
                  pContext );
   if ( pItem == NULL ) return LIST_NO_MEMORY;
   
   pItem->nextItem = NULL_OFFSET;

   /* keyLength must be set before calling getData() */   
   pItem->keyLength = keyLength;
   pItem->dataLength = dataLength;

   memcpy( pItem->key,     pKey, keyLength );
   memcpy( getData(pItem), pData, dataLength );
   

   pHash->totalDataSizeInBytes += dataLength;
   pHash->numberOfItems++;

   pHash->enumResize = isItTimeToResize( pHash );
   
   *pOffsetOfNewItem = SET_OFFSET(pItem );
   
   if ( previousItem == NULL )
      pArray[bucket] = *pOffsetOfNewItem;
   else
      previousItem->nextItem = *pOffsetOfNewItem;
   
   return LIST_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum ListErrors 
vdseHashResize( vdseHash*           pHash,
                vdseSessionContext* pContext )
{
   int newIndexLength;
   ptrdiff_t* ptr;
   size_t len, i;
   ptrdiff_t* pArray, currentOffset, nextOffset, newBucket, newOffset;
   vdseHashItem* pItem, *pNewItem;
  
   VDS_PRE_CONDITION( pHash != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_INV_CONDITION( pHash->initialized == VDSE_HASH_SIGNATURE );
   
   SET_PTR( pArray, pHash->arrayOffset, ptrdiff_t );
   VDS_INV_CONDITION( pArray != NULL );

   if ( pHash->enumResize == VDSE_HASH_NO_RESIZE )
      return LIST_OK;

   if ( pHash->enumResize == VDSE_HASH_TIME_TO_GROW )
      newIndexLength = pHash->lengthIndex + 1;
   else
      newIndexLength = pHash->lengthIndex - 1;     

   len = g_arrayLengths[newIndexLength] * sizeof(ptrdiff_t);
   
   ptr = (ptrdiff_t*) 
      vdseMalloc( (vdseMemObject*)pContext->pCurrentMemObject, 
                  len, 
                  pContext );
   if ( ptr == NULL )
      return LIST_NO_MEMORY;

   for ( i = 0; i < g_arrayLengths[newIndexLength]; ++i)
      ptr[i] = NULL_OFFSET;
      
   for ( i = 0; i < g_arrayLengths[pHash->lengthIndex]; ++i )
   {
      currentOffset = pArray[i];
      
      while ( currentOffset != NULL_OFFSET )
      {
         pItem = GET_PTR( currentOffset, vdseHashItem );
         nextOffset = pItem->nextItem;
         
         newBucket = hash_pjw( pItem->key, pItem->keyLength ) % 
                     g_arrayLengths[newIndexLength];
         if ( ptr[newBucket] == NULL_OFFSET )
            ptr[newBucket] = currentOffset;
         else
         {
            newOffset = ptr[newBucket];
            pNewItem = GET_PTR( newOffset, vdseHashItem );
            while ( pNewItem->nextItem != NULL_OFFSET )
            {
               newOffset = pNewItem->nextItem;
               pNewItem = GET_PTR( newOffset, vdseHashItem );
            }
            pNewItem->nextItem = currentOffset;
            pItem->nextItem = NULL_OFFSET;
         }
         
         /* Move to the next item in our bucket */
         currentOffset = nextOffset;
      }
   }
   
   len = g_arrayLengths[pHash->lengthIndex]*sizeof(ptrdiff_t);

   pHash->lengthIndex = newIndexLength;
   pHash->arrayOffset = SET_OFFSET( ptr );

   vdseFree( (vdseMemObject*)pContext->pCurrentMemObject,
             (unsigned char*)pArray,
             len, 
             pContext );
   
   return LIST_OK;   
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if 0

not sure this function is useful in a transaction environment
you can't really rollback here

enum ListErrors 
vdseHashUpdate( vdseHash*            pHash,
                const unsigned char* pKey,
                size_t               keyLength,
                void*                pData,
                size_t               dataLength,
                vdseSessionContext*  pContext )
{
   size_t bucket = 0;
   bool keyFound = false;
   ptrdiff_t* pArray;
   
   vdseHashItem* pItem, *previousItem = NULL;
   
   VDS_PRE_CONDITION( pHash != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_INV_CONDITION( pHash->initialized == VDSE_HASH_SIGNATURE );
   
   SET_PTR( pArray, pHash->arrayOffset, ptrdiff_t );
   VDS_INV_CONDITION( pArray != NULL );

   keyFound = findKey( pHash, pArray, pKey, keyLength, 
                       &pItem, &previousItem, &bucket );
   if ( keyFound )   
   {
      RowDescriptor* pOldRow = 
         GET_PTR( pArray->Get(bucket), 
                  RowDescriptor, 
                  (vdseMemObject*)pContext->pCurrentMemObject );

      /* Sadly, if ( dataLength == pOldRow->dataLength )
       * we cannot just copy the new data into the row - this
       * is not "crash-recovery safe"!!! */

      /* mem alloc first - the only thing that might fail */
      RowDescriptor* pRow = (RowDescriptor*) 
         vdseMalloc( (vdseMemObject*)pContext->pCurrentMemObject, 
                     sizeof(RowDescriptor) + keyLength + dataLength,
                     pContext );
      if ( pRow == NULL )
         return LIST_NO_MEMORY;

      /* initialize the new row */

      new (pRow) RowDescriptor;

      pRow->dataOffset = SET_OFFSET( (char*)pRow + sizeof(RowDescriptor),
                                     (vdseMemObject*)pContext->pCurrentMemObject );
      pRow->keyOffset  = SET_OFFSET( (char*)pRow + sizeof(RowDescriptor) + 
                                     dataLength,
                                     (vdseMemObject*)pContext->pCurrentMemObject );
   
      memcpy( (char*)pRow + sizeof(RowDescriptor) + dataLength,  pKey,  
              keyLength );
      memcpy( (char*)pRow + sizeof(RowDescriptor), pData, dataLength );
   
      pRow->keyLength = keyLength;
      pRow->dataLength = dataLength;
      
      /* Eliminate the old row */
      pHash->totalDataSizeInBytes -= pOldRow->dataLength;
      pOldRow->~RowDescriptor();
      vdseFree( (vdseMemObject*)pContext->pCurrentMemObject, pOldRow, pContext );

      pHash->totalDataSizeInBytes += dataLength;

      pArray->Set( bucket, SET_OFFSET( pRow, (vdseMemObject*)pContext->pCurrentMemObject ) );

      return LIST_OK;

   }
   return LIST_KEY_NOT_FOUND;
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if 0

   int ResetArray( vdseMemAlloc* pAlloc );

   bool SelfTest( vdseMemAlloc* pAlloc );
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum ListErrors HashList::GetFirst( size_t*         pNewBucket, 
                                    RowDescriptor** ppRow,
                                    vdseMemAlloc*   pAlloc )
{
   if ( pHash->numberOfItems == 0 )
      return LIST_EMPTY;

   ptrdiff_t* pArray;
   
   SET_PTR( pArray, pHash->arrayOffset, ptrdiff_t, pAlloc );
   VDS_ASSERT( pArray != NULL );
  
   for ( size_t i = 0; i < pHash->arrayLength; ++i )
   {
      if ( pArray->Get(i) != NULL_OFFSET )
      {
         *ppRow  = GET_PTR( pArray->Get(i), 
                            RowDescriptor, 
                            pAlloc );
         *pNewBucket = i;

         return LIST_OK;
      }
   }
   if ( pHash->numberOfItems > 0 ) 
      fprintf( stderr, " ERROR 1 - %d \n", pHash->numberOfItems );
   
   return LIST_EMPTY; /* Should never occur */
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum ListErrors HashList::GetNext( size_t  oldBucket,
                                   size_t* pNewBucket, 
                                   RowDescriptor** ppRow,
                                   vdseMemAlloc* pAlloc )
{
   ptrdiff_t* pArray;
   
   SET_PTR( pArray, pHash->arrayOffset, ptrdiff_t, pAlloc );
   VDS_ASSERT( pArray != NULL );

   for ( size_t i = oldBucket+1; i < pHash->arrayLength; ++i )
   {
      if ( pArray->Get(i) != NULL_OFFSET )
      {
         *ppRow  = GET_PTR( pArray->Get(i), 
                            RowDescriptor, 
                            pAlloc );
         *pNewBucket = i;

         return LIST_OK;
      }
   }
   return LIST_END_OF_LIST;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Rehash the whole array back into the current array */
int HashList::ResetArray( vdseMemAlloc* pAlloc )
{
   size_t i, bucket = 0;
   ptrdiff_t* pArray;
   
   SET_PTR( pArray, pHash->arrayOffset, ptrdiff_t, pAlloc );
   VDS_ASSERT( pArray != NULL );

   /* Create a temporary array on the heap to hold the pointers */
   ptrdiff_t* ptr = (ptrdiff_t*) 
      malloc( pHash->arrayLength * sizeof ( ptrdiff_t ) );
   if ( ptr == NULL )
      return -1;

   /* Copy to the temp array */
   for ( i = 0; i < pHash->arrayLength; ++i)
      ptr[i] = pArray->Get(i);

   /* Clear the existing array */
   for ( i = 0; i < pHash->arrayLength; ++i)
      pArray->Set( i, NULL_OFFSET );
   
   /* Repopulate the array using our hash function */
   for ( i = 0; i < pHash->arrayLength; ++i)
   {
      if ( ptr[i] != NULL_OFFSET )
      {
         RowDescriptor* pRow = GET_PTR( ptr[i], 
                                        RowDescriptor,
                                        pAlloc );
         char* pKey = GET_PTR( pRow->keyOffset, char, pAlloc );
         
         bucket =  hash_pjw( pKey, pRow->keyLength ) % pHash->arrayLength;
         
         while (1)
         {
            if ( pArray->Get(bucket) == NULL_OFFSET )
            {
               /* We found a valid spot */
               pArray->Set( bucket, ptr[i] );
               break;
            }
            ++bucket;
            
            if ( bucket == pHash->arrayLength )
               bucket = 0;
         }
      }
   }
   free( ptr );
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool HashList::SelfTest( vdseMemAlloc* pAlloc )
{
   size_t i, j, bucket, numRows = 0, sum = 0;
   ptrdiff_t* pArray;
   
   SET_PTR( pArray, pHash->arrayOffset, ptrdiff_t, pAlloc );
   VDS_ASSERT( pArray != NULL );

   for ( i = 0; i < pHash->arrayLength; ++i)
   {
      if ( pArray->Get(i) != NULL_OFFSET )
      {
         RowDescriptor* pRow = GET_PTR( pArray->Get(i), 
                                        RowDescriptor,
                                        pAlloc );
         char* pKey = GET_PTR( pRow->keyOffset, char, pAlloc );

         if ( pRow->keyLength > MAX_KEY_LENGTH )
            fprintf( stderr, "Wrong key length\n" );
         if ( pRow->dataLength != 8 )
            fprintf( stderr, "Wrong data length\n" );
      
         numRows++;
         sum += pRow->dataLength;
         
         /* Check the key */
         bucket =  hash_pjw( pKey, pRow->keyLength ) % pHash->arrayLength;
         
         if ( bucket == i ) continue;
         
         /* The row is at "i" but the key hashes to "bucket" - which means */
         /* that there was something at bucket already if all is well (and */
         /* at bucket+1, bucket+2, etc. if it applies) */
         /* Let's see... */

         j = bucket;
         while ( j != i )
         {
            if ( pArray->Get(j) == NULL_OFFSET )
            {
               fprintf( stderr, "Key at wrong place in hash list\n" );
               VDS_ASSERT( 0 );
               return false;
            }
            j++;
            if ( j == pHash->arrayLength )
               j = 0;
         }
      }
   }
   if ( numRows != pHash->numberOfItems )
      fprintf( stderr, "Invalid number of rows %d %d\n", numRows, pHash->numberOfItems );
   if ( sum != pHash->totalDataSizeInBytes )
      fprintf( stderr, "Invalid total sum %d %d \n", sum, pHash->totalDataSizeInBytes );

   pHash->numberOfItems = numRows;
   pHash->totalDataSizeInBytes = sum;
   
   return BaseObject::SelfTest();
}

#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
