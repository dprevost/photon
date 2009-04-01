/*
 * Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#ifndef PSON_HASH_MAP_H
#define PSON_HASH_MAP_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Nucleus/Engine.h"
#include "Nucleus/MemoryObject.h"
#include "Nucleus/TreeNode.h"
#include "Nucleus/BlockGroup.h"
#include "Nucleus/HashTx.h"
#include "Nucleus/Definitions.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psonHashMapItem
{
   psonHashTxItem * pHashItem;

   ptrdiff_t   itemOffset;

};

typedef struct psonHashMapItem psonHashMapItem;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psonHashMap
{
   /** Always first */
   struct psonMemObject memObject;

   /** Basic info for all leaves and branches of our tree. */
   struct psonTreeNode  nodeObject;

   struct psonHashTx      hashObj;

   /** Offset to the data definition */
   ptrdiff_t            dataDefOffset;

   /** Offset to the key definition */
   ptrdiff_t keyDefOffset;

   /* Creation flags */
   uint32_t flags;

   /*
    * This field cannot be set or modified by the API. It is set to false
    * by the "constructor". Quasar will set it to true, as needed, when 
    * creating a new shared-memory and constructing its system objects.
    *
    * When this field is set to true, no data can be added or removed 
    * directly by accessing the object through the API. Updates are done 
    * indirectly as a consequence of some API functions.
    */
   bool isSystemObject;

   /** Variable size struct - always put at the end */
   struct psonBlockGroup blockGroup;

};

typedef struct psonHashMap psonHashMap;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

PHOTON_ENGINE_EXPORT
void psonHashMapCommitAdd( psonHashMap        * pHashMap, 
                           ptrdiff_t            itemOffset,
                           psonSessionContext * pContext  );

PHOTON_ENGINE_EXPORT
void psonHashMapCommitRemove( psonHashMap        * pHashMap, 
                              ptrdiff_t            itemOffset,
                              psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonHashMapDelete( psonHashMap        * pHashMap,
                        const void         * key,
                        uint32_t             keyLength, 
                        psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
void psonHashMapFini( psonHashMap        * pHashMap,
                      psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonHashMapGet( psonHashMap        * pHashMap,
                     const void         * pKey,
                     uint32_t             keyLength, 
                     psonHashTxItem      ** ppItem,
                     uint32_t             bufferLength,
                     psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonHashMapGetFirst( psonHashMap        * pHashMap,
                          psonHashMapItem    * pItem,
                          uint32_t             keyLength,
                          uint32_t             bufferLength,
                          psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonHashMapGetNext( psonHashMap        * pHashMap,
                         psonHashMapItem    * pItem,
                         uint32_t             keyLength,
                         uint32_t             bufferLength,
                         psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonHashMapInit( psonHashMap         * pHashMap,
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
bool psonHashMapInsert( psonHashMap        * pHashMap,
                        const void         * pKey,
                        uint32_t             keyLength,
                        const void         * pItem,
                        uint32_t             itemLength,
                        psonDataDefinition * pDefinition,
                        psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonHashMapRelease( psonHashMap        * pHashMap,
                         psonHashTxItem       * pHashItem,
                         psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonHashMapReplace( psonHashMap        * pHashMap,
                         const void         * pKey,
                         uint32_t             keyLength,
                         const void         * pItem,
                         uint32_t             itemLength,
                         psonDataDefinition * pDefinition,
                         psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
void psonHashMapRollbackAdd( psonHashMap        * pHashMap, 
                             ptrdiff_t            itemOffset,
                             psonSessionContext * pContext  );

PHOTON_ENGINE_EXPORT
void psonHashMapRollbackRemove( psonHashMap        * pHashMap, 
                                ptrdiff_t            itemOffset,
                                psonSessionContext * pContext  );

PHOTON_ENGINE_EXPORT
void psonHashMapStatus( psonHashMap  * pHashMap,
                        psoObjStatus * pStatus );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSON_HASH_MAP_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

