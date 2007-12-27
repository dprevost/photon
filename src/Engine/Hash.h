/*
 * Copyright (C) 2006 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef VDSE_HASH_H
#define VDSE_HASH_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Engine.h"
#include "ListErrors.h"
#include "SessionContext.h"
#include "TxStatus.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define VDSE_HASH_SIGNATURE  ((unsigned int)0x2026fe02)

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Descriptor/container for elements of the hash array. 
 *
 * Note: the data is to be found after the key. It MUST be properly 
 * aligned if we eventually allow direct access to the data (from the 
 * API) or even to be able to use it easily internally. 
 */
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

typedef enum vdseHashResizeEnum
{
   VDSE_HASH_NO_RESIZE,
   VDSE_HASH_TIME_TO_GROW,
   VDSE_HASH_TIME_TO_SHRINK
   
} vdseHashResizeEnum;

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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

VDSF_ENGINE_EXPORT 
enum ListErrors vdseHashDelete( vdseHash            * pHash,
                                const unsigned char * pKey, 
                                size_t                keyLength,
                                vdseHashItem        * pHashItem,
                                vdseSessionContext  * pContext );

/* Used to delete an hash item when you know its exact position */
VDSF_ENGINE_EXPORT 
void vdseHashDeleteAt( vdseHash            * pHash,
                       size_t                bucket,
                       vdseHashItem        * pItem,
                       vdseSessionContext  * pContext );

VDSF_ENGINE_EXPORT 
void vdseHashEmpty( vdseHash           * pHash,
                    vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
void vdseHashFini( vdseHash * pHash );

VDSF_ENGINE_EXPORT 
enum ListErrors vdseHashGet( vdseHash            * pHash,
                             const unsigned char * pkey,
                             size_t                keyLength,
                             vdseHashItem       ** ppItem,
                             vdseSessionContext  * pContext,
                             size_t              * pBucket );

VDSF_ENGINE_EXPORT 
enum ListErrors vdseHashGetFirst( vdseHash  * pHash,
                                  size_t    * pBucket, 
                                  ptrdiff_t * pFirstItemOffset );

VDSF_ENGINE_EXPORT
enum ListErrors vdseHashGetNext( vdseHash  * pHash,
                                 size_t      previousBucket,
                                 ptrdiff_t   previousOffset,
                                 size_t    * pNextBucket, 
                                 ptrdiff_t * pNextItemOffset );

VDSF_ENGINE_EXPORT 
enum ListErrors vdseHashInit( vdseHash           * pHash,
                              ptrdiff_t            memObjOffset,
                              size_t               reservedSize, 
                              vdseSessionContext * pContext );

/*
 * ppNewItem is used to access the original name of 
 * objects and the vdseTxStatus by the objects themselves 
 */
VDSF_ENGINE_EXPORT 
enum ListErrors vdseHashInsert( vdseHash            * pHash,
                                const unsigned char * pKey,
                                size_t                keyLength,
                                const void          * pData,
                                size_t                dataLength,
                                vdseHashItem       ** ppNewItem,
                                vdseSessionContext  * pContext );

/*
 * Insert at is used to insert an item in a given bucket, at the end
 * of the linked list. This is used for adding a replacement item, 
 * before the change is committed.
 */
VDSF_ENGINE_EXPORT 
enum ListErrors vdseHashInsertAt( vdseHash            * pHash,
                                  size_t                bucket,
                                  const unsigned char * pKey,
                                  size_t                keyLength,
                                  const void          * pData,
                                  size_t                dataLength,
                                  vdseHashItem       ** ppNewItem,
                                  vdseSessionContext  * pContext );

VDSF_ENGINE_EXPORT
enum ListErrors vdseHashResize( vdseHash           * pHash,
                                vdseSessionContext * pContext );

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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSE_HASH_MAP_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

