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
#include "MemoryAllocator.h"
#include "ListErrors.h"
#include "SessionContext.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define HASH_CASE_INSENSITIVE 1

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Descriptor for elements of the hash array. It keeps track of the locations
 * of both the key and the data and of their respective length.
 */
typedef struct vdseHashElement
{
   size_t    keyLength;
   size_t    dataLength;
   ptrdiff_t dataOffset;
   ptrdiff_t keyOffset;
   
} vdseHashElement;

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
   /** Offset to an array of offsets to vdseHashElement objects */
   ptrdiff_t    arrayOffset; 
   
   size_t       numberOfRows;
   size_t       arraySize;
   unsigned int lowDensity;   
   unsigned int highDensity;
   size_t       totalSize;
   int          flag;
   
} vdseHash;


/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseHashEmpty( vdseSessionContext* pContext );

void vdseHashClose( vdseSessionContext* pContext );
   
enum ListErrors 
vdseHashInit( vdseHash*           pHash,
              int                 caseSensitiveFlag,
              size_t              initialSize, 
              vdseSessionContext* pContext );

enum ListErrors 
vdseHashGetFirst( vdseHash*         pHash,
                  size_t*           pNewRowNumber,
                  vdseHashElement** ppRow,
                  vdseMemAlloc*     pAlloc );

enum ListErrors 
vdseHashGetNext( vdseHash*         pHash,
                 size_t            oldRowNumber,
                 size_t*           pNewRowNumber, 
                 vdseHashElement** ppRow,
                 vdseMemAlloc*     pAlloc );

enum ListErrors 
vdseHashInsert( vdseHash*           pHash,
                const char*         pKey,
                size_t              keyLength,
                void*               pData,
                size_t              dataLength,
                /* pSelf is used to access the name of  */
                /* objects from the "this" pointer of */
                /* the objects themselves */
                ptrdiff_t*          pSelf,
                vdseSessionContext* pContext );

enum ListErrors 
vdseHashDelete( vdseHash*           pHash,
                const char*         pKey, 
                size_t              keyLength,
                vdseSessionContext* pContext );

enum ListErrors 
vdseHashReplaceData( vdseHash*           pHash,
                     const char*         pKey,
                     size_t              keyLength,
                     void*               pData,
                     size_t              dataLength,
                     vdseSessionContext* pContext );

enum ListErrors 
vdseHashGet( vdseHash*           pHash,
             const char*         pkey,
             size_t              keyLength,
             void **             ppData,
             size_t*             pDataLength,
             vdseSessionContext* pContext,
             size_t*             pRowNumber );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSE_HASH_MAP_H */
