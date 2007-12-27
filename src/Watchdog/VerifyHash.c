/*
 * Copyright (C) 2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

#include "Common/Common.h"
#include "Watchdog/VerifyCommon.h"
#include "Engine/Hash.h"
#include "Engine/hash_fnv.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if 0
typedef struct vdseHashItem
{
   vdseTxStatus  txStatus;
   
   /** Next item in this bucket */
   ptrdiff_t     nextItem;
   /** Next item with same key (for replace, etc.) */
   ptrdiff_t     nextSameKey;
   
   size_t        keyLength;
   ptrdiff_t     dataOffset;
   size_t        dataLength;
   unsigned char key[1];
   
} vdseHashItem;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct vdseHash
{
   /** offset of the memory object we need to use for allocating memory. */
   ptrdiff_t memObjOffset;
   
   /** Offset to an array of offsets to vdseHashItem objects */
   ptrdiff_t    arrayOffset; 
   
   /** Number of items stored in this hash map. */
   size_t       numberOfItems;
   
   /** Total amount of bytes of data stored in this hash */
   size_t       totalDataSizeInBytes;

   /** The index into the array of lengths (aka the number of buckets). */
   int lengthIndex;

   /** The mimimum shrinking factor that we can tolerate to accommodate
    *  the reservedSize argument of vdseHashInit() ) */
   int lengthIndexMinimum;

   /** Indicator of the current status of the array. */
   vdseHashResizeEnum enumResize;
   
   /** Set to VDSE_HASH_SIGNATURE at initialization. */
   unsigned int initialized;

} vdseHash;
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int
vdswVerifyHash( vdswVerifyStruct * pVerify,
                struct vdseHash  * pHash,
                ptrdiff_t          offset )
{
   ptrdiff_t * pArray;
   size_t i;
   ptrdiff_t currentOffset, nextOffset, bucket;
   vdseHashItem * pItem;
   size_t invalidBuckets = 0;
   size_t numberOfItems = 0;
   size_t totalDataSizeInBytes = 0;

   if ( pHash->initialized != VDSE_HASH_SIGNATURE ) {
      vdswEcho( pVerify, 
         "Hash::initialized is not VDSE_HASH_SIGNATURE - it might indicate a serious problem" );
      if ( pVerify->doRepair ) {
         pHash->initialized = VDSE_HASH_SIGNATURE;
         vdswEcho( pVerify, "Resetting Hash::initialized" );
      }
   }
   
   if ( pHash->memObjOffset != offset ) {
      vdswEcho( pVerify, 
         "Hash::memObjOffset is wrong - it might indicate a serious problem" );
      if ( pVerify->doRepair ) {
         pHash->memObjOffset = offset;
         vdswEcho( pVerify, "Resetting Hash::memObjOffset" );
      }
   }

   if ( ! vdswVerifyOffset( pVerify, pHash->arrayOffset ) ) {
      vdswEcho( pVerify, 
         "Hash::arrayOffset is invalid - aborting the hash verification" );
      return -1;
   }
   GET_PTR( pArray, pHash->arrayOffset, ptrdiff_t );

   if ( pHash->lengthIndex >= PRIME_NUMBER_ARRAY_LENGTH ) {
      vdswEcho( pVerify, 
         "Hash::lengthIndex is invalid - aborting the hash verification" );
      return -1;
   }
    /** The mimimum shrinking factor that we can tolerate to accommodate
    *  the reservedSize argument of vdseHashInit() ) */
   int lengthIndexMinimum;
  
   for ( i = 0; i < g_arrayLengths[pHash->lengthIndex]; ++i )
   {
      currentOffset = pArray[i];
      
      while ( currentOffset != NULL_OFFSET )
      {
         if ( ! vdswVerifyOffset( pVerify, currentOffset ) ) {
            vdswEcho( pVerify, 
               "Hash item offset is invalid - jumping to next offset" );
            if ( pVerify->doRepair )
               pArray[i] = NULL_OFFSET;
            continue;
         }

         GET_PTR( pItem, currentOffset, vdseHashItem );
         nextOffset = pItem->nextItem;
         
         bucket = fnv_buf( (void *)pItem->key, pItem->keyLength, FNV1_INIT) %
                     g_arrayLengths[pHash->lengthIndex];
         if ( bucket != i ) {
            vdswEcho( pVerify, "Hash item - invalid bucket" );
            invalidBuckets++;
         }
         
         // test the hash item
         
         if ( pItem->nextSameItem != NULL_OFFSET ) {
            if ( ! vdswVerifyOffset( pVerify, pItem->nextSameItem ) ) {
               vdswEcho( pVerify, "Hash item next-same offset is invalid" );
               if ( pVerify->doRepair )
                  pItem->nextSameItem = NULL_OFFSET;
            }
         }
         if ( pItem->dataOffset != NULL_OFFSET ) {
            if ( ! vdswVerifyOffset( pVerify, pItem->dataOffset ) ) {
               vdswEcho( pVerify, "HashItem::dataOffset is invalid" );
               if ( pVerify->doRepair )
//                  pItem->nextSameItem = NULL_OFFSET;
            }
         }

#if 0
         typedef struct vdseHashItem
{
   vdseTxStatus  txStatus;
   
   /** Next item in this bucket */
   ptrdiff_t     nextItem;
   /** Next item with same key (for replace, etc.) */
   ptrdiff_t     nextSameKey;
   
   size_t        keyLength;
   ptrdiff_t     dataOffset;
   size_t        dataLength;
   unsigned char key[1];
   
} vdseHashItem;
#endif

         numberOfItems++;
         totalDataSizeInBytes += pItem->dataLength;

         /* Move to the next item in our bucket */
         currentOffset = nextOffset;
      }
   }

   if ( pVerify->doRepair ) {
      for ( i = 0; i < g_arrayLengths[pHash->lengthIndex]; ++i ) {
         currentOffset = pArray[i];
      
         while ( currentOffset != NULL_OFFSET ) {

            GET_PTR( pItem, currentOffset, vdseHashItem );
            nextOffset = pItem->nextItem;
         
  loop on all other members in bucket to find record with same key
            // test the hash item
         
            if ( pItem->nextSameItem != NULL_OFFSET ) {
            }

            /* Move to the next item in our bucket */
            currentOffset = nextOffset;
         }
      }
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
