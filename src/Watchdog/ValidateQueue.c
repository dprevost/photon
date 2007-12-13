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

#include "Common/Common.h"
#include "Watchdog/ValidateQueue.h"
#include "Engine/Queue.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdswResetCountersList( struct vdseQueue * pQueue, int verbose, int spaces )
{
   vdseTxStatus * txItemStatus;
   enum ListErrors listErrCode;
   vdseLinkNode * pNode = NULL;
   vdseQueueItem* pQueueItem = NULL;
   
   listErrCode = vdseLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
   while ( listErrCode == LIST_OK )
   {
      pQueueItem = (vdseQueueItem*) 
         ((char*)pNode - offsetof( vdseQueueItem, node ));
      txItemStatus = &pQueueItem->txStatus;
      txItemStatus->usageCounter = 0;

      listErrCode =  vdseLinkedListPeakNext( &pQueue->listOfElements, 
                                             pNode, 
                                             &pNode );
   }

   if ( listErrCode == LIST_END_OF_LIST || listErrCode == LIST_EMPTY )
      return 0;
   
   fprintf( stderr, "Abnormal error in list, list error code = %d\n", 
      listErrCode );
   return -1;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum vdswValidation 
vdswValidateQueue( struct vdseQueue * pQueue, int verbose, int spaces )
{
   vdseTxStatus * txQueueStatus;
   int rc;
   
   if ( verbose )
   {
      fprintf( stderr, "\n" );
   }
   
   GET_PTR( txQueueStatus, pQueue->nodeObject.txStatusOffset, vdseTxStatus );

   if ( txQueueStatus->txOffset != NULL_OFFSET )
   {
      /*
       * So we have an interrupted transaction. What kind? 
       *   FLAG                 ACTION          Comment
       *   0                    remove object   Added object non-committed
       *   MARKED_AS_DESTROYED  reset txStatus  
       *   REMOVE_IS_COMMITTED  remove object
       *
       * Action is the equivalent of what a rollback would do.
       */
      if ( txQueueStatus->statusFlag == 0 ||
         txQueueStatus->statusFlag == VDSE_REMOVE_IS_COMMITTED )
      {
         fprintf( stderr, "Object is removed\n" );
         return VDSW_DELETE_OBJECT;
      }
      txQueueStatus->txOffset = NULL_OFFSET;
      txQueueStatus->statusFlag = 0;
   }
   
   txQueueStatus->usageCounter = 0;
   txQueueStatus->parentCounter = 0;
   pQueue->nodeObject.txCounter = 0;

   rc = vdswResetCountersList( pQueue, verbose, spaces );
   
   return VDSW_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
