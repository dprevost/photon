/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef PSN_HASH_H
#define PSN_HASH_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Nucleus/Engine.h"
#include "Nucleus/SessionContext.h"
#include "Nucleus/TxStatus.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define PSN_HASH_SIGNATURE  ((unsigned int)0x2026fe02)

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Descriptor/container for elements of the hash array. 
 *
 * Note: the data is to be found after the key. It MUST be properly 
 * aligned if we eventually allow direct access to the data (from the 
 * API) or even to be able to use it easily internally. 
 */
struct psnHashItem
{
   psnTxStatus  txStatus;
   
   /** Next item in this bucket */
   ptrdiff_t     nextItem;
   /** Next item with same key (for replace, etc.) */
   ptrdiff_t     nextSameKey;
   
   size_t        keyLength;
   ptrdiff_t     dataOffset;
   size_t        dataLength;
   size_t        bucket;
   unsigned char key[1];
   
};

typedef struct psnHashItem psnHashItem;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum psnHashResizeEnum
{
   PSN_HASH_NO_RESIZE,
   PSN_HASH_TIME_TO_GROW,
   PSN_HASH_TIME_TO_SHRINK
   
};

typedef enum psnHashResizeEnum psnHashResizeEnum;

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
struct psnHash
{
   /** offset of the memory object we need to use for allocating memory. */
   ptrdiff_t memObjOffset;
   
   /** Offset to an array of offsets to psnHashItem objects */
   ptrdiff_t    arrayOffset; 
   
   /** Number of items stored in this hash map. */
   size_t       numberOfItems;
   
   /** Total amount of bytes of data stored in this hash */
   size_t       totalDataSizeInBytes;

   /** The index into the array of lengths (aka the number of buckets). */
   int lengthIndex;

   /** The mimimum shrinking factor that we can tolerate to accommodate
    *  the reservedSize argument of psnHashInit() ) */
   int lengthIndexMinimum;

   /** Indicator of the current status of the array. */
   psnHashResizeEnum enumResize;
   
   /** Set to PSN_HASH_SIGNATURE at initialization. */
   unsigned int initialized;

};

typedef struct psnHash psnHash;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Used to create a copy of a read-only hash map for editing (updates)
 */
PHOTON_ENGINE_EXPORT
enum psoErrors psnHashCopy( psnHash           * pOldHash,
                             psnHash           * pNewHash,
                             psnSessionContext * pContext );

/*
 * Used to delete an hash item when you know its exact position
 * (through the psnHashItem) 
 */
PHOTON_ENGINE_EXPORT 
void psnHashDelWithItem( psnHash            * pHash,
                          psnHashItem        * pItem,
                          psnSessionContext  * pContext );

/* Direct delete using the key and nothing else. */
PHOTON_ENGINE_EXPORT 
bool psnHashDelWithKey( psnHash            * pHash,
                         const unsigned char * pKey, 
                         size_t                keyLength,
                         psnSessionContext  * pContext );

PHOTON_ENGINE_EXPORT 
void psnHashEmpty( psnHash           * pHash,
                    psnSessionContext * pContext );

PHOTON_ENGINE_EXPORT
void psnHashFini( psnHash * pHash );

PHOTON_ENGINE_EXPORT 
bool psnHashGet( psnHash            * pHash,
                  const unsigned char * pkey,
                  size_t                keyLength,
                  psnHashItem       ** ppItem,
                  size_t              * pBucket,
                  psnSessionContext  * pContext );

PHOTON_ENGINE_EXPORT 
bool psnHashGetFirst( psnHash  * pHash,
                       ptrdiff_t * pFirstItemOffset );

PHOTON_ENGINE_EXPORT
bool psnHashGetNext( psnHash  * pHash,
                      ptrdiff_t   previousOffset,
                      ptrdiff_t * pNextItemOffset );

PHOTON_ENGINE_EXPORT 
enum psoErrors psnHashInit( psnHash           * pHash,
                             ptrdiff_t            memObjOffset,
                             size_t               reservedSize, 
                             psnSessionContext * pContext );

/*
 * ppNewItem is used to access the original name of 
 * objects and the psnTxStatus by the objects themselves 
 */
PHOTON_ENGINE_EXPORT 
enum psoErrors psnHashInsert( psnHash            * pHash,
                               const unsigned char * pKey,
                               size_t                keyLength,
                               const void          * pData,
                               size_t                dataLength,
                               psnHashItem       ** ppNewItem,
                               psnSessionContext  * pContext );

/*
 * Insert at is used to insert an item in a given bucket, at the end
 * of the linked list. This is used for adding a replacement item, 
 * before the change is committed.
 */
PHOTON_ENGINE_EXPORT 
enum psoErrors psnHashInsertAt( psnHash            * pHash,
                                 size_t                bucket,
                                 const unsigned char * pKey,
                                 size_t                keyLength,
                                 const void          * pData,
                                 size_t                dataLength,
                                 psnHashItem       ** ppNewItem,
                                 psnSessionContext  * pContext );

PHOTON_ENGINE_EXPORT
enum psoErrors psnHashResize( psnHash           * pHash,
                               psnSessionContext * pContext );

PHOTON_ENGINE_EXPORT
enum psoErrors psnHashUpdate( psnHash            * pHash,
                               const unsigned char * pKey,
                               size_t                keyLength,
                               const void          * pData,
                               size_t                dataLength,
                               psnSessionContext  * pContext );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
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
#  define PSN_PRIME_NUMBER_ARRAY_LENGTH 28
PHOTON_ENGINE_EXPORT
extern size_t g_psnArrayLengths[PSN_PRIME_NUMBER_ARRAY_LENGTH];
#else
#  define PSN_PRIME_NUMBER_ARRAY_LENGTH 60
PHOTON_ENGINE_EXPORT
extern size_t g_psnArrayLengths[PSN_PRIME_NUMBER_ARRAY_LENGTH];
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSN_HASH_MAP_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

