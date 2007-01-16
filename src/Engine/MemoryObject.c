/*
 * Copyright (C) 2006-2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

#include "MemoryObject.h"
#include "PageGroup.h"
#include "MemoryAllocator.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Initialize a vdseMemObject struct. 
 *
 *
 * \param[in] pMemObj A pointer to the data struct we are initializing.
 * \param[in] objType The buffer used to store the message.
 * \param[in] objSize The size of the object struct, needed to calculate the
 *                    amount of free space left in the page(s).
 * \param[in] numPages The initial number of pages allocated to this object.
 *
 * \retval VDS_OK  No error found
 * \retval VDS_NOT_ENOUGH_RESOURCES Something went wrong in allocating 
 *                                  resources for the Process Lock.
 *
 * \pre \em pMemObj cannot be NULL.
 * \pre \em objType must be valid (greater than VDSE_IDENT_FIRST and less than 
 *          VDSE_IDENT_LAST).
 * \pre \em objSize must be greater than zero.
 * \pre \em numPages must be greater than zero.
 */

enum vdsErrors 
vdseMemObjectInit( vdseMemObject* pMemObj,
                   enum ObjectIdentifier objType,
                   size_t numPages )
{
   int errcode = 0;
   
   VDS_PRE_CONDITION( pMemObj != NULL );
   VDS_PRE_CONDITION( numPages > 0 );
   VDS_PRE_CONDITION( objType > VDSE_IDENT_FIRST && 
                      objType < VDSE_IDENT_LAST );

   /* In case InitProcessLock fails */
   pMemObj->objType = VDSE_IDENT_CLEAR;
   
   errcode =  vdscInitProcessLock( &pMemObj->lock );
   if ( errcode != 0 )
      return VDS_NOT_ENOUGH_RESOURCES;
   
   pMemObj->objType = objType;
   pMemObj->accessCounter = 0;
   vdseLinkedListInit( &pMemObj->listPageGroup );
   
   pMemObj->totalPages = numPages;
   
   return VDS_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Terminate access to (destroy) a vdseMemObject struct. 
 *
 *
 * \param[in] pMemObj A pointer to the data struct we are initializing.
 *
 * \pre \em pMemObj cannot be NULL.
 * \pre \em objType must be valid (greater than VDSE_IDENT_FIRST and less than 
 *          VDSE_IDENT_LAST).
 */

enum vdsErrors 
vdseMemObjectFini( vdseMemObject* pMemObj )
{
   VDS_PRE_CONDITION( pMemObj != NULL );
   VDS_PRE_CONDITION( pMemObj->objType > VDSE_IDENT_FIRST && 
                      pMemObj->objType < VDSE_IDENT_LAST );

   pMemObj->objType = VDSE_IDENT_CLEAR;

   pMemObj->accessCounter = 0;
   pMemObj->totalPages = 0;

   vdseLinkedListFini( &pMemObj->listPageGroup );

   if ( vdscFiniProcessLock( &pMemObj->lock ) != 0 )
      return VDS_NOT_ENOUGH_RESOURCES;
   return VDS_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

unsigned char* vdseMalloc( vdseMemObject*      pMemObj,
                           size_t              numBytes,
                           vdseSessionContext* pContext )
{
   size_t numBlocks, requestedBlocks, remainingBlocks;
   vdseFreeBufferNode *oldNode, *currentNode, *newNode;
   enum ListErrors errGroup, errNode;
   vdsePageGroup* oldGroup, *currentGroup;
   vdseLinkNode* dummy;
   unsigned char* ptr;
   size_t requestedPages, i;
   
   VDS_PRE_CONDITION( pMemObj  != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( numBytes > 0 );
   
   requestedBlocks = (numBytes-1)/VDSE_ALLOCATION_UNIT + 1;
   
   errGroup = vdseLinkedListPeakFirst( &pMemObj->listPageGroup,
                                       &dummy );
   while ( errGroup == LIST_OK )
   {
      currentGroup = (vdsePageGroup*)( 
         (unsigned char*)dummy + offsetof(vdsePageGroup,node));

      errNode = vdseLinkedListPeakFirst( &currentGroup->freeList, &dummy );
      while ( errNode == LIST_OK )
      {
         currentNode = (vdseFreeBufferNode*)( 
            (unsigned char*)dummy + offsetof(vdseFreeBufferNode,node));
         numBlocks = ((vdseFreeBufferNode*)currentNode)->numBlocks;
         if ( numBlocks >= requestedBlocks )
         {
            /* We got it */
            remainingBlocks = numBlocks - requestedBlocks;
            if ( remainingBlocks == 0 )
            {
               /* Remove the node from the list */
               vdseLinkedListRemoveItem( &currentGroup->freeList, 
                                         &currentNode->node );
            }
            else
            {
               newNode = (vdseFreeBufferNode*)
                         ((unsigned char*) currentNode + 
                         (requestedBlocks*VDSE_ALLOCATION_UNIT));
               newNode->numBlocks = remainingBlocks;
               vdseLinkedListReplaceItem( &currentGroup->freeList, 
                                          &currentNode->node, 
                                          &newNode->node );
               /*
                * Put the offset of the first free block on the last free 
                * block. This makes it simpler/faster to rejoin groups 
                * of free blocks. But only if there is more than one free
                * block.
                */
               if ( remainingBlocks > 1 )
               {
                  ptr = (unsigned char*) newNode + 
                     (remainingBlocks-1) * VDSE_ALLOCATION_UNIT; 
                  *((ptrdiff_t *)ptr) = SET_OFFSET(newNode);
               }
            }

            /* Set the bitmap */
            vdseSetBlocksAllocated( &currentGroup->bitmap, 
                                    SET_OFFSET(currentNode), 
                                    requestedBlocks*VDSE_ALLOCATION_UNIT );
                              
            return (unsigned char*) currentNode;
         }
   
         oldNode = currentNode;

         errNode = vdseLinkedListPeakNext( &currentGroup->freeList,
                                           &oldNode->node,
                                           &dummy );
      }
      
      oldGroup = currentGroup;
      errGroup = vdseLinkedListPeakNext( &pMemObj->listPageGroup,
                                         &oldGroup->node,
                                         &dummy );
   }

   /*
    * If we come here, it means that we did not find a free buffer of
    * the proper size. So we need to ad pages to the object.
    *  - find how many pages to get
    *  - get them
    *  - initialize them
    *  - alloc the buffer from them
    */
    
   i = ( sizeof(vdsePageGroup) - 1 ) / VDSE_ALLOCATION_UNIT + 1;
   requestedPages = (((requestedBlocks+i)*VDSE_ALLOCATION_UNIT - 1) >> VDSE_PAGE_SHIFT) + 1;
   /* We increment the size by 3%, if 3% is bigger than the request */
   i = 3 * pMemObj->totalPages / 100;
   if ( i < requestedPages )
      i = requestedPages;
   
   currentGroup = (vdsePageGroup*) vdseMallocPages( pContext->pAllocator,
                                                    i,
                                                    pContext );
   if ( currentGroup == NULL && i > requestedPages )
   {
      /* retry again with a smaller number */
      if ( vdscGetLastError( &pContext->errorHandler ) == 
         VDS_NOT_ENOUGH_MEMORY )
      {
         i = requestedPages;
         currentGroup = (vdsePageGroup*) vdseMallocPages( pContext->pAllocator,
                                                          i,
                                                          pContext );
      }
   }
   if ( currentGroup != NULL )
   {
      vdsePageGroupInit( currentGroup,
                         SET_OFFSET( currentGroup ),
                         i );
      /* Add the pageGroup to the list of groups of the memObject */
      vdseLinkedListPutLast( &pMemObj->listPageGroup, &currentGroup->node );
      pMemObj->totalPages += i;
      
      /* Allocate the memory */
      
      errNode = vdseLinkedListPeakFirst( &currentGroup->freeList, &dummy );
      if ( errNode == LIST_OK )
      {
         currentNode = (vdseFreeBufferNode*)( 
            (unsigned char*)dummy + offsetof(vdseFreeBufferNode,node));
         numBlocks = ((vdseFreeBufferNode*)currentNode)->numBlocks;
         remainingBlocks = numBlocks - requestedBlocks;
         
         if ( remainingBlocks == 0 )
         {
            /* Remove the node from the list */
            vdseLinkedListRemoveItem( &currentGroup->freeList, 
                                      &currentNode->node );
         }
         else
         {
            newNode = (vdseFreeBufferNode*)
                      ((unsigned char*) currentNode + 
                      (requestedBlocks*VDSE_ALLOCATION_UNIT));
            newNode->numBlocks = remainingBlocks;
            vdseLinkedListReplaceItem( &currentGroup->freeList, 
                                       &currentNode->node, 
                                       &newNode->node );
            /*
             * Put the offset of the first free block on the last free 
             * block. This makes it simpler/faster to rejoin groups 
             * of free blocks. But only if there is more than one free
             * block.
             */
            if ( remainingBlocks > 1 )
            {
               ptr = (unsigned char*) newNode + 
                  (remainingBlocks-1) * VDSE_ALLOCATION_UNIT; 
               *((ptrdiff_t *)ptr) = SET_OFFSET(newNode);
            }
         }

         /* Set the bitmap */
         vdseSetBlocksAllocated( &currentGroup->bitmap, 
                                 SET_OFFSET(currentNode), 
                                 requestedBlocks*VDSE_ALLOCATION_UNIT );
                              
         return (unsigned char*) currentNode;
      }
   }
   
   /** 
    * \todo Eventually, it might be a good idea to separate "no memory"
    * versus a lack of a chunk big enough to accomodate the # of requested
    * pages.
    */
   vdscSetError( &pContext->errorHandler, 
                 g_vdsErrorHandle, 
                 VDS_NOT_ENOUGH_MEMORY );

   return NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseFree( vdseMemObject*      pMemObj,
               unsigned char*      ptr, 
               size_t              numBytes,
               vdseSessionContext* pContext )
{
   enum ListErrors errGroup;
   vdsePageGroup* oldGroup, *currentGroup, *goodGroup = NULL;
   vdseLinkNode* dummy;
   bool otherBufferisFree = false;
   unsigned char* p;
   vdseFreeBufferNode* otherNode;
   ptrdiff_t offset;
   size_t numPages;
   
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pMemObj  != NULL );
   VDS_PRE_CONDITION( ptr      != NULL );
   VDS_PRE_CONDITION( numBytes > 0 );

   /*
    * Ajust numBytes so that it is a multiple of VDSE_ALLOCATION_UNIT,
    * without truncating it, of course...
    */
   numBytes = ((numBytes-1)/VDSE_ALLOCATION_UNIT+1)*VDSE_ALLOCATION_UNIT;
   
   /* Find to which pagegroup ptr belongs to */
   errGroup = vdseLinkedListPeakFirst( &pMemObj->listPageGroup,
                                       &dummy );
   while ( errGroup == LIST_OK )
   {
      currentGroup = (vdsePageGroup*)( 
         (unsigned char*)dummy + offsetof(vdsePageGroup,node));

      if ( ptr >= (unsigned char*)currentGroup && 
           ptr < (unsigned char*)currentGroup + (currentGroup->numPages << VDSE_PAGE_SHIFT) )
      {
         goodGroup = currentGroup;
         break;
      }
      oldGroup = currentGroup;
      errGroup = vdseLinkedListPeakNext( &pMemObj->listPageGroup,
                                         &oldGroup->node,
                                         &dummy );
   }
   VDS_PRE_CONDITION( goodGroup != NULL );

   /* 
    * Check if the block before the current block (ptr)
    * is in the freeList or not.
    */
   p = ptr - VDSE_ALLOCATION_UNIT;
   otherBufferisFree = vdseIsBlockFree( &goodGroup->bitmap, SET_OFFSET(p) );
   if ( otherBufferisFree )
   {
      /* Find the start of that free group of pages */
      if ( vdseIsBlockFree( &goodGroup->bitmap, 
         SET_OFFSET( ptr - 2*VDSE_ALLOCATION_UNIT ) ) )
      {
         /* The free group has more than one page */
         offset = *((ptrdiff_t*)p);
         p = GET_PTR(offset, unsigned char);
      }
      
      /*
       * The node is already in the linked list! All we need to do is
       * to modify the number of blocks that this node has.
       */
      ((vdseFreeBufferNode*)p)->numBlocks += (numBytes/VDSE_ALLOCATION_UNIT);
   }
   else
   {
      /*
       * The previous buffer is not free, so we need to insert our released
       * buffer as a new node in the list.
       */
      p = ptr; /* This is needed further down */
      ((vdseFreeBufferNode*)p)->numBlocks = numBytes/VDSE_ALLOCATION_UNIT;
      vdseLinkedListPutLast( &goodGroup->freeList, 
                             &((vdseFreeBufferNode*)p)->node );
   }

   /* 
    * Check if the page after the current group-of-pages-to-be-released
    * is in the freeList or not.
    *
    * Note: "p" at this point represents the node which contain our buffer
    *       and, possibly, the previous buffer if it was free.
    */
   otherNode = (vdseFreeBufferNode*)( ptr + numBytes);
   otherBufferisFree = vdseIsBlockFree( &goodGroup->bitmap, 
                                        SET_OFFSET(otherNode) );
   if ( otherBufferisFree )
   {
      ((vdseFreeBufferNode*)p)->numBlocks += otherNode->numBlocks;
      vdseLinkedListRemoveItem( &goodGroup->freeList, 
                                &otherNode->node );
      memset( otherNode, 0, sizeof(vdseFreeBufferNode) );
   }

   if ( ((vdseFreeBufferNode*)p)->numBlocks*VDSE_ALLOCATION_UNIT == 
      goodGroup->maxFreeBytes && goodGroup->isDeletable )
   {
      numPages = goodGroup->numPages;
      pMemObj->totalPages -= numPages;
      /* Remove the pageGroup to the list of groups of the memObject */
      vdseLinkedListRemoveItem( &pMemObj->listPageGroup, &goodGroup->node );

      vdsePageGroupFini( goodGroup );
      
      vdseFreePages( pContext->pAllocator,
                     goodGroup,
                     numPages,
                     pContext );
   }
   else
   {
      /* Set the bitmap */
      vdseSetBlocksFree( &goodGroup->bitmap, SET_OFFSET(ptr), numBytes );
   
      /*
       * Put the offset of the first free page on the last free page.
       * This makes it simpler/faster to rejoin groups of free pages. But 
       * only if the number of pages in the group > 1.
       */
      if ( ((vdseFreeBufferNode*)p)->numBlocks > 1 )
      {
         /* Warning - we reuse ptr here */
          ptr = p + 
             (((vdseFreeBufferNode*)p)->numBlocks-1) * VDSE_ALLOCATION_UNIT; 
          *((ptrdiff_t *)ptr) = SET_OFFSET(p);
      }
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

