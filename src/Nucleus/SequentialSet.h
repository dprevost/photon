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

struct psonSeqSetItem
{
   size_t dummy;
};

typedef struct psonSeqSetItem psonSeqSetItem;

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
bool psonSeqSetCopy( psonSeqSet         * pSeqSet, 
                     psonSeqSet         * pNewSeqSet,
                     psonHashTxItem     * pHashItem,
                     const char         * origName,
                     psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonSeqSetDelete( psonSeqSet         * pSeqSet,
                       const void         * key,
                       uint32_t             keyLength, 
                       psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
void psonSeqSetEmpty( psonSeqSet         * pSeqSet,
                      psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
void psonSeqSetFini( psonSeqSet         * pSeqSet,
                     psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonSeqSetGet( psonSeqSet         * pSeqSet,
                    const void         * pKey,
                    uint32_t             keyLength, 
                    psonSeqSetItem    ** ppItem,
                    uint32_t             bufferLength,
                    psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonSeqSetGetFirst( psonSeqSet         * pSeqSet,
                         psonSeqSetItem     * pItem,
                         uint32_t             keyLength,
                         uint32_t             bufferLength,
                         psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonSeqSetGetNext( psonSeqSet         * pSeqSet,
                        psonSeqSetItem     * pItem,
                        uint32_t             keyLength,
                        uint32_t             bufferLength,
                        psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonSeqSetInit( psonSeqSet          * pSeqSet,
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
bool psonSeqSetInsert( psonSeqSet         * pSeqSet,
                       const void         * pKey,
                       uint32_t             keyLength,
                       const void         * pItem,
                       uint32_t             itemLength,
                       psonDataDefinition * pDefinition,
                       psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonSeqSetRelease( psonSeqSet         * pSeqSet,
                        psonSeqSetItem     * pSeqSetItem,
                        psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonSeqSetReplace( psonSeqSet         * pSeqSet,
                        const void         * pKey,
                        uint32_t             keyLength,
                        const void         * pItem,
                        uint32_t             itemLength,
                        psonDataDefinition * pDefinition,
                        psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
void psonSeqSetStatus( psonSeqSet   * pSeqSet,
                       psoObjStatus * pStatus );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSON_SEQUENTIAL_SET_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
