/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef PSN_MAP_H
#define PSN_MAP_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Nucleus/Engine.h"
#include "Nucleus/DataType.h"
#include "Nucleus/MemoryObject.h"
#include "Nucleus/TreeNode.h"
#include "Nucleus/BlockGroup.h"
#include "Nucleus/Hash.h"
#include "Nucleus/HashMap.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psnMap
{
   /** Always first */
   struct psnMemObject memObject;

   /** Basic info for all leaves and branches of our tree. */
   struct psnTreeNode  nodeObject;

   struct psnHash      hashObj;

   struct psoKeyDefinition keyDef;
   
   /** Offset to the data definition */
   ptrdiff_t            dataDefOffset;

   ptrdiff_t latestVersion;

   ptrdiff_t editVersion;
   
   uint16_t numFields;
   
   /** Variable size struct - always put at the end */
   struct psnBlockGroup blockGroup;

};

typedef struct psnMap psnMap;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

VDSF_ENGINE_EXPORT
bool psnMapCopy( psnMap            * pHashMap, 
                  psnMap            * pNewMap,
                  psnHashItem       * pHashItem,
                  const char         * origName,
                  psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool psnMapDelete( psnMap            * pHashMap,
                    const void         * key,
                    size_t               keyLength, 
                    psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
void psnMapEmpty( psnMap            * pHashMap,
                   psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
void psnMapFini( psnMap            * pHashMap,
                  psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool psnMapGet( psnMap            * pHashMap,
                 const void         * pKey,
                 size_t               keyLength, 
                 psnHashItem      ** ppItem,
                 size_t               bufferLength,
                 psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool psnMapGetFirst( psnMap            * pHashMap,
                      psnHashMapItem    * pItem,
                      size_t               keyLength,
                      size_t               bufferLength,
                      psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool psnMapGetNext( psnMap            * pHashMap,
                     psnHashMapItem    * pItem,
                     size_t               keyLength,
                     size_t               bufferLength,
                     psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool psnMapInit( psnMap             * pHashMap,
                  ptrdiff_t             parentOffset,
                  size_t                numberOfBlocks,
                  size_t                expectedNumOfChilds,
                  psnTxStatus        * pTxStatus,
                  size_t                origNameLength,
                  char                * origName,
                  ptrdiff_t             hashItemOffset,
                  psoObjectDefinition * pDefinition,
                  psnSessionContext  * pContext );

VDSF_ENGINE_EXPORT
bool psnMapInsert( psnMap            * pHashMap,
                    const void         * pKey,
                    size_t               keyLength,
                    const void         * pItem,
                    size_t               itemLength,
                    psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool psnMapRelease( psnMap            * pHashMap,
                     psnHashItem       * pHashItem,
                     psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool psnMapReplace( psnMap            * pHashMap,
                     const void         * pKey,
                     size_t               keyLength,
                     const void         * pItem,
                     size_t               itemLength,
                     psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
void psnMapStatus( psnMap      * pHashMap,
                    psoObjStatus * pStatus );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSN_MAP_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

