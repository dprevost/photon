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

#ifndef VDSE_HASH_MAP_H
#define VDSE_HASH_MAP_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Engine/Engine.h"
#include "Engine/DataType.h"
#include "Engine/MemoryObject.h"
#include "Engine/TreeNode.h"
#include "Engine/BlockGroup.h"
#include "Engine/Hash.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct vdseHashMapItem
{
   vdseHashItem * pHashItem;

   ptrdiff_t   itemOffset;

};

typedef struct vdseHashMapItem vdseHashMapItem;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct vdseHashMap
{
   /** Always first */
   struct vdseMemObject memObject;

   /** Basic info for all leaves and branches of our tree. */
   struct vdseTreeNode  nodeObject;

   struct vdseHash      hashObj;

   struct vdsKeyDefinition keyDef;
   
   /** Offset to the data definition */
   ptrdiff_t            dataDefOffset;

   uint16_t numFields;
   
   /** Variable size struct - always put at the end */
   struct vdseBlockGroup blockGroup;

};

typedef struct vdseHashMap vdseHashMap;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

VDSF_ENGINE_EXPORT
void vdseHashMapCommitAdd( vdseHashMap        * pHashMap, 
                           ptrdiff_t            itemOffset,
                           vdseSessionContext * pContext  );

VDSF_ENGINE_EXPORT
void vdseHashMapCommitRemove( vdseHashMap        * pHashMap, 
                              ptrdiff_t            itemOffset,
                              vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
int vdseHashMapDelete( vdseHashMap        * pHashMap,
                       const void         * key,
                       size_t               keyLength, 
                       vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
void vdseHashMapFini( vdseHashMap        * pHashMap,
                      vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
int vdseHashMapGet( vdseHashMap        * pHashMap,
                    const void         * pKey,
                    size_t               keyLength, 
                    vdseHashItem      ** ppItem,
                    size_t               bufferLength,
                    vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
int vdseHashMapGetFirst( vdseHashMap        * pHashMap,
                         vdseHashMapItem    * pItem,
                         size_t               keyLength,
                         size_t               bufferLength,
                         vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
int vdseHashMapGetNext( vdseHashMap        * pHashMap,
                        vdseHashMapItem    * pItem,
                        size_t               keyLength,
                        size_t               bufferLength,
                        vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
int vdseHashMapInit( vdseHashMap         * pHashMap,
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
int vdseHashMapInsert( vdseHashMap        * pHashMap,
                       const void         * pKey,
                       size_t               keyLength,
                       const void         * pItem,
                       size_t               itemLength,
                       vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
int vdseHashMapRelease( vdseHashMap        * pHashMap,
                        vdseHashItem       * pHashItem,
                        vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
int vdseHashMapReplace( vdseHashMap        * pHashMap,
                        const void         * pKey,
                        size_t               keyLength,
                        const void         * pItem,
                        size_t               itemLength,
                        vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
void vdseHashMapRollbackAdd( vdseHashMap        * pHashMap, 
                             ptrdiff_t            itemOffset,
                             vdseSessionContext * pContext  );

VDSF_ENGINE_EXPORT
void vdseHashMapRollbackRemove( vdseHashMap        * pHashMap, 
                                ptrdiff_t            itemOffset,
                                vdseSessionContext * pContext  );

VDSF_ENGINE_EXPORT
void vdseHashMapStatus( vdseHashMap  * pHashMap,
                        vdsObjStatus * pStatus );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSE_HASH_MAP_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

