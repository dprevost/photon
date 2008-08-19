/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of photon (photonsoftware.org).
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
#include "Nucleus/Hash.h"
#include "Nucleus/hash_fnv.h"

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
 *
 */
 
enum vdswRecoverError
vdswVerifyHash( vdswVerifyStruct * pVerify,
                struct psnHash  * pHash,
                ptrdiff_t          offset )
{
   ptrdiff_t * pArray;
   size_t i, bucket;
   ptrdiff_t previousOffset, currentOffset, nextOffset, tmpOffset;
   psnHashItem * pItem, * previousItem;
   size_t invalidBuckets = 0;
   size_t numberOfItems = 0;
   size_t totalDataSizeInBytes = 0;
   bool removeItem;
   enum vdswRecoverError rc = VDSWR_OK;
   
   if ( pHash->initialized != PSN_HASH_SIGNATURE ) {
      rc = VDSWR_CHANGES;
      vdswEcho( pVerify, 
         "Hash::initialized is not PSN_HASH_SIGNATURE - it might indicate a serious problem" );
      if ( pVerify->doRepair ) {
         pHash->initialized = PSN_HASH_SIGNATURE;
         vdswEcho( pVerify, "Resetting Hash::initialized" );
      }
   }
   
   if ( pHash->memObjOffset != offset ) {
      rc = VDSWR_CHANGES;
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
      return VDSWR_UNRECOVERABLE_ERROR;
   }
   GET_PTR( pArray, pHash->arrayOffset, ptrdiff_t );

   if ( pHash->lengthIndex >= PSN_PRIME_NUMBER_ARRAY_LENGTH ) {
      vdswEcho( pVerify, 
         "Hash::lengthIndex is invalid - aborting the hash verification" );
      return VDSWR_UNRECOVERABLE_ERROR;
   }
  
   for ( i = 0; i < g_psnArrayLengths[pHash->lengthIndex]; ++i ) {
      previousOffset = PSN_NULL_OFFSET;
      currentOffset = pArray[i];
      
      while ( currentOffset != PSN_NULL_OFFSET ) {
         removeItem = false;
         
         if ( ! vdswVerifyOffset( pVerify, currentOffset ) ) {
            rc = VDSWR_CHANGES;
            vdswEcho( pVerify, 
               "Hash item offset is invalid - jumping to next offset" );
            if ( pVerify->doRepair ) {
               if ( previousOffset == PSN_NULL_OFFSET ) {
                  pArray[i] = PSN_NULL_OFFSET;
               }
               else {
                  GET_PTR( previousItem, previousOffset, psnHashItem );
                  previousItem->nextItem = PSN_NULL_OFFSET;
               }
            }
            break; /* of the while loop */
         }

         GET_PTR( pItem, currentOffset, psnHashItem );
         nextOffset = pItem->nextItem;
         
         if ( pItem->keyLength == 0 ) {
            rc = VDSWR_CHANGES;
            vdswEcho( pVerify, "HashItem::keyLength is invalid" );
            removeItem = true;
         }
         else {
            /* test the hash item itself */
            if ( pItem->nextSameKey != PSN_NULL_OFFSET ) {
               if ( ! vdswVerifyOffset( pVerify, pItem->nextSameKey ) ) {
                  rc = VDSWR_CHANGES;
                  vdswEcho( pVerify, "HashItem::nextSameKey is invalid" );
                  if ( pVerify->doRepair ) {
                     pItem->nextSameKey = PSN_NULL_OFFSET;
                  }
               }
            }
            if ( pItem->dataOffset == PSN_NULL_OFFSET ) {
               rc = VDSWR_CHANGES;
               vdswEcho( pVerify, "HashItem::dataOffset is NULL" );
               removeItem = true;
            }
            else {
               if ( ! vdswVerifyOffset( pVerify, pItem->dataOffset ) ) {
                  rc = VDSWR_CHANGES;
                  vdswEcho( pVerify, "HashItem::dataOffset is invalid" );
                  removeItem = true;
               }
               else {
                  if ( ! vdswVerifyOffset( 
                        pVerify, pItem->dataOffset + pItem->dataLength ) ) {
                     rc = VDSWR_CHANGES;
                     vdswEcho( pVerify, "HashItem::dataOffset is invalid" );
                     removeItem = true;
                  }
               }
            }
         }
         
         if ( ! removeItem ) {
            numberOfItems++;
            totalDataSizeInBytes += pItem->dataLength;
         }
         
         if ( pVerify->doRepair && removeItem ) {
            rc = VDSWR_CHANGES;
            vdswEcho( pVerify, "HashItem is removed" );
            if ( previousOffset == PSN_NULL_OFFSET ) {
               pArray[i] = nextOffset;
            }
            else {
               GET_PTR(previousItem, previousOffset, psnHashItem );
               previousItem->nextItem = nextOffset;
            }
            
         }

         /*
          * Obviously, if the current item was remove, previousOffset must
          * be left unchanged 
          */
         if ( ! removeItem ) previousOffset = currentOffset; 
         
         /* Move to the next item in our bucket. */
         currentOffset = nextOffset;
      }
   }

   if ( pHash->numberOfItems != numberOfItems ) {
      rc = VDSWR_CHANGES;
      vdswEcho( pVerify, "Hash::numberOfItems is invalid" );
      if ( pVerify->doRepair ) {
         pHash->numberOfItems = numberOfItems;
         vdswEcho( pVerify, "Resetting Hash::numberOfItems" );
      }
   }
   if ( pHash->totalDataSizeInBytes != totalDataSizeInBytes ) {
      rc = VDSWR_CHANGES;
      vdswEcho( pVerify, "Hash::totalDataSizeInBytes is invalid" );
      if ( pVerify->doRepair ) {
         pHash->totalDataSizeInBytes = totalDataSizeInBytes;
         vdswEcho( pVerify, "Resetting Hash::totalDataSizeInBytes" );
      }
   }

   /*
    * Next iteration - are the keys in the right bucket? 
    * Question: can this really happen?
    */
   for ( i = 0; i < g_psnArrayLengths[pHash->lengthIndex]; ++i ) {
      currentOffset = pArray[i];
      
      while ( currentOffset != PSN_NULL_OFFSET ) {
         GET_PTR( pItem, currentOffset, psnHashItem );
         nextOffset = pItem->nextItem;
         
         bucket = fnv_buf( (void *)pItem->key, pItem->keyLength, FNV1_INIT) %
                     g_psnArrayLengths[pHash->lengthIndex];
         if ( bucket != i ) {
           rc = VDSWR_CHANGES;
           vdswEcho( pVerify, "Hash item - invalid bucket" );
            invalidBuckets++;
            if ( pVerify->doRepair ) {
               if ( pArray[bucket] == PSN_NULL_OFFSET ) {
                  pArray[bucket] = currentOffset;
               }
               else {
                  tmpOffset = pArray[bucket];
                  do {
                     GET_PTR( pItem, tmpOffset, psnHashItem );
                     tmpOffset = pItem->nextItem;
                  } while ( tmpOffset != PSN_NULL_OFFSET );
                  pItem->nextItem = currentOffset;
               }
            }
         }

         /* Move to the next item in our bucket. */
         currentOffset = nextOffset;
      }
   }

#if 0
   /*
    * Next iteration - are identical keys properly arranged?
    */
   for ( i = 0; i < g_psnArrayLengths[pHash->lengthIndex]; ++i ) {
      currentOffset = pArray[i];
      
      while ( currentOffset != PSN_NULL_OFFSET ) {
         GET_PTR( pItem, currentOffset, psnHashItem );
         nextOffset = pItem->nextItem;
         
         bucket = fnv_buf( (void *)pItem->key, pItem->keyLength, FNV1_INIT) %
                     g_psnArrayLengths[pHash->lengthIndex];
         if ( bucket != i ) {
            vdswEcho( pVerify, "Hash item - invalid bucket" );
            invalidBuckets++;
            if ( pVerify->doRepair ) {
               if ( pArray[bucket] == PSN_NULL_OFFSET ) {
                  pArray[bucket] = currentOffset;
               }
               else {
                  tmpOffset = pArray[bucket];
                  do {
                     GET_PTR( pItem, tmpOffset, psnHashItem );
                     tmpOffset = pItem->nextItem;
                  } while ( tmpOffset != PSN_NULL_OFFSET );
                  pItem->nextItem = currentOffset;
               }
            }
         }

         /* Move to the next item in our bucket. */
         currentOffset = nextOffset;
      }
   }
#endif

   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

