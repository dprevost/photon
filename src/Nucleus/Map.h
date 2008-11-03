/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@photonsoftware.org>
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

#ifndef PSON_MAP_H
#define PSON_MAP_H

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

struct psonFashMapItem
{
   psonHashItem * pHashItem;

   ptrdiff_t   itemOffset;
};

typedef struct psonFashMapItem psonFashMapItem;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psonMap
{
   /** Always first */
   struct psonMemObject memObject;

   /** Basic info for all leaves and branches of our tree. */
   struct psonTreeNode  nodeObject;

   struct psonHash      hashObj;

   struct psoKeyDefinition keyDef;
   
   /** Offset to the data definition */
   ptrdiff_t            dataDefOffset;

   ptrdiff_t latestVersion;

   ptrdiff_t editVersion;
   
   uint16_t numFields;
   
   /** Variable size struct - always put at the end */
   struct psonBlockGroup blockGroup;

};

typedef struct psonMap psonMap;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

PHOTON_ENGINE_EXPORT
bool psonMapCopy( psonMap            * pHashMap, 
                  psonMap            * pNewMap,
                  psonHashTxItem     * pHashItem,
                  const char         * origName,
                  psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonMapDelete( psonMap            * pHashMap,
                    const void         * key,
                    size_t               keyLength, 
                    psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
void psonMapEmpty( psonMap            * pHashMap,
                   psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
void psonMapFini( psonMap            * pHashMap,
                  psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonMapGet( psonMap            * pHashMap,
                 const void         * pKey,
                 size_t               keyLength, 
                 psonHashItem      ** ppItem,
                 size_t               bufferLength,
                 psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonMapGetFirst( psonMap            * pHashMap,
                      psonFashMapItem    * pItem,
                      size_t               keyLength,
                      size_t               bufferLength,
                      psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonMapGetNext( psonMap            * pHashMap,
                     psonFashMapItem    * pItem,
                     size_t               keyLength,
                     size_t               bufferLength,
                     psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonMapInit( psonMap             * pHashMap,
                  ptrdiff_t             parentOffset,
                  size_t                numberOfBlocks,
                  size_t                expectedNumOfChilds,
                  psonTxStatus        * pTxStatus,
                  size_t                origNameLength,
                  char                * origName,
                  ptrdiff_t             hashItemOffset,
                  psoObjectDefinition * pDefinition,
                  psonSessionContext  * pContext );

PHOTON_ENGINE_EXPORT
bool psonMapInsert( psonMap            * pHashMap,
                    const void         * pKey,
                    size_t               keyLength,
                    const void         * pItem,
                    size_t               itemLength,
                    psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonMapRelease( psonMap            * pHashMap,
                     psonHashItem       * pHashItem,
                     psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonMapReplace( psonMap            * pHashMap,
                     const void         * pKey,
                     size_t               keyLength,
                     const void         * pItem,
                     size_t               itemLength,
                     psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
void psonMapStatus( psonMap      * pHashMap,
                    psoObjStatus * pStatus );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSON_MAP_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

