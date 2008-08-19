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

#ifndef PSN_QUEUE_H
#define PSN_QUEUE_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Engine/Engine.h"      
#include "Engine/MemoryObject.h"
#include "Engine/TreeNode.h"
#include "Engine/BlockGroup.h"
#include "Engine/LinkedList.h"
#include "Engine/TxStatus.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum psnQueueEnum
{
   PSN_QUEUE_FIRST = 101,
   PSN_QUEUE_LAST  = 202
   
};

typedef enum psnQueueEnum psnQueueEnum;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psnQueueItem
{
   psnTxStatus  txStatus;

   psnLinkNode node;

   size_t dataLength;
   
   unsigned char data[1];

};

typedef struct psnQueueItem psnQueueItem;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psnQueue
{
   /** Always first */
   struct psnMemObject memObject;

   /** Basic info for all leaves and branches of our tree. */
   struct psnTreeNode  nodeObject;

   /** The type of queue (as decided when vdsCreateObject() was called). */
   enum vdsObjectType queueType;

   /** Our own doubly-linked list, to hold the data. */
   psnLinkedList listOfElements;

   /** Offset to the data definition */
   ptrdiff_t  dataDefOffset;

   uint16_t numFields;

   /**
    * Number of valid items. Valid items are the number of items NOT counting
    * items that might be added (but not committed) - also, items which are
    * removed but not committed are counted as valid).
    */
   size_t numValidItems;

   /** Variable size struct - always put at the end */
   struct psnBlockGroup blockGroup;

};

typedef struct psnQueue psnQueue;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

VDSF_ENGINE_EXPORT
bool psnQueueInit( psnQueue           * pQueue,
                    ptrdiff_t             parentOffset,
                    size_t                numberOfBlocks,
                    psnTxStatus        * pTxStatus,
                    size_t                origNameLength,
                    char                * origName,
                    ptrdiff_t             hashItemOffset,
                    vdsObjectDefinition * pDefinition,
                    psnSessionContext  * pContext );

VDSF_ENGINE_EXPORT
void psnQueueFini( psnQueue          * pQueue,
                    psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool psnQueueInsert( psnQueue          * pQueue,
                      const void         * pItem, 
                      size_t               length,
                      enum psnQueueEnum   firstOrLast,
                      psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool psnQueueInsertNow( psnQueue          * pQueue,
                         const void         * pItem, 
                         size_t               length,
                         enum psnQueueEnum   firstOrLast,
                         psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool psnQueueRemove( psnQueue          * pQueue,
                      psnQueueItem     ** ppQueueItem,
                      enum psnQueueEnum   firstOrLast,
                      size_t               bufferLength,
                      psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool psnQueueGet( psnQueue          * pQueue,
                   unsigned int         flag,
                   psnQueueItem     ** ppIterator,
                   size_t               bufferLength,
                   psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool psnQueueRelease( psnQueue          * pQueue,
                       psnQueueItem      * pQueueItem,
                       psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
void psnQueueCommitAdd( psnQueue * pQueue, 
                         ptrdiff_t   itemOffset );

VDSF_ENGINE_EXPORT
void psnQueueRollbackAdd( psnQueue          * pQueue, 
                           ptrdiff_t            itemOffset,
                           psnSessionContext * pContext  );

VDSF_ENGINE_EXPORT
void psnQueueCommitRemove( psnQueue          * pQueue, 
                            ptrdiff_t            itemOffset,
                            psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
void psnQueueRollbackRemove( psnQueue * pQueue, 
                              ptrdiff_t   itemOffset );

VDSF_ENGINE_EXPORT
void psnQueueStatus( psnQueue    * pQueue,
                      vdsObjStatus * pStatus );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSN_QUEUE_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

