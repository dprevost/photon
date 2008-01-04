/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

/*
 * This function only check the hash itself - it makes sure that the hash
 * is self consistent (that the keys hashes to the proper bucket and that 
 * the links to the next item in the bucket are ok). 
 *
 * Since both folders and hash maps are using the hash, tests for the hash 
 * items are done in two different functions (hash items for folders may 
 * point to blocks of memory that must be free to the main allocator so they
 * must be handle separately).
 */
 
int
vdswVerifyHash( vdswVerifyStruct * pVerify,
                struct vdseHash  * pHash,
                ptrdiff_t          offset )
{
   ptrdiff_t * pArray;
   size_t i;
   ptrdiff_t previousOffset, currentOffset, nextOffset, bucket;
   vdseHashItem * pItem, * previousItem;
   size_t invalidBuckets = 0;
   size_t numberOfItems = 0;
   size_t totalDataSizeInBytes = 0;
   bool removeItem;
   
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

   if ( pHash->lengthIndex >= VDSE_PRIME_NUMBER_ARRAY_LENGTH ) {
      vdswEcho( pVerify, 
         "Hash::lengthIndex is invalid - aborting the hash verification" );
      return -1;
   }
  
   for ( i = 0; i < g_vdseArrayLengths[pHash->lengthIndex]; ++i ) {

      previousOffset = NULL_OFFSET;
      currentOffset = pArray[i];
      
      while ( currentOffset != NULL_OFFSET ) {
      
         removeItem = false;
         
         if ( ! vdswVerifyOffset( pVerify, currentOffset ) ) {
            vdswEcho( pVerify, 
               "Hash item offset is invalid - jumping to next offset" );
            if ( pVerify->doRepair ) {
               if ( previousOffset == NULL_OFFSET )
                  pArray[i] = NULL_OFFSET;
               else {
                  GET_PTR( previousItem, previousOffset, vdseHashItem );
                  previousItem->nextItem = NULL_OFFSET;
               }
            }
            break; /* of the while loop */
         }

         GET_PTR( pItem, currentOffset, vdseHashItem );
         nextOffset = pItem->nextItem;
         
         if ( pItem->keyLength == 0 ) {
            vdswEcho( pVerify, "HashItem::keyLength is invalid" );
            removeItem = true;
         }
         else {
            bucket = fnv_buf( (void *)pItem->key, pItem->keyLength, FNV1_INIT) %
                        g_vdseArrayLengths[pHash->lengthIndex];
            if ( bucket != i ) {
               vdswEcho( pVerify, "Hash item - invalid bucket" );
               invalidBuckets++;
            }
         }
         // test the hash item
         
         if ( pItem->nextSameKey != NULL_OFFSET ) {
            if ( ! vdswVerifyOffset( pVerify, pItem->nextSameKey ) ) {
               vdswEcho( pVerify, "HashItem::nextSameKey is invalid" );
               if ( pVerify->doRepair )
                  pItem->nextSameKey = NULL_OFFSET;
            }
         }
         if ( pItem->dataOffset == NULL_OFFSET ) {
            vdswEcho( pVerify, "HashItem::dataOffset is NULL" );
            removeItem = true;
         }
         else {
            if ( ! vdswVerifyOffset( pVerify, pItem->dataOffset ) ) {
               vdswEcho( pVerify, "HashItem::dataOffset is invalid" );
               removeItem = true;
            }
            else {
               if ( ! vdswVerifyOffset( pVerify, 
                                 pItem->dataOffset + pItem->dataLength ) ) {
                  vdswEcho( pVerify, "HashItem::dataOffset is invalid" );
                  removeItem = true;
               }
            }
         }

         if ( ! removeItem ) {
            numberOfItems++;
            totalDataSizeInBytes += pItem->dataLength;
         }
         
         if ( pVerify->doRepair && removeItem ) {
            if ( previousOffset == NULL_OFFSET )
               pArray[i] = nextOffset;
            else {
               GET_PTR(previousItem, previousOffset, vdseHashItem );
               previousItem->nextItem = nextOffset;
            }
            
         }

         /*
          * Obviously, if the current item was remove, previousOffset must
          * be left unchanged 
          */
         if ( ! removeItem ) 
            previousOffset = currentOffset; 
         
         /* Move to the next item in our bucket. */
         currentOffset = nextOffset;
      }
   }

   if ( pHash->numberOfItems != numberOfItems ) {
      vdswEcho( pVerify, 
         "Hash::numberOfItems is invalid" );
      if ( pVerify->doRepair ) {
         pHash->numberOfItems = numberOfItems;
         vdswEcho( pVerify, "Resetting Hash::numberOfItems" );
      }
   }
   if ( pHash->totalDataSizeInBytes != totalDataSizeInBytes ) {
      vdswEcho( pVerify, 
         "Hash::totalDataSizeInBytes is invalid" );
      if ( pVerify->doRepair ) {
         pHash->totalDataSizeInBytes = totalDataSizeInBytes;
         vdswEcho( pVerify, "Resetting Hash::totalDataSizeInBytes" );
      }
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int
vdswVerifyHashContent( vdswVerifyStruct * pVerify,
                       struct vdseHash  * pHash )
{
   ptrdiff_t * pArray;
   size_t i;
   ptrdiff_t previousOffset, currentOffset, nextOffset, bucket;
   vdseHashItem * pItem, * previousItem;
   size_t invalidBuckets = 0;
   size_t numberOfItems = 0;
   size_t totalDataSizeInBytes = 0;
   bool removeItem;
   
   GET_PTR( pArray, pHash->arrayOffset, ptrdiff_t );

   for ( i = 0; i < g_vdseArrayLengths[pHash->lengthIndex]; ++i ) {

      previousOffset = NULL_OFFSET;
      currentOffset = pArray[i];
      
      while ( currentOffset != NULL_OFFSET ) {
      
         removeItem = false;
         
         if ( ! vdswVerifyOffset( pVerify, currentOffset ) ) {
            vdswEcho( pVerify, 
               "Hash item offset is invalid - jumping to next offset" );
            if ( pVerify->doRepair ) {
               if ( previousOffset == NULL_OFFSET )
                  pArray[i] = NULL_OFFSET;
               else {
                  GET_PTR( previousItem, previousOffset, vdseHashItem );
                  previousItem->nextItem = NULL_OFFSET;
               }
            }
            break; /* of the while loop */
         }

         GET_PTR( pItem, currentOffset, vdseHashItem );
         nextOffset = pItem->nextItem;
         
         if ( pItem->keyLength == 0 ) {
            vdswEcho( pVerify, "HashItem::keyLength is invalid" );
            removeItem = true;
         }
         else {
            bucket = fnv_buf( (void *)pItem->key, pItem->keyLength, FNV1_INIT) %
                        g_vdseArrayLengths[pHash->lengthIndex];
            if ( bucket != i ) {
               vdswEcho( pVerify, "Hash item - invalid bucket" );
               invalidBuckets++;
            }
         }
         // test the hash item
         
         if ( pItem->nextSameKey != NULL_OFFSET ) {
            if ( ! vdswVerifyOffset( pVerify, pItem->nextSameKey ) ) {
               vdswEcho( pVerify, "HashItem::nextSameKey is invalid" );
               if ( pVerify->doRepair )
                  pItem->nextSameKey = NULL_OFFSET;
            }
         }
         if ( pItem->dataOffset == NULL_OFFSET ) {
            vdswEcho( pVerify, "HashItem::dataOffset is NULL" );
            removeItem = true;
         }
         else {
            if ( ! vdswVerifyOffset( pVerify, pItem->dataOffset ) ) {
               vdswEcho( pVerify, "HashItem::dataOffset is invalid" );
               removeItem = true;
            }
            else {
               if ( ! vdswVerifyOffset( pVerify, 
                                 pItem->dataOffset + pItem->dataLength ) ) {
                  vdswEcho( pVerify, "HashItem::dataOffset is invalid" );
                  removeItem = true;
               }
            }
         }

         if ( ! removeItem ) {
            numberOfItems++;
            totalDataSizeInBytes += pItem->dataLength;
         }
         
         if ( pVerify->doRepair && removeItem ) {
            if ( previousOffset == NULL_OFFSET )
               pArray[i] = nextOffset;
            else {
               GET_PTR(previousItem, previousOffset, vdseHashItem );
               previousItem->nextItem = nextOffset;
            }
            
         }

         /*
          * Obviously, if the current item was remove, previousOffset must
          * be left unchanged 
          */
         if ( ! removeItem ) 
            previousOffset = currentOffset; 
         
         /* Move to the next item in our bucket. */
         currentOffset = nextOffset;
      }
   }

   if ( pHash->numberOfItems != numberOfItems ) {
      vdswEcho( pVerify, 
         "Hash::numberOfItems is invalid" );
      if ( pVerify->doRepair ) {
         pHash->numberOfItems = numberOfItems;
         vdswEcho( pVerify, "Resetting Hash::numberOfItems" );
      }
   }
   if ( pHash->totalDataSizeInBytes != totalDataSizeInBytes ) {
      vdswEcho( pVerify, 
         "Hash::totalDataSizeInBytes is invalid" );
      if ( pVerify->doRepair ) {
         pHash->totalDataSizeInBytes = totalDataSizeInBytes;
         vdswEcho( pVerify, "Resetting Hash::totalDataSizeInBytes" );
      }
   }

#if 0
\todo:   loop on all other members in a bucket to find record with same key
         and fix the chain of same-key records if needed.
         
   if ( pVerify->doRepair ) {
      for ( i = 0; i < g_vdseArrayLengths[pHash->lengthIndex]; ++i ) {
         currentOffset = pArray[i];
      
         while ( currentOffset != NULL_OFFSET ) {

            GET_PTR( pItem, currentOffset, vdseHashItem );
            nextOffset = pItem->nextItem;
         
            // test the hash item
         
            if ( pItem->nextSameKey != NULL_OFFSET ) {
            }

            /* Move to the next item in our bucket */
            currentOffset = nextOffset;
         }
      }
   }
#endif

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

