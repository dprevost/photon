/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@photonsoftware.org>
 *
 * This file is part of Photon (photonsoftware.org).
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
#include "Quasar/VerifyCommon.h"
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
 
enum psoqRecoverError
psoqVerifyHash( psoqVerifyStruct * pVerify,
                struct psonHash  * pHash,
                ptrdiff_t          offset )
{
   ptrdiff_t * pArray;
   size_t i, bucket;
   ptrdiff_t previousOffset, currentOffset, nextOffset, tmpOffset;
   psonHashItem * pItem, * previousItem;
   size_t invalidBuckets = 0;
   size_t numberOfItems = 0;
   size_t totalDataSizeInBytes = 0;
   bool removeItem;
   enum psoqRecoverError rc = PSOQ_REC_OK;
   
   if ( pHash->initialized != PSON_HASH_SIGNATURE ) {
      rc = PSOQ_REC_CHANGES;
      psoqEcho( pVerify, 
         "Hash::initialized is not PSON_HASH_SIGNATURE - it might indicate a serious problem" );
      if ( pVerify->doRepair ) {
         pHash->initialized = PSON_HASH_SIGNATURE;
         psoqEcho( pVerify, "Resetting Hash::initialized" );
      }
   }
   
   if ( pHash->memObjOffset != offset ) {
      rc = PSOQ_REC_CHANGES;
      psoqEcho( pVerify, 
         "Hash::memObjOffset is wrong - it might indicate a serious problem" );
      if ( pVerify->doRepair ) {
         pHash->memObjOffset = offset;
         psoqEcho( pVerify, "Resetting Hash::memObjOffset" );
      }
   }

   if ( ! psoqVerifyOffset( pVerify, pHash->arrayOffset ) ) {
      psoqEcho( pVerify, 
         "Hash::arrayOffset is invalid - aborting the hash verification" );
      return PSOQ_REC_UNRECOVERABLE_ERROR;
   }
   GET_PTR( pArray, pHash->arrayOffset, ptrdiff_t );

   if ( pHash->lengthIndex >= PSON_PRIME_NUMBER_ARRAY_LENGTH ) {
      psoqEcho( pVerify, 
         "Hash::lengthIndex is invalid - aborting the hash verification" );
      return PSOQ_REC_UNRECOVERABLE_ERROR;
   }
  
   for ( i = 0; i < g_psonArrayLengths[pHash->lengthIndex]; ++i ) {
      previousOffset = PSON_NULL_OFFSET;
      currentOffset = pArray[i];
      
      while ( currentOffset != PSON_NULL_OFFSET ) {
         removeItem = false;
         
         if ( ! psoqVerifyOffset( pVerify, currentOffset ) ) {
            rc = PSOQ_REC_CHANGES;
            psoqEcho( pVerify, 
               "Hash item offset is invalid - jumping to next offset" );
            if ( pVerify->doRepair ) {
               if ( previousOffset == PSON_NULL_OFFSET ) {
                  pArray[i] = PSON_NULL_OFFSET;
               }
               else {
                  GET_PTR( previousItem, previousOffset, psonHashItem );
                  previousItem->nextItem = PSON_NULL_OFFSET;
               }
            }
            break; /* of the while loop */
         }

         GET_PTR( pItem, currentOffset, psonHashItem );
         nextOffset = pItem->nextItem;
         
         if ( pItem->keyLength == 0 ) {
            rc = PSOQ_REC_CHANGES;
            psoqEcho( pVerify, "HashItem::keyLength is invalid" );
            removeItem = true;
         }
         else {
            /* test the hash item itself */
            if ( pItem->nextSameKey != PSON_NULL_OFFSET ) {
               if ( ! psoqVerifyOffset( pVerify, pItem->nextSameKey ) ) {
                  rc = PSOQ_REC_CHANGES;
                  psoqEcho( pVerify, "HashItem::nextSameKey is invalid" );
                  if ( pVerify->doRepair ) {
                     pItem->nextSameKey = PSON_NULL_OFFSET;
                  }
               }
            }
            if ( pItem->dataOffset == PSON_NULL_OFFSET ) {
               rc = PSOQ_REC_CHANGES;
               psoqEcho( pVerify, "HashItem::dataOffset is NULL" );
               removeItem = true;
            }
            else {
               if ( ! psoqVerifyOffset( pVerify, pItem->dataOffset ) ) {
                  rc = PSOQ_REC_CHANGES;
                  psoqEcho( pVerify, "HashItem::dataOffset is invalid" );
                  removeItem = true;
               }
               else {
                  if ( ! psoqVerifyOffset( 
                        pVerify, pItem->dataOffset + pItem->dataLength ) ) {
                     rc = PSOQ_REC_CHANGES;
                     psoqEcho( pVerify, "HashItem::dataOffset is invalid" );
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
            rc = PSOQ_REC_CHANGES;
            psoqEcho( pVerify, "HashItem is removed" );
            if ( previousOffset == PSON_NULL_OFFSET ) {
               pArray[i] = nextOffset;
            }
            else {
               GET_PTR(previousItem, previousOffset, psonHashItem );
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
      rc = PSOQ_REC_CHANGES;
      psoqEcho( pVerify, "Hash::numberOfItems is invalid" );
      if ( pVerify->doRepair ) {
         pHash->numberOfItems = numberOfItems;
         psoqEcho( pVerify, "Resetting Hash::numberOfItems" );
      }
   }
   if ( pHash->totalDataSizeInBytes != totalDataSizeInBytes ) {
      rc = PSOQ_REC_CHANGES;
      psoqEcho( pVerify, "Hash::totalDataSizeInBytes is invalid" );
      if ( pVerify->doRepair ) {
         pHash->totalDataSizeInBytes = totalDataSizeInBytes;
         psoqEcho( pVerify, "Resetting Hash::totalDataSizeInBytes" );
      }
   }

   /*
    * Next iteration - are the keys in the right bucket? 
    * Question: can this really happen?
    */
   for ( i = 0; i < g_psonArrayLengths[pHash->lengthIndex]; ++i ) {
      currentOffset = pArray[i];
      
      while ( currentOffset != PSON_NULL_OFFSET ) {
         GET_PTR( pItem, currentOffset, psonHashItem );
         nextOffset = pItem->nextItem;
         
         bucket = fnv_buf( (void *)pItem->key, pItem->keyLength, FNV1_INIT) %
                     g_psonArrayLengths[pHash->lengthIndex];
         if ( bucket != i ) {
           rc = PSOQ_REC_CHANGES;
           psoqEcho( pVerify, "Hash item - invalid bucket" );
            invalidBuckets++;
            if ( pVerify->doRepair ) {
               if ( pArray[bucket] == PSON_NULL_OFFSET ) {
                  pArray[bucket] = currentOffset;
               }
               else {
                  tmpOffset = pArray[bucket];
                  do {
                     GET_PTR( pItem, tmpOffset, psonHashItem );
                     tmpOffset = pItem->nextItem;
                  } while ( tmpOffset != PSON_NULL_OFFSET );
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
   for ( i = 0; i < g_psonArrayLengths[pHash->lengthIndex]; ++i ) {
      currentOffset = pArray[i];
      
      while ( currentOffset != PSON_NULL_OFFSET ) {
         GET_PTR( pItem, currentOffset, psonHashItem );
         nextOffset = pItem->nextItem;
         
         bucket = fnv_buf( (void *)pItem->key, pItem->keyLength, FNV1_INIT) %
                     g_psonArrayLengths[pHash->lengthIndex];
         if ( bucket != i ) {
            psoqEcho( pVerify, "Hash item - invalid bucket" );
            invalidBuckets++;
            if ( pVerify->doRepair ) {
               if ( pArray[bucket] == PSON_NULL_OFFSET ) {
                  pArray[bucket] = currentOffset;
               }
               else {
                  tmpOffset = pArray[bucket];
                  do {
                     GET_PTR( pItem, tmpOffset, psonHashItem );
                     tmpOffset = pItem->nextItem;
                  } while ( tmpOffset != PSON_NULL_OFFSET );
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

