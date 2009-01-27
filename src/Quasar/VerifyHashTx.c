/*
 * Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
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
#include "Quasar/VerifyCommon.h"
#include "Nucleus/HashTx.h"
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
 
enum qsrRecoverError
qsrVerifyHashTx( qsrVerifyStruct  * pVerify,
                  struct psonHashTx * pHash,
                  ptrdiff_t           offset )
{
   ptrdiff_t * pArray;
   size_t i, bucket;
   ptrdiff_t previousOffset, currentOffset, nextOffset, tmpOffset;
   psonHashTxItem * pItem, * previousItem;
   size_t invalidBuckets = 0;
   size_t numberOfItems = 0;
   size_t totalDataSizeInBytes = 0;
   bool removeItem;
   enum qsrRecoverError rc = QSR_REC_OK;
   
   if ( pHash->initialized != PSON_HASH_TX_SIGNATURE ) {
      rc = QSR_REC_CHANGES;
      qsrEcho( pVerify, 
         "HashTx::initialized is not PSON_HASH_TX_SIGNATURE - it might indicate a serious problem" );
      if ( pVerify->doRepair ) {
         pHash->initialized = PSON_HASH_TX_SIGNATURE;
         qsrEcho( pVerify, "Resetting HashTx::initialized" );
      }
   }
   
   if ( pHash->memObjOffset != offset ) {
      rc = QSR_REC_CHANGES;
      qsrEcho( pVerify, 
         "HashTx::memObjOffset is wrong - it might indicate a serious problem" );
      if ( pVerify->doRepair ) {
         pHash->memObjOffset = offset;
         qsrEcho( pVerify, "Resetting HashTx::memObjOffset" );
      }
   }

   if ( ! qsrVerifyOffset( pVerify, pHash->arrayOffset ) ) {
      qsrEcho( pVerify, 
         "HashTx::arrayOffset is invalid - aborting the hash verification" );
      return QSR_REC_UNRECOVERABLE_ERROR;
   }
   GET_PTR( pArray, pHash->arrayOffset, ptrdiff_t );

   if ( pHash->lengthIndex >= PSON_PRIME_NUMBER_ARRAY_LENGTH ) {
      qsrEcho( pVerify, 
         "HashTx::lengthIndex is invalid - aborting the hash verification" );
      return QSR_REC_UNRECOVERABLE_ERROR;
   }
  
   for ( i = 0; i < g_psonArrayLengths[pHash->lengthIndex]; ++i ) {
      previousOffset = PSON_NULL_OFFSET;
      currentOffset = pArray[i];
      
      while ( currentOffset != PSON_NULL_OFFSET ) {
         removeItem = false;
         
         if ( ! qsrVerifyOffset( pVerify, currentOffset ) ) {
            rc = QSR_REC_CHANGES;
            qsrEcho( pVerify, 
               "Hash item offset is invalid - jumping to next offset" );
            if ( pVerify->doRepair ) {
               if ( previousOffset == PSON_NULL_OFFSET ) {
                  pArray[i] = PSON_NULL_OFFSET;
               }
               else {
                  GET_PTR( previousItem, previousOffset, psonHashTxItem );
                  previousItem->nextItem = PSON_NULL_OFFSET;
               }
            }
            break; /* of the while loop */
         }

         GET_PTR( pItem, currentOffset, psonHashTxItem );
         nextOffset = pItem->nextItem;
         
         if ( pItem->keyLength == 0 ) {
            rc = QSR_REC_CHANGES;
            qsrEcho( pVerify, "HashItem::keyLength is invalid" );
            removeItem = true;
         }
         else {
            /* test the hash item itself */
            if ( pItem->nextSameKey != PSON_NULL_OFFSET ) {
               if ( ! qsrVerifyOffset( pVerify, pItem->nextSameKey ) ) {
                  rc = QSR_REC_CHANGES;
                  qsrEcho( pVerify, "HashItem::nextSameKey is invalid" );
                  if ( pVerify->doRepair ) {
                     pItem->nextSameKey = PSON_NULL_OFFSET;
                  }
               }
            }
            if ( pItem->dataOffset == PSON_NULL_OFFSET ) {
               rc = QSR_REC_CHANGES;
               qsrEcho( pVerify, "HashItem::dataOffset is NULL" );
               removeItem = true;
            }
            else {
               if ( ! qsrVerifyOffset( pVerify, pItem->dataOffset ) ) {
                  rc = QSR_REC_CHANGES;
                  qsrEcho( pVerify, "HashItem::dataOffset is invalid" );
                  removeItem = true;
               }
               else {
                  if ( ! qsrVerifyOffset( 
                        pVerify, pItem->dataOffset + pItem->dataLength ) ) {
                     rc = QSR_REC_CHANGES;
                     qsrEcho( pVerify, "HashItem::dataOffset is invalid" );
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
            rc = QSR_REC_CHANGES;
            qsrEcho( pVerify, "HashItem is removed" );
            if ( previousOffset == PSON_NULL_OFFSET ) {
               pArray[i] = nextOffset;
            }
            else {
               GET_PTR(previousItem, previousOffset, psonHashTxItem );
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
      rc = QSR_REC_CHANGES;
      qsrEcho( pVerify, "HashTx::numberOfItems is invalid" );
      if ( pVerify->doRepair ) {
         pHash->numberOfItems = numberOfItems;
         qsrEcho( pVerify, "Resetting HashTx::numberOfItems" );
      }
   }
   if ( pHash->totalDataSizeInBytes != totalDataSizeInBytes ) {
      rc = QSR_REC_CHANGES;
      qsrEcho( pVerify, "HashTx::totalDataSizeInBytes is invalid" );
      if ( pVerify->doRepair ) {
         pHash->totalDataSizeInBytes = totalDataSizeInBytes;
         qsrEcho( pVerify, "Resetting HashTx::totalDataSizeInBytes" );
      }
   }

   /*
    * Next iteration - are the keys in the right bucket? 
    * Question: can this really happen?
    */
   for ( i = 0; i < g_psonArrayLengths[pHash->lengthIndex]; ++i ) {
      currentOffset = pArray[i];
      
      while ( currentOffset != PSON_NULL_OFFSET ) {
         GET_PTR( pItem, currentOffset, psonHashTxItem );
         nextOffset = pItem->nextItem;
         
         bucket = fnv_buf( (void *)pItem->key, pItem->keyLength, FNV1_INIT) %
                     g_psonArrayLengths[pHash->lengthIndex];
         if ( bucket != i ) {
           rc = QSR_REC_CHANGES;
           qsrEcho( pVerify, "Hash item - invalid bucket" );
            invalidBuckets++;
            if ( pVerify->doRepair ) {
               if ( pArray[bucket] == PSON_NULL_OFFSET ) {
                  pArray[bucket] = currentOffset;
               }
               else {
                  tmpOffset = pArray[bucket];
                  do {
                     GET_PTR( pItem, tmpOffset, psonHashTxItem );
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
         GET_PTR( pItem, currentOffset, psonHashTxItem );
         nextOffset = pItem->nextItem;
         
         bucket = fnv_buf( (void *)pItem->key, pItem->keyLength, FNV1_INIT) %
                     g_psonArrayLengths[pHash->lengthIndex];
         if ( bucket != i ) {
            qsrEcho( pVerify, "Hash item - invalid bucket" );
            invalidBuckets++;
            if ( pVerify->doRepair ) {
               if ( pArray[bucket] == PSON_NULL_OFFSET ) {
                  pArray[bucket] = currentOffset;
               }
               else {
                  tmpOffset = pArray[bucket];
                  do {
                     GET_PTR( pItem, tmpOffset, psonHashTxItem );
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

