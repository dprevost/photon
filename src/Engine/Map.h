/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef VDSE_MAP_H
#define VDSE_MAP_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Engine/Engine.h"
#include "Engine/DataType.h"
#include "Engine/MemoryObject.h"
#include "Engine/TreeNode.h"
#include "Engine/BlockGroup.h"
#include "Engine/Hash.h"
#include "Engine/HashMap.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct vdseMap
{
   /** Always first */
   struct vdseMemObject memObject;

   /** Basic info for all leaves and branches of our tree. */
   struct vdseTreeNode  nodeObject;

   struct vdseHash      hashObj;

   struct vdsKeyDefinition keyDef;
   
   /** Offset to the data definition */
   ptrdiff_t            dataDefOffset;

   ptrdiff_t latestVersion;

   ptrdiff_t editVersion;
   
   uint16_t numFields;
   
   /** Variable size struct - always put at the end */
   struct vdseBlockGroup blockGroup;

};

typedef struct vdseMap vdseMap;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

VDSF_ENGINE_EXPORT
int vdseMapCopy( vdseMap            * pHashMap, 
                 vdseMap            * pNewMap,
                 vdseHashItem       * pHashItem,
                 const char         * origName,
                 vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
int vdseMapDelete( vdseMap            * pHashMap,
                   const void         * key,
                   size_t               keyLength, 
                   vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
void vdseMapEmpty( vdseMap            * pHashMap,
                   vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
void vdseMapFini( vdseMap            * pHashMap,
                  vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
int vdseMapGet( vdseMap            * pHashMap,
                const void         * pKey,
                size_t               keyLength, 
                vdseHashItem      ** ppItem,
                size_t               bufferLength,
                vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
int vdseMapGetFirst( vdseMap            * pHashMap,
                     vdseHashMapItem    * pItem,
                     size_t               keyLength,
                     size_t               bufferLength,
                     vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
int vdseMapGetNext( vdseMap            * pHashMap,
                    vdseHashMapItem    * pItem,
                    size_t               keyLength,
                    size_t               bufferLength,
                    vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
int vdseMapInit( vdseMap             * pHashMap,
                 ptrdiff_t             parentOffset,
                 size_t                numberOfBlocks,
                 size_t                expectedNumOfChilds,
                 vdseTxStatus        * pTxStatus,
                 size_t                origNameLength,
                 char                * origName,
                 ptrdiff_t             hashItemOffset,
                 vdsObjectDefinition * pDefinition,
                 vdseSessionContext  * pContext );


VDSF_ENGINE_EXPORT
int vdseMapInsert( vdseMap            * pHashMap,
                   const void         * pKey,
                   size_t               keyLength,
                   const void         * pItem,
                   size_t               itemLength,
                   vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
int vdseMapRelease( vdseMap            * pHashMap,
                    vdseHashItem       * pHashItem,
                    vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
int vdseMapReplace( vdseMap            * pHashMap,
                    const void         * pKey,
                    size_t               keyLength,
                    const void         * pItem,
                    size_t               itemLength,
                    vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
void vdseMapStatus( vdseMap      * pHashMap,
                    vdsObjStatus * pStatus );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSE_MAP_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

