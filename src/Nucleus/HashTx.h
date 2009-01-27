/*
 * Copyright (C) 2006-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#ifndef PSON_HASH_TX_H
#define PSON_HASH_TX_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Nucleus/HashCommon.h"
#include "Nucleus/TxStatus.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define PSON_HASH_TX_SIGNATURE  ((unsigned int)0x2026fe02)

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Descriptor/container for elements of the hash array. 
 *
 * Note: the data is to be found after the key. It MUST be properly 
 * aligned if we eventually allow direct access to the data (from the 
 * API) or even to be able to use it easily internally. 
 */
struct psonHashTxItem
{
   psonTxStatus  txStatus;
   
   /** Next item in this bucket */
   ptrdiff_t     nextItem;
   /** Next item with same key (for replace, etc.) */
   ptrdiff_t     nextSameKey;
   
   uint32_t      keyLength;
   ptrdiff_t     dataOffset;
   uint32_t      dataLength;
   size_t        bucket;
   unsigned char key[1];
   
};

typedef struct psonHashTxItem psonHashTxItem;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * The current version of the Hash uses buckets (the first version was
 * built with linear probing).
 *
 * One of the main advantage of buckets is that it gives us a better 
 * method for controlling when the Hash is reorganized (it have to be 
 * done in sync with a transaction to avoid confusion - with linear probing 
 * there is a hard limit on how long we can wait for a reorganization 
 * (when the array is 100% populated). There is no such limit with buckets 
 * since it is an array of linked lists).
 */
struct psonHashTx
{
   /** offset of the memory object we need to use for allocating memory. */
   ptrdiff_t memObjOffset;
   
   /** Offset to an array of offsets to psonHashTxItem objects */
   ptrdiff_t    arrayOffset; 
   
   /** Number of items stored in this hash map. */
   size_t       numberOfItems;
   
   /** Total amount of bytes of data stored in this hash */
   size_t       totalDataSizeInBytes;

   /** The index into the array of lengths (aka the number of buckets). */
   int lengthIndex;

   /** The mimimum shrinking factor that we can tolerate to accommodate
    *  the reservedSize argument of psonHashTxInit() ) */
   int lengthIndexMinimum;

   /** Indicator of the current status of the array. */
   psonHashResizeEnum enumResize;
   
   /** Set to PSON_HASH_TX_SIGNATURE at initialization. */
   unsigned int initialized;

};

typedef struct psonHashTx psonHashTx;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Used to delete an hash item when you know its exact position
 * (through the psonHashTxItem) 
 */
PHOTON_ENGINE_EXPORT 
void psonHashTxDelete( psonHashTx         * pHash,
                       psonHashTxItem     * pItem,
                       psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
void psonHashTxFini( psonHashTx * pHash );

PHOTON_ENGINE_EXPORT 
bool psonHashTxGet( psonHashTx          * pHash,
                    const unsigned char * pkey,
                    uint32_t              keyLength,
                    psonHashTxItem     ** ppItem,
                    size_t              * pBucket,
                    psonSessionContext  * pContext );

PHOTON_ENGINE_EXPORT 
bool psonHashTxGetFirst( psonHashTx * pHash,
                         ptrdiff_t  * pFirstItemOffset );

PHOTON_ENGINE_EXPORT
bool psonHashTxGetNext( psonHashTx * pHash,
                        ptrdiff_t    previousOffset,
                        ptrdiff_t  * pNextItemOffset );

PHOTON_ENGINE_EXPORT 
enum psoErrors psonHashTxInit( psonHashTx         * pHash,
                               ptrdiff_t            memObjOffset,
                               size_t               reservedSize, 
                               psonSessionContext * pContext );

/*
 * Insert always insert the item in a given bucket, at the end of the 
 * linked list.
 * Because of transactions, Insert() is always preceded with a Get().
 * If a record with the same key exists, its transaction status is checked 
 * to see if the new record can be inserted or not (the call to Get() gives
 * us the bucket needed by Insert()).
 */
PHOTON_ENGINE_EXPORT 
enum psoErrors psonHashTxInsert( psonHashTx          * pHash,
                                 size_t                bucket,
                                 const unsigned char * pKey,
                                 uint32_t              keyLength,
                                 const void          * pData,
                                 uint32_t              dataLength,
                                 psonHashTxItem     ** ppNewItem,
                                 psonSessionContext  * pContext );

PHOTON_ENGINE_EXPORT
enum psoErrors psonHashTxResize( psonHashTx         * pHash,
                                 psonSessionContext * pContext );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSON_HASH_TX_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

