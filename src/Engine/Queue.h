/*
 * Copyright (C) 2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef VDSE_QUEUE_H
#define VDSE_QUEUE_H

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

typedef enum vdseQueueEnum
{
   VDSE_QUEUE_FIRST = 101,
   VDSE_QUEUE_LAST  = 202
   
} vdseQueueEnum;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct vdseQueueItem
{
   vdseTxStatus  txStatus;

   vdseLinkNode node;

   size_t dataLength;
   
   unsigned char data[1];

} vdseQueueItem;
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct vdseQueue
{
   /** Always first */
   struct vdseMemObject memObject;

   /** Basic info for all leaves and branches of our tree. */
   struct vdseTreeNode  nodeObject;

   /** The type of queue (as decided when vdsCreateObject() was called). */
   enum vdsObjectType queueType;

   /** Our own doubly-linked list, to hold the data. */
   vdseLinkedList listOfElements;

   /**
    * Number of valid items. Valid items are the number of items NOT counting
    * items that might be added (but not committed) - also, items which are
    * removed but not committed are counted as valid).
    */
   size_t numValidItems;

   /** Variable size struct - always put at the end */
   struct vdseBlockGroup blockGroup;

} vdseQueue;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

VDSF_ENGINE_EXPORT
int vdseQueueInit( vdseQueue          * pQueue,
                   ptrdiff_t            parentOffset,
                   size_t               numberOfBlocks,
                   vdseTxStatus       * pTxStatus,
                   size_t               origNameLength,
                   vdsChar_T          * origName,
                   ptrdiff_t            keyOffset,
                   vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
void vdseQueueFini( vdseQueue          * pQueue,
                    vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
int vdseQueueInsert( vdseQueue          * pQueue,
                     const void         * pItem, 
                     size_t               length,
                     enum vdseQueueEnum   firstOrLast,
                     vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
int vdseQueueRemove( vdseQueue          * pQueue,
                     vdseQueueItem     ** ppQueueItem,
                     enum vdseQueueEnum   firstOrLast,
                     vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
int vdseQueueGet( vdseQueue          * pQueue,
                  unsigned int         flag,
                  vdseQueueItem     ** ppIterator,
                  vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
int vdseQueueRelease( vdseQueue          * pQueue,
                      vdseQueueItem      * pQueueItem,
                      vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
int vdseQueueStatus( vdseQueue          * pQueue,
                     size_t             * pNumValidItems,
                     size_t             * pNumTotalItems,
                     vdseSessionContext * pContext  );

VDSF_ENGINE_EXPORT
void vdseQueueCommitAdd( vdseQueue * pQueue, 
                         ptrdiff_t   itemOffset );

VDSF_ENGINE_EXPORT
void vdseQueueRollbackAdd( vdseQueue          * pQueue, 
                           ptrdiff_t            itemOffset,
                           vdseSessionContext * pContext  );

VDSF_ENGINE_EXPORT
void vdseQueueCommitRemove( vdseQueue          * pQueue, 
                            ptrdiff_t            itemOffset,
                            vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
void vdseQueueRollbackRemove( vdseQueue * pQueue, 
                              ptrdiff_t   itemOffset );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSE_QUEUE_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
