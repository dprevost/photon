/*
 * Copyright (C) 2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#ifndef PSON_SEQUENTIAL_SET_H
#define PSON_SEQUENTIAL_SET_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Nucleus/Engine.h"      
#include "Nucleus/MemoryObject.h"
#include "Nucleus/TreeNode.h"
#include "Nucleus/BlockGroup.h"
#include "Nucleus/LinkedList.h"
#include "Nucleus/TxStatus.h"
#include "Nucleus/Definitions.h"
#include "Nucleus/HashTx.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psonSequentialSet
{
   /** Always first */
   struct psonMemObject memObject;

   /** Basic info for all leaves and branches of our tree. */
   struct psonTreeNode  nodeObject;

   /** Offset to the data definition */
   ptrdiff_t  dataDefOffset;

   /** Offset to the key definition */
   ptrdiff_t keyDefOffset;

   /* Creation flags */
   uint32_t flags;
   
   /**
    * Number of valid items. Valid items are the number of items NOT counting
    * items that might be added (but not committed) - also, items which are
    * removed but not committed are counted as valid).
    */
   size_t numValidItems;

   ptrdiff_t latestVersion;

   ptrdiff_t editVersion;
   
   /** Variable size struct - always put at the end */
   struct psonBlockGroup blockGroup;

};

typedef struct psonSequentialSet psonSeqSet;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

PHOTON_ENGINE_EXPORT
bool psonSeqSetCopy( psonSeqSet        * pHashMap, 
                      psonSeqSet        * pNewHashMap,
                      psonHashTxItem     * pHashItem,
                      const char         * origName,
                      psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonSeqSetDelete( psonSeqSet        * pHashMap,
                        const void         * key,
                        uint32_t             keyLength, 
                        psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
void psonSeqSetEmpty( psonSeqSet        * pHashMap,
                       psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
void psonSeqSetFini( psonSeqSet        * pHashMap,
                      psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonSeqSetGet( psonSeqSet        * pHashMap,
                     const void         * pKey,
                     uint32_t             keyLength, 
                     psonHashItem      ** ppItem,
                     uint32_t             bufferLength,
                     psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonSeqSetGetFirst( psonSeqSet        * pHashMap,
                          psonSeqSetItem    * pItem,
                          uint32_t             keyLength,
                          uint32_t             bufferLength,
                          psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonSeqSetGetNext( psonSeqSet        * pHashMap,
                         psonSeqSetItem    * pItem,
                         uint32_t             keyLength,
                         uint32_t             bufferLength,
                         psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonSeqSetInit( psonSeqSet         * pHashMap,
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
bool psonSeqSetInsert( psonSeqSet        * pHashMap,
                        const void         * pKey,
                        uint32_t             keyLength,
                        const void         * pItem,
                        uint32_t             itemLength,
                        psonDataDefinition * pDefinition,
                        psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonSeqSetRelease( psonSeqSet        * pHashMap,
                         psonHashItem       * pHashItem,
                         psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonSeqSetReplace( psonSeqSet        * pHashMap,
                         const void         * pKey,
                         uint32_t             keyLength,
                         const void         * pItem,
                         uint32_t             itemLength,
                         psonDataDefinition * pDefinition,
                         psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
void psonSeqSetStatus( psonSeqSet  * pHashMap,
                        psoObjStatus * pStatus );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSON_SEQUENTIAL_SET_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
