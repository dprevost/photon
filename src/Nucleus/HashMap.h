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

//   struct psoKeyDefinition keyDef;
   
   /** Offset to the data definition */
   ptrdiff_t            dataDefOffset;

   /** Offset to the key definition */
   ptrdiff_t keyDefOffset;

   uint32_t keyDefLength;
   uint32_t dataDefLength;
   
   /** The type of the key definition (metadata) */
   enum psoDefinitionType keyDefType;
   
   /** The type of the field definition (metadata) */
   enum psoDefinitionType fieldDefType;

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
                      const unsigned char * pKeyDef,
                      uint32_t              keyDefLength,
                      const unsigned char * pDataDef,
                      uint32_t              dataDefLength,
                      psonSessionContext  * pContext );

PHOTON_ENGINE_EXPORT
bool psonHashMapInsert( psonHashMap        * pHashMap,
                        const void         * pKey,
                        uint32_t             keyLength,
                        const void         * pItem,
                        uint32_t             itemLength,
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

