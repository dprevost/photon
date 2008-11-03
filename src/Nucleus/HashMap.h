/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@photonsoftware.org>
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

#ifndef PSON_HASH_MAP_H
#define PSON_HASH_MAP_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Nucleus/Engine.h"
#include "Nucleus/DataType.h"
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

   struct psoKeyDefinition keyDef;
   
   /** Offset to the data definition */
   ptrdiff_t            dataDefOffset;

   uint16_t numFields;
   
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
                        size_t               keyLength, 
                        psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
void psonHashMapFini( psonHashMap        * pHashMap,
                      psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonHashMapGet( psonHashMap        * pHashMap,
                     const void         * pKey,
                     size_t               keyLength, 
                     psonHashTxItem      ** ppItem,
                     size_t               bufferLength,
                     psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonHashMapGetFirst( psonHashMap        * pHashMap,
                          psonHashMapItem    * pItem,
                          size_t               keyLength,
                          size_t               bufferLength,
                          psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonHashMapGetNext( psonHashMap        * pHashMap,
                         psonHashMapItem    * pItem,
                         size_t               keyLength,
                         size_t               bufferLength,
                         psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonHashMapInit( psonHashMap         * pHashMap,
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
bool psonHashMapInsert( psonHashMap        * pHashMap,
                        const void         * pKey,
                        size_t               keyLength,
                        const void         * pItem,
                        size_t               itemLength,
                        psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonHashMapRelease( psonHashMap        * pHashMap,
                         psonHashTxItem       * pHashItem,
                         psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonHashMapReplace( psonHashMap        * pHashMap,
                         const void         * pKey,
                         size_t               keyLength,
                         const void         * pItem,
                         size_t               itemLength,
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

