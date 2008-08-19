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

#ifndef PSN_HASH_MAP_H
#define PSN_HASH_MAP_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Nucleus/Engine.h"
#include "Nucleus/DataType.h"
#include "Nucleus/MemoryObject.h"
#include "Nucleus/TreeNode.h"
#include "Nucleus/BlockGroup.h"
#include "Nucleus/Hash.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psnHashMapItem
{
   psnHashItem * pHashItem;

   ptrdiff_t   itemOffset;

};

typedef struct psnHashMapItem psnHashMapItem;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psnHashMap
{
   /** Always first */
   struct psnMemObject memObject;

   /** Basic info for all leaves and branches of our tree. */
   struct psnTreeNode  nodeObject;

   struct psnHash      hashObj;

   struct vdsKeyDefinition keyDef;
   
   /** Offset to the data definition */
   ptrdiff_t            dataDefOffset;

   uint16_t numFields;
   
   /** Variable size struct - always put at the end */
   struct psnBlockGroup blockGroup;

};

typedef struct psnHashMap psnHashMap;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

VDSF_ENGINE_EXPORT
void psnHashMapCommitAdd( psnHashMap        * pHashMap, 
                           ptrdiff_t            itemOffset,
                           psnSessionContext * pContext  );

VDSF_ENGINE_EXPORT
void psnHashMapCommitRemove( psnHashMap        * pHashMap, 
                              ptrdiff_t            itemOffset,
                              psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool psnHashMapDelete( psnHashMap        * pHashMap,
                        const void         * key,
                        size_t               keyLength, 
                        psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
void psnHashMapFini( psnHashMap        * pHashMap,
                      psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool psnHashMapGet( psnHashMap        * pHashMap,
                     const void         * pKey,
                     size_t               keyLength, 
                     psnHashItem      ** ppItem,
                     size_t               bufferLength,
                     psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool psnHashMapGetFirst( psnHashMap        * pHashMap,
                          psnHashMapItem    * pItem,
                          size_t               keyLength,
                          size_t               bufferLength,
                          psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool psnHashMapGetNext( psnHashMap        * pHashMap,
                         psnHashMapItem    * pItem,
                         size_t               keyLength,
                         size_t               bufferLength,
                         psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool psnHashMapInit( psnHashMap         * pHashMap,
                      ptrdiff_t             parentOffset,
                      size_t                numberOfBlocks,
                      size_t                expectedNumOfChilds,
                      psnTxStatus        * pTxStatus,
                      size_t                origNameLength,
                      char                * origName,
                      ptrdiff_t             hashItemOffset,
                      vdsObjectDefinition * pDefinition,
                      psnSessionContext  * pContext );

VDSF_ENGINE_EXPORT
bool psnHashMapInsert( psnHashMap        * pHashMap,
                        const void         * pKey,
                        size_t               keyLength,
                        const void         * pItem,
                        size_t               itemLength,
                        psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool psnHashMapRelease( psnHashMap        * pHashMap,
                         psnHashItem       * pHashItem,
                         psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool psnHashMapReplace( psnHashMap        * pHashMap,
                         const void         * pKey,
                         size_t               keyLength,
                         const void         * pItem,
                         size_t               itemLength,
                         psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
void psnHashMapRollbackAdd( psnHashMap        * pHashMap, 
                             ptrdiff_t            itemOffset,
                             psnSessionContext * pContext  );

VDSF_ENGINE_EXPORT
void psnHashMapRollbackRemove( psnHashMap        * pHashMap, 
                                ptrdiff_t            itemOffset,
                                psnSessionContext * pContext  );

VDSF_ENGINE_EXPORT
void psnHashMapStatus( psnHashMap  * pHashMap,
                        vdsObjStatus * pStatus );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSN_HASH_MAP_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

