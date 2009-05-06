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

#include "Nucleus/Cursor.h"
#include "Nucleus/LinkedList.h"
#include "Nucleus/Transaction.h"
#include "Nucleus/Folder.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonCursorEmpty( psonCursor         * pCursor,
                      psonSessionContext * pContext )
{
   psonCursorItem* pCursorItem = NULL;
   psonLinkNode * pNode = NULL;

   PSO_PRE_CONDITION( pCursor  != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pCursor->memObject.objType == PSON_IDENT_CURSOR );

   while ( psonLinkedListPeakFirst( &pCursor->listOfElements, &pNode ) ) {
      pCursorItem = (psonCursorItem *)
         ((char*)pNode - offsetof( psonCursorItem, node ));
      psonFree( &pCursor->memObject,
                (unsigned char *)pCursorItem,
                sizeof(psonCursorItem),
                pContext );
   }

}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonCursorFini( psonCursor         * pCursor,
                     psonSessionContext * pContext )
{
   PSO_PRE_CONDITION( pCursor  != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pCursor->memObject.objType == PSON_IDENT_CURSOR );

   psonLinkedListFini( &pCursor->listOfElements );
   psonMemObjectFini(  &pCursor->memObject, PSON_ALLOC_API_OBJ, pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonCursorGetFirst( psonCursor         * pCursor,
                         psonCursorItem    ** ppIterator,
                         uint32_t             bufferLength,
                         psonSessionContext * pContext )
{
   psonCursorItem* pCursorItem = NULL;
   psonLinkNode * pNode = NULL;
   bool okList;
   
   PSO_PRE_CONDITION( pCursor    != NULL );
   PSO_PRE_CONDITION( ppIterator != NULL );
   PSO_PRE_CONDITION( pContext   != NULL );
   PSO_PRE_CONDITION( pCursor->memObject.objType == PSON_IDENT_CURSOR );
   
   /* This call can only fail if the queue is empty. */
   okList = psonLinkedListPeakFirst( &pCursor->listOfElements, &pNode );
   
   if ( okList ) {
      pCursorItem = (psonCursorItem*) 
         ((char*)pNode - offsetof( psonCursorItem, node ));
         
      *ppIterator = pCursorItem;

      return true;
   }
   
   /* 
    * If we come here, there are no data items to retrieve. As 
    * long as we clearly say that the internal iterator is reset (in case a 
    * "Get Previous" is implemented later), we can just release the iterator
    * at this point.
    */
   *ppIterator = NULL;
   
   psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_IS_EMPTY );

   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonCursorGetNext( psonCursor         * pCursor,
                        psonCursorItem    ** ppIterator,
                        uint32_t             bufferLength,
                        psonSessionContext * pContext )
{
   psonCursorItem* pCursorItem = NULL;
   psonCursorItem* pOldItem = NULL;
   psonLinkNode * pNode = NULL;
   bool okList;
   
   PSO_PRE_CONDITION( pCursor    != NULL );
   PSO_PRE_CONDITION( ppIterator != NULL );
   PSO_PRE_CONDITION( pContext   != NULL );
   PSO_PRE_CONDITION( pCursor->memObject.objType == PSON_IDENT_CURSOR );
   
   pOldItem = (psonCursorItem*) *ppIterator;
   okList =  psonLinkedListPeakNext( &pCursor->listOfElements, 
                                     &pOldItem->node, 
                                     &pNode );
      
   if ( okList ) {
      pCursorItem = (psonCursorItem*)
         ((char*)pNode - offsetof( psonCursorItem, node ));
         
      /* 
       * If the transaction id of the item (to retrieve) is equal to the 
       * current transaction id AND the object is marked as deleted, we 
       * go to the next item.
       *
       * If the transaction id of the item (to retrieve) is NOT equal to the 
       * current transaction id AND the object is added... next!
       *
       * If the item is flagged as deleted and committed, it does not exists
       * from the API point of view.
       */

     *ppIterator = pCursorItem;
     
     return true;
   }
   
   /* 
    * If we come here, there are no additional data items to retrieve. As 
    * long as we clearly say that the internal iterator is reset (in case a 
    * "Get Previous" is implemented later), we can just release the iterator
    * at this point.
    */
   *ppIterator = NULL;
   
   psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_REACHED_THE_END );

   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonCursorInit( psonCursor          * pCursor,
                     ptrdiff_t             parentOffset,
                     size_t                numberOfBlocks,
                     psoObjectDefinition * pDefinition,
                     psonSessionContext  * pContext )
{
   psoErrors errcode;
   
   PSO_PRE_CONDITION( pCursor          != NULL );
   PSO_PRE_CONDITION( pContext        != NULL );
   PSO_PRE_CONDITION( pDefinition     != NULL );
   PSO_PRE_CONDITION( parentOffset   != PSON_NULL_OFFSET );
   PSO_PRE_CONDITION( numberOfBlocks > 0 );
   
   errcode = psonMemObjectInit( &pCursor->memObject, 
                                PSON_IDENT_CURSOR,
                                &pCursor->blockGroup,
                                numberOfBlocks );
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler,
                    g_psoErrorHandle,
                    errcode );
      return false;
   }

   psonLinkedListInit( &pCursor->listOfElements );

   pCursor->flags = pDefinition->flags;
   
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonCursorInsert( psonCursor         * pCursor,
                       void               * pItem,
                       int                  itemType,
                       psonSessionContext * pContext )
{
   psonCursorItem * pCursorItem;
   
   PSO_PRE_CONDITION( pCursor  != NULL );
   PSO_PRE_CONDITION( pItem    != NULL )
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pCursor->memObject.objType == PSON_IDENT_CURSOR );

   pCursorItem = (psonCursorItem *) psonMalloc( &pCursor->memObject,
                                                sizeof(psonCursorItem),
                                                pContext );
   if ( pCursorItem == NULL ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, 
                    PSO_NOT_ENOUGH_PSO_MEMORY );
      return false;
   }
   
   psonLinkNodeInit( &pCursorItem->node );
      
   pCursorItem->realItemfOffset = SET_OFFSET(pItem);
   pCursorItem->itemType = itemType;

   psonLinkedListPutLast( &pCursor->listOfElements,
                          &pCursorItem->node );
      
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonCursorSize( psonCursor * pCursor,
                     size_t     * pNumItems )
{
   PSO_PRE_CONDITION( pCursor   != NULL );
   PSO_PRE_CONDITION( pNumItems != NULL );
   PSO_PRE_CONDITION( pCursor->memObject.objType == PSON_IDENT_CURSOR );

   *pNumItems = pCursor->listOfElements.currentSize;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

