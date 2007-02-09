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
 * The current version of the Hash uses linear probing. This will be
 * modified (in a future version) to use buckets instead. 

 * One of the main advantage of buckets is that it gives us a better 
 * method for controlling when the Hash is reorganized (it have to be 
 * done in sync with a transaction to avoid confusion - with linear probing 
 * there is a hard limit on how long we can wait for a reorganization 
 * (when the array is 100% populated). There is no such limit with buckets 
 * since it is an array of linked lists).
 *
 * \todo The case sensitive/case insensitive stuff needs some rethinking,
 *       specially in view of using Unicode internally to hold string
 *       keys. One solution might be to have 4 data elements instead of
 *       2: the label (user provided key), the character set used for
 *       the label. the key (uppercase and in a standard Unicode character
 *       set [UCS4?]) and the data.
 */

typedef struct vdseHash
{
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

VDSF_ENGINE_EXPORT enum ListErrors 
vdseHashDelete( vdseHash*            pHash,
                const unsigned char* pKey, 
                size_t               keyLength,
                vdseSessionContext*  pContext );

VDSF_ENGINE_EXPORT 
void vdseHashEmpty( vdseHash*           pHash,
                    vdseSessionContext* pContext );

VDSF_ENGINE_EXPORT
void vdseHashFini( vdseHash*           pHash,
                   vdseSessionContext* pContext );

VDSF_ENGINE_EXPORT enum ListErrors 
vdseHashInit( vdseHash*           pHash,
              size_t              reservedSize, 
              vdseSessionContext* pContext );

VDSF_ENGINE_EXPORT enum ListErrors 
vdseHashGet( vdseHash*            pHash,
             const unsigned char* pkey,
             size_t               keyLength,
             vdseHashItem**       ppItem,
             vdseSessionContext*  pContext,
             size_t*              pBucket );

VDSF_ENGINE_EXPORT enum ListErrors 
vdseHashGetFirst( vdseHash*  pHash,
                  size_t*    pBucket, 
                  ptrdiff_t* pFirstItemOffset );

VDSF_ENGINE_EXPORT enum ListErrors 
vdseHashGetNext( vdseHash*  pHash,
                 size_t     previousBucket,
                 ptrdiff_t  previousOffset,
                 size_t*    pNextBucket, 
                 ptrdiff_t* pNextItemOffset );

VDSF_ENGINE_EXPORT enum ListErrors 
vdseHashInsert( vdseHash*            pHash,
                const unsigned char* pKey,
                size_t               keyLength,
                void*                pData,
                size_t               dataLength,
                /* ppNewItem is used to access the original name of 
                 * objects and the vdseTxStatus by the objects themselves */
                vdseHashItem**       ppNewItem,
                vdseSessionContext*  pContext );

VDSF_ENGINE_EXPORT enum ListErrors 
vdseHashResize( vdseHash*           pHash,
                vdseSessionContext* pContext );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSE_HASH_MAP_H */

