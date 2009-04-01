/*
 * Copyright (C) 2008-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#ifndef PSON_FASTMAP_H
#define PSON_FASTMAP_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Nucleus/Engine.h"
#include "Nucleus/MemoryObject.h"
#include "Nucleus/TreeNode.h"
#include "Nucleus/BlockGroup.h"
#include "Nucleus/Hash.h"
#include "Nucleus/HashMap.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psonFastMapItem
{
   psonHashItem * pHashItem;
};

typedef struct psonFastMapItem psonFastMapItem;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psonFastMap
{
   /** Always first */
   struct psonMemObject memObject;

   /** Basic info for all leaves and branches of our tree. */
   struct psonTreeNode  nodeObject;

   struct psonHash hashObj;

   /** Offset to the data definition */
   ptrdiff_t dataDefOffset;

   /** Offset to the key definition */
   ptrdiff_t keyDefOffset;

   /* Creation flags */
   uint32_t flags;

   ptrdiff_t latestVersion;

   ptrdiff_t editVersion;
   
   /** Variable size struct - always put at the end */
   struct psonBlockGroup blockGroup;

};

typedef struct psonFastMap psonFastMap;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

PHOTON_ENGINE_EXPORT
bool psonFastMapCopy( psonFastMap        * pHashMap, 
                      psonFastMap        * pNewHashMap,
                      psonHashTxItem     * pHashItem,
                      const char         * origName,
                      psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonFastMapDelete( psonFastMap        * pHashMap,
                        const void         * key,
                        uint32_t             keyLength, 
                        psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
void psonFastMapEmpty( psonFastMap        * pHashMap,
                       psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
void psonFastMapFini( psonFastMap        * pHashMap,
                      psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonFastMapGet( psonFastMap        * pHashMap,
                     const void         * pKey,
                     uint32_t             keyLength, 
                     psonHashItem      ** ppItem,
                     uint32_t             bufferLength,
                     psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonFastMapGetFirst( psonFastMap        * pHashMap,
                          psonFastMapItem    * pItem,
                          uint32_t             keyLength,
                          uint32_t             bufferLength,
                          psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonFastMapGetNext( psonFastMap        * pHashMap,
                         psonFastMapItem    * pItem,
                         uint32_t             keyLength,
                         uint32_t             bufferLength,
                         psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonFastMapInit( psonFastMap         * pHashMap,
                      ptrdiff_t             parentOffset,
                      size_t                numberOfBlocks,
                      size_t                expectedNumOfChilds,
                      psonTxStatus        * pTxStatus,
                      uint32_t              origNameLength,
                      char                * origName,
                      ptrdiff_t             hashItemOffset,
                      psoObjectDefinition * pDefinition,
                      psonKeyDefinition   * pKeyDefinition,
                      psonDataDefinition  * pDataDefinition,
                      psonSessionContext  * pContext );

PHOTON_ENGINE_EXPORT
bool psonFastMapInsert( psonFastMap        * pHashMap,
                        const void         * pKey,
                        uint32_t             keyLength,
                        const void         * pItem,
                        uint32_t             itemLength,
                        psonDataDefinition * pDefinition,
                        psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonFastMapRelease( psonFastMap        * pHashMap,
                         psonHashItem       * pHashItem,
                         psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonFastMapReplace( psonFastMap        * pHashMap,
                         const void         * pKey,
                         uint32_t             keyLength,
                         const void         * pItem,
                         uint32_t             itemLength,
                         psonDataDefinition * pDefinition,
                         psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
void psonFastMapStatus( psonFastMap  * pHashMap,
                        psoObjStatus * pStatus );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSON_FASTMAP_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

