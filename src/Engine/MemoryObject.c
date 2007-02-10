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
#include "BlockGroup.h"
#include "MemoryAllocator.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Initialize a vdseMemObject struct. 
 *
 *
 * \param[in] pMemObj A pointer to the data struct we are initializing.
 * \param[in] objType The buffer used to store the message.
 * \param[in] objSize The size of the object struct, needed to calculate the
 *                    amount of free space left in the Block(s).
 * \param[in] numBlocks The initial number of Blocks allocated to this object.
 *
 * \retval VDS_OK  No error found
 * \retval VDS_NOT_ENOUGH_RESOURCES Something went wrong in allocating 
 *                                  resources for the Process Lock.
 *
 * \pre \em pMemObj cannot be NULL.
 * \pre \em objType must be valid (greater than VDSE_IDENT_FIRST and less than 
 *          VDSE_IDENT_LAST).
 * \pre \em objSize must be greater than zero.
 * \pre \em numBlocks must be greater than zero.
 */

enum vdsErrors 
vdseMemObjectInit( vdseMemObject* pMemObj,
                   enum ObjectIdentifier objType,
                   size_t numBlocks )
{
   int errcode = 0;
   
   VDS_PRE_CONDITION( pMemObj != NULL );
   VDS_PRE_CONDITION( numBlocks > 0 );
   VDS_PRE_CONDITION( objType > VDSE_IDENT_FIRST && 
                      objType < VDSE_IDENT_LAST );

   /* In case InitProcessLock fails */
   pMemObj->objType = VDSE_IDENT_CLEAR;
   
   errcode =  vdscInitProcessLock( &pMemObj->lock );
   if ( errcode != 0 )
      return VDS_NOT_ENOUGH_RESOURCES;
   
   pMemObj->objType = objType;
   vdseLinkedListInit( &pMemObj->listBlockGroup );
   
   pMemObj->totalBlocks = numBlocks;
   
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

   pMemObj->totalBlocks = 0;

   vdseLinkedListFini( &pMemObj->listBlockGroup );

   if ( vdscFiniProcessLock( &pMemObj->lock ) != 0 )
      return VDS_NOT_ENOUGH_RESOURCES;
   return VDS_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

unsigned char* vdseMalloc( vdseMemObject*      pMemObj,
                           size_t              numBytes,
                           vdseSessionContext* pContext )
{
   size_t numChunks, requestedChunks, remainingChunks;
   vdseFreeBufferNode *oldNode, *currentNode, *newNode;
   enum ListErrors errGroup, errNode;
   vdseBlockGroup* oldGroup, *currentGroup;
   vdseLinkNode* dummy;
   unsigned char* ptr;
   size_t requestedBlocks, i;
   
   VDS_PRE_CONDITION( pMemObj  != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( numBytes > 0 );
   
   requestedChunks = (numBytes-1)/VDSE_ALLOCATION_UNIT + 1;
   
   errGroup = vdseLinkedListPeakFirst( &pMemObj->listBlockGroup,
                                       &dummy );
   while ( errGroup == LIST_OK )
   {
      currentGroup = (vdseBlockGroup*)( 
         (unsigned char*)dummy + offsetof(vdseBlockGroup,node));

      errNode = vdseLinkedListPeakFirst( &currentGroup->freeList, &dummy );
      while ( errNode == LIST_OK )
      {
         currentNode = (vdseFreeBufferNode*)( 
            (unsigned char*)dummy + offsetof(vdseFreeBufferNode,node));
         numChunks = ((vdseFreeBufferNode*)currentNode)->numBuffers;
         if ( numChunks >= requestedChunks )
         {
            /* We got it */
            remainingChunks = numChunks - requestedChunks;
            if ( remainingChunks == 0 )
            {
               /* Remove the node from the list */
               vdseLinkedListRemoveItem( &currentGroup->freeList, 
                                         &currentNode->node );
            }
            else
            {
               newNode = (vdseFreeBufferNode*)
                         ((unsigned char*) currentNode + 
                         (requestedChunks*VDSE_ALLOCATION_UNIT));
               newNode->numBuffers = remainingChunks;
               vdseLinkedListReplaceItem( &currentGroup->freeList, 
                                          &currentNode->node, 
                                          &newNode->node );
               /*
                * Put the offset of the first free chunk on the last free 
                * chunk. This makes it simpler/faster to rejoin groups 
                * of free chunks. But only if there is more than one free
                * chunk.
                */
               if ( remainingChunks > 1 )
               {
                  ptr = (unsigned char*) newNode + 
                     (remainingChunks-1) * VDSE_ALLOCATION_UNIT; 
                  *((ptrdiff_t *)ptr) = SET_OFFSET(newNode);
               }
            }

            /* Set the bitmap */
            vdseSetBufferAllocated( &currentGroup->bitmap, 
                                    SET_OFFSET(currentNode), 
                                    requestedChunks*VDSE_ALLOCATION_UNIT );
                              
            return (unsigned char*) currentNode;
         }
   
         oldNode = currentNode;

         errNode = vdseLinkedListPeakNext( &currentGroup->freeList,
                                           &oldNode->node,
                                           &dummy );
      }
      
      oldGroup = currentGroup;
      errGroup = vdseLinkedListPeakNext( &pMemObj->listBlockGroup,
                                         &oldGroup->node,
                                         &dummy );
   }

   /*
    * If we come here, it means that we did not find a free buffer of
    * the proper size. So we need to add blocks to the object.
    *  - find how many blocks to get
    *  - get them
    *  - initialize them
    *  - alloc the buffer from them
    */
    
   i = ( sizeof(vdseBlockGroup) - 1 ) / VDSE_ALLOCATION_UNIT + 1;
   requestedBlocks = (((requestedChunks+i)*VDSE_ALLOCATION_UNIT - 1) >> VDSE_BLOCK_SHIFT) + 1;
   /* We increment the size by 3%, if 3% is bigger than the request */
   i = 3 * pMemObj->totalBlocks / 100;
   if ( i < requestedBlocks )
      i = requestedBlocks;
   
   currentGroup = (vdseBlockGroup*) vdseMallocBlocks( pContext->pAllocator,
                                                    i,
                                                    pContext );
   if ( currentGroup == NULL && i > requestedBlocks )
   {
      /* retry again with a smaller number */
      if ( vdscGetLastError( &pContext->errorHandler ) == 
         VDS_NOT_ENOUGH_VDS_MEMORY )
      {
         i = requestedBlocks;
         currentGroup = (vdseBlockGroup*) vdseMallocBlocks( pContext->pAllocator,
                                                          i,
                                                          pContext );
      }
   }
   if ( currentGroup != NULL )
   {
      vdseBlockGroupInit( currentGroup,
                         SET_OFFSET( currentGroup ),
                         i );
      /* Add the blockGroup to the list of groups of the memObject */
      vdseLinkedListPutLast( &pMemObj->listBlockGroup, &currentGroup->node );
      pMemObj->totalBlocks += i;
      
      /* Allocate the memory */
      
      errNode = vdseLinkedListPeakFirst( &currentGroup->freeList, &dummy );
      if ( errNode == LIST_OK )
      {
         currentNode = (vdseFreeBufferNode*)( 
            (unsigned char*)dummy + offsetof(vdseFreeBufferNode,node));
         numChunks = ((vdseFreeBufferNode*)currentNode)->numBuffers;
         remainingChunks = numChunks - requestedChunks;
         
         if ( remainingChunks == 0 )
         {
            /* Remove the node from the list */
            vdseLinkedListRemoveItem( &currentGroup->freeList, 
                                      &currentNode->node );
         }
         else
         {
            newNode = (vdseFreeBufferNode*)
                      ((unsigned char*) currentNode + 
                      (requestedChunks*VDSE_ALLOCATION_UNIT));
            newNode->numBuffers = remainingChunks;
            vdseLinkedListReplaceItem( &currentGroup->freeList, 
                                       &currentNode->node, 
                                       &newNode->node );
            /*
             * Put the offset of the first free chunk on the last free 
             * chunk. This makes it simpler/faster to rejoin groups 
             * of free chunks. But only if there is more than one free
             * chunk.
             */
            if ( remainingChunks > 1 )
            {
               ptr = (unsigned char*) newNode + 
                  (remainingChunks-1) * VDSE_ALLOCATION_UNIT; 
               *((ptrdiff_t *)ptr) = SET_OFFSET(newNode);
            }
         }

         /* Set the bitmap */
         vdseSetBufferAllocated( &currentGroup->bitmap, 
                                 SET_OFFSET(currentNode), 
                                 requestedChunks*VDSE_ALLOCATION_UNIT );
                              
         return (unsigned char*) currentNode;
      }
   }
   
   /** 
    * \todo Eventually, it might be a good idea to separate "no memory"
    * versus a lack of a chunk big enough to accomodate the # of requested
    * blocks.
    */
   vdscSetError( &pContext->errorHandler, 
                 g_vdsErrorHandle, 
                 VDS_NOT_ENOUGH_VDS_MEMORY );

   return NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseFree( vdseMemObject*      pMemObj,
               unsigned char*      ptr, 
               size_t              numBytes,
               vdseSessionContext* pContext )
{
   enum ListErrors errGroup;
   vdseBlockGroup* oldGroup, *currentGroup, *goodGroup = NULL;
   vdseLinkNode* dummy;
   bool otherBufferisFree = false;
   unsigned char* p;
   vdseFreeBufferNode* otherNode;
   ptrdiff_t offset;
   size_t numBlocks;
   
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pMemObj  != NULL );
   VDS_PRE_CONDITION( ptr      != NULL );
   VDS_PRE_CONDITION( numBytes > 0 );

   /*
    * Ajust numBytes so that it is a multiple of VDSE_ALLOCATION_UNIT,
    * without truncating it, of course...
    */
   numBytes = ((numBytes-1)/VDSE_ALLOCATION_UNIT+1)*VDSE_ALLOCATION_UNIT;
   
   /* Find to which blockgroup ptr belongs to */
   errGroup = vdseLinkedListPeakFirst( &pMemObj->listBlockGroup,
                                       &dummy );
   while ( errGroup == LIST_OK )
   {
      currentGroup = (vdseBlockGroup*)( 
         (unsigned char*)dummy + offsetof(vdseBlockGroup,node));

      if ( ptr >= (unsigned char*)currentGroup && 
           ptr < (unsigned char*)currentGroup + (currentGroup->numBlocks << VDSE_BLOCK_SHIFT) )
      {
         goodGroup = currentGroup;
         break;
      }
      oldGroup = currentGroup;
      errGroup = vdseLinkedListPeakNext( &pMemObj->listBlockGroup,
                                         &oldGroup->node,
                                         &dummy );
   }
   VDS_PRE_CONDITION( goodGroup != NULL );

   /* 
    * Check if the chunk before the current chunk (ptr)
    * is in the freeList or not.
    */
   p = ptr - VDSE_ALLOCATION_UNIT;
   otherBufferisFree = vdseIsBufferFree( &goodGroup->bitmap, SET_OFFSET(p) );
   if ( otherBufferisFree )
   {
      /* Find the start of that free group of chunks */
      if ( vdseIsBufferFree( &goodGroup->bitmap, 
         SET_OFFSET( ptr - 2*VDSE_ALLOCATION_UNIT ) ) )
      {
         /* The free group has more than one chunk */
         offset = *((ptrdiff_t*)p);
         p = GET_PTR(offset, unsigned char);
      }
      
      /*
       * The node is already in the linked list! All we need to do is
       * to modify the number of chunks that this node has.
       */
      ((vdseFreeBufferNode*)p)->numBuffers += (numBytes/VDSE_ALLOCATION_UNIT);
   }
   else
   {
      /*
       * The previous buffer is not free, so we need to insert our released
       * buffer as a new node in the list.
       */
      p = ptr; /* This is needed further down */
      ((vdseFreeBufferNode*)p)->numBuffers = numBytes/VDSE_ALLOCATION_UNIT;
      vdseLinkedListPutLast( &goodGroup->freeList, 
                             &((vdseFreeBufferNode*)p)->node );
   }

   /* 
    * Check if the chunk after the current group-of-chunks-to-be-released
    * is in the freeList or not.
    *
    * Note: "p" at this point represents the node which contain our buffer
    *       and, possibly, the previous buffer if it was free.
    */
   otherNode = (vdseFreeBufferNode*)( ptr + numBytes);
   otherBufferisFree = vdseIsBufferFree( &goodGroup->bitmap, 
                                         SET_OFFSET(otherNode) );
   if ( otherBufferisFree )
   {
      ((vdseFreeBufferNode*)p)->numBuffers += otherNode->numBuffers;
      vdseLinkedListRemoveItem( &goodGroup->freeList, 
                                &otherNode->node );
      memset( otherNode, 0, sizeof(vdseFreeBufferNode) );
   }

   if ( ((vdseFreeBufferNode*)p)->numBuffers*VDSE_ALLOCATION_UNIT == 
      goodGroup->maxFreeBytes && goodGroup->isDeletable )
   {
      numBlocks = goodGroup->numBlocks;
      pMemObj->totalBlocks -= numBlocks;
      /* Remove the blockGroup to the list of groups of the memObject */
      vdseLinkedListRemoveItem( &pMemObj->listBlockGroup, &goodGroup->node );

      vdseBlockGroupFini( goodGroup );
      
      vdseFreeBlocks( pContext->pAllocator,
                      goodGroup,
                      numBlocks,
                      pContext );
   }
   else
   {
      /* Set the bitmap */
      vdseSetBufferFree( &goodGroup->bitmap, SET_OFFSET(ptr), numBytes );
   
      /*
       * Put the offset of the first free block on the last free block.
       * This makes it simpler/faster to rejoin groups of free blocks. But 
       * only if the number of blocks in the group > 1.
       */
      if ( ((vdseFreeBufferNode*)p)->numBuffers > 1 )
      {
         /* Warning - we reuse ptr here */
          ptr = p + 
             (((vdseFreeBufferNode*)p)->numBuffers-1) * VDSE_ALLOCATION_UNIT; 
          *((ptrdiff_t *)ptr) = SET_OFFSET(p);
      }
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

