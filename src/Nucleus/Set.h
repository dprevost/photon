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

#ifndef PSON_SET_H
#define PSON_SET_H

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

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum psonSetEnum
{
   PSON_QUEUE_FIRST = 101,
   PSON_QUEUE_LAST  = 202
   
};

typedef enum psonSetEnum psonSetEnum;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psonSetItem
{
   psonTxStatus  txStatus;

   psonLinkNode node;

   /** Offset to the data definition for this specific item */
   ptrdiff_t  dataDefOffset;

   size_t dataLength;
   
   unsigned char data[1];

};

typedef struct psonSetItem psonSetItem;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psonSet
{
   /** Always first */
   struct psonMemObject memObject;

   /** Basic info for all leaves and branches of our tree. */
   struct psonTreeNode  nodeObject;

   /** The type of queue (as decided when psoCreateObject() was called). */
   enum psoObjectType queueType;

   /** Our own doubly-linked list, to hold the data. */
   psonLinkedList listOfElements;

   /** Offset to the data definition */
   ptrdiff_t  dataDefOffset;

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

typedef struct psonSet psonSet;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

PHOTON_ENGINE_EXPORT
bool psonSetCopy( psonSet            * pOldSet, 
                  psonSet            * pNewSet,
                  psonHashTxItem     * pHashItem,
                  const char         * origName,
                  psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
void psonSetEmpty( psonSet            * pSet,
                   psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
void psonSetFini( psonSet            * pSet,
                  psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonSetInit( psonSet             * pSet,
                  ptrdiff_t             parentOffset,
                  size_t                numberOfBlocks,
                  psonTxStatus        * pTxStatus,
                  uint32_t              origNameLength,
                  char                * origName,
                  ptrdiff_t             hashItemOffset,
                  psoObjectDefinition * pDefinition,
                  psonDataDefinition  * pDataDefinition,
                  psonSessionContext  * pContext );

PHOTON_ENGINE_EXPORT
bool psonSetGetFirst( psonSet            * pSet,
                      psonSetItem       ** ppIterator,
                      uint32_t             bufferLength,
                      psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonSetGetNext( psonSet            * pSet,
                     psonSetItem       ** ppIterator,
                     uint32_t             bufferLength,
                     psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonSetInsert( psonSet            * pSet,
                    const void         * pItem, 
                    uint32_t             length,
                    psonDataDefinition * pDefinition,
                    psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonSetRelease( psonSet            * pSet,
                     psonSetItem        * pSetItem,
                     psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
void psonSetStatus( psonSet      * pSet,
                    psoObjStatus * pStatus );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSON_SET_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

