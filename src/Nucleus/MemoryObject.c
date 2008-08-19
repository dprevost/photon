/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of photon (photonsoftware.org).
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
 * Initialize a psnMemObject struct. This will also initialize the lock
 * itself (each basic memory object has a lock), the initial group of
 * blocks struct representing the memory allocated for the object and 
 * the linked list of all such groups of blocks that might be allocated 
 * in the future, as needed.
 *
 * \param[in] pMemObj A pointer to the data struct we are initializing.
 * \param[in] objType The buffer used to store the message.
 * \param[in] pGroup  A pointer to the psnBlockGroup struct.
 * \param[in] numBlocks The initial number of Blocks allocated to this object.
 *
 * \retval VDS_OK  No error found
 * \retval VDS_NOT_ENOUGH_RESOURCES Something went wrong in allocating 
 *                                  resources for the Process Lock.
 *
 * \pre \em pMemObj cannot be NULL.
 * \pre \em objType must be valid (greater than PSN_IDENT_FIRST and less than 
 *          PSN_IDENT_LAST).
 * \pre \em pGroup cannot be NULL.
 * \pre \em numBlocks must be greater than zero.
 */

enum vdsErrors 
psnMemObjectInit( psnMemObject   * pMemObj,
                   psnMemObjIdent   objType,
                   psnBlockGroup  * pGroup,
                   size_t            numBlocks )
{
   bool ok;
   
   VDS_PRE_CONDITION( pMemObj != NULL );
   VDS_PRE_CONDITION( pGroup  != NULL );
   VDS_PRE_CONDITION( numBlocks > 0 );
   VDS_PRE_CONDITION( objType > PSN_IDENT_FIRST && 
                      objType < PSN_IDENT_LAST );

   /* In case InitProcessLock fails */
   pMemObj->objType = PSN_IDENT_CLEAR;
   
   ok =  pscInitProcessLock( &pMemObj->lock );
   VDS_POST_CONDITION( ok == true || ok == false );
   /*
    * The only possible error is a lack of resources when using semaphores, 
    * i.e. when the number of semaphores is greater than SEM_VALUE_MAX.
    */
   if ( ! ok ) return VDS_NOT_ENOUGH_RESOURCES;
   
   pMemObj->objType = objType;
   psnLinkedListInit( &pMemObj->listBlockGroup );
   
   psnBlockGroupInit( pGroup,
                       SET_OFFSET(pMemObj),
                       numBlocks,
                       objType );
   
   /* Add the blockGroup to the list of groups of the memObject */
   psnLinkNodeInit( &pGroup->node );
   psnLinkedListPutFirst( &pMemObj->listBlockGroup, 
                           &pGroup->node );
                           
   pMemObj->totalBlocks = numBlocks;
   
   return VDS_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Terminate access to (destroy) a psnMemObject struct. It will release
 * groups of blocks of memory associated with this object.
 *
 *
 * \param[in] pMemObj A pointer to the data struct we are initializing.
 * \param[in] pContext A pointer to our session information
 *
 * \pre \em pMemObj cannot be NULL.
 * \pre \em pContext cannot be NULL.
 * \pre \em objType must be valid (greater than PSN_IDENT_FIRST and less than 
 *          PSN_IDENT_LAST).
 */

enum vdsErrors 
psnMemObjectFini( psnMemObject      * pMemObj,
                   psnAllocTypeEnum    allocType,
                   psnSessionContext * pContext )
{
   psnLinkNode* firstNode, *dummy;
   bool ok;
   psnBlockGroup* pGroup;

   VDS_PRE_CONDITION( pMemObj  != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pMemObj->objType > PSN_IDENT_FIRST && 
                      pMemObj->objType < PSN_IDENT_LAST );

   /*
    * We retrieve the first node - and leave it alone.
    */
   ok = psnLinkedListGetFirst( &pMemObj->listBlockGroup, &firstNode );
   VDS_PRE_CONDITION( ok );

   while ( psnLinkedListGetFirst(&pMemObj->listBlockGroup, &dummy) ) {
      pGroup = (psnBlockGroup*)( 
         (unsigned char*)dummy - offsetof(psnBlockGroup,node));

      psnFreeBlocks( pContext->pAllocator, 
                      PSN_ALLOC_ANY,
                      (unsigned char*)pGroup, 
                      pGroup->numBlocks,
                      pContext );
   }
                      
   pMemObj->objType = PSN_IDENT_CLEAR;

   pMemObj->totalBlocks = 0;

   psnLinkedListFini( &pMemObj->listBlockGroup );

   if ( ! pscFiniProcessLock( &pMemObj->lock ) ) {
      return VDS_SEM_DESTROY_ERROR;
   }
   
   pGroup = (psnBlockGroup*)(
      (unsigned char*)firstNode - offsetof(psnBlockGroup,node));

   /*
    * This must be the last access to the memory object.
    */
   psnFreeBlocks( pContext->pAllocator, 
                   allocType,
                   (unsigned char*)pMemObj, 
                   pGroup->numBlocks,
                   pContext );
   
   return VDS_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

unsigned char* psnMalloc( psnMemObject*      pMemObj,
                           size_t              numBytes,
                           psnSessionContext* pContext )
{
   size_t numChunks, requestedChunks, remainingChunks;
   psnFreeBufferNode *oldNode, *currentNode, *newNode;
   psnBlockGroup* oldGroup, *currentGroup;
   psnLinkNode* dummy;
   unsigned char* ptr;
   size_t requestedBlocks, i;
   bool okGroup, okChunk;
   
   VDS_PRE_CONDITION( pMemObj  != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( numBytes > 0 );
   
   requestedChunks = (numBytes-1)/PSN_ALLOCATION_UNIT + 1;
   
   /*
    * The first loop is done on the list of block groups of the current 
    * memory object.
    */
   okGroup = psnLinkedListPeakFirst( &pMemObj->listBlockGroup, &dummy );
   while ( okGroup ) {
      currentGroup = (psnBlockGroup*)( 
         (unsigned char*)dummy - offsetof(psnBlockGroup,node));

      /*
       * No point in trying if the amount of free space is less than
       * the requested size for that group.
       */
      if ( currentGroup->freeBytes > requestedChunks *PSN_ALLOCATION_UNIT ) {
         /*
          * This second loop is done on the list of free chunks in the 
          * current block group.
          */
         okChunk = psnLinkedListPeakFirst( &currentGroup->freeList, &dummy );
         while ( okChunk ) {
            currentNode = (psnFreeBufferNode*)( 
               (unsigned char*)dummy - offsetof(psnFreeBufferNode,node));
            numChunks = ((psnFreeBufferNode*)currentNode)->numBuffers;
            if ( numChunks >= requestedChunks ) {
               /* We got it */
               remainingChunks = numChunks - requestedChunks;
               if ( remainingChunks == 0 ) {
                  /* Remove the chunk from the list */
                  psnLinkedListRemoveItem( &currentGroup->freeList, 
                                            &currentNode->node );
               }
               else {
                  newNode = (psnFreeBufferNode*)
                            ((unsigned char*) currentNode + 
                            (requestedChunks*PSN_ALLOCATION_UNIT));
                  newNode->numBuffers = remainingChunks;
                  psnLinkNodeInit( &newNode->node );
                  psnLinkedListReplaceItem( &currentGroup->freeList, 
                                             &currentNode->node, 
                                             &newNode->node );
                  /*
                   * Put the offset of the first free chunk on the last free 
                   * chunk. This makes it simpler/faster to rejoin groups 
                   * of free chunks. But only if there is more than one free
                   * chunk.
                   */
                  if ( remainingChunks > 1 ) {
                     ptr = (unsigned char*) newNode + 
                        (remainingChunks-1) * PSN_ALLOCATION_UNIT; 
                     *((ptrdiff_t *)ptr) = SET_OFFSET(newNode);
                  }
               }

               currentGroup->freeBytes -= requestedChunks*PSN_ALLOCATION_UNIT;
               /* Set the bitmap of chunks of the current block group. */
               psnSetBufferAllocated( &currentGroup->bitmap, 
                                       SET_OFFSET(currentNode), 
                                       requestedChunks*PSN_ALLOCATION_UNIT );
                              
               return (unsigned char*) currentNode;
            } /* end if of numChunks >= requestedChunks */
   
            oldNode = currentNode;
            okChunk = psnLinkedListPeakNext( &currentGroup->freeList,
                                              &oldNode->node,
                                              &dummy );
         } /* end of second loop on chunks */
      } /* end of test on freeBytes */
      
      oldGroup = currentGroup;
      okGroup = psnLinkedListPeakNext( &pMemObj->listBlockGroup,
                                        &oldGroup->node,
                                        &dummy );
   } /* end of first loop on block groups */

   /*
    * If we come here, it means that we did not find a free buffer of
    * the proper size. So we need to add blocks to the object.
    *  - find how many blocks to get
    *  - get them
    *  - initialize them
    *  - alloc the buffer the caller requested from that new chunk of memory.
    */
    
   i = ( sizeof(psnBlockGroup) - 1 ) / PSN_ALLOCATION_UNIT + 1;
   requestedBlocks = (((requestedChunks+i)*PSN_ALLOCATION_UNIT - 1) >> PSN_BLOCK_SHIFT) + 1;
   /* We increment the size by 3%, if 3% is bigger than the request */
   i = 3 * pMemObj->totalBlocks / 100;
   if ( i < requestedBlocks ) i = requestedBlocks;
   
   currentGroup = (psnBlockGroup*) psnMallocBlocks( pContext->pAllocator,
                                                      PSN_ALLOC_ANY,
                                                      i,
                                                      pContext );
   if ( currentGroup == NULL && i > requestedBlocks ) {
      /* 
       * We retry again with a smaller number, the minimal increase needed
       * to satisfy the original request.
       */
      if ( pscGetLastError( &pContext->errorHandler ) == 
         VDS_NOT_ENOUGH_VDS_MEMORY ) {

         i = requestedBlocks;
         currentGroup = (psnBlockGroup*) psnMallocBlocks( pContext->pAllocator,
                                                            PSN_ALLOC_ANY,
                                                            i,
                                                            pContext );
      }
   }
   if ( currentGroup != NULL ) {
      psnBlockGroupInit( currentGroup, 
                          SET_OFFSET( currentGroup ), 
                          i, 
                          pMemObj->objType );
      /* Add the blockGroup to the list of groups of the memObject */
      psnLinkNodeInit( &currentGroup->node );
      psnLinkedListPutLast( &pMemObj->listBlockGroup, &currentGroup->node );
      pMemObj->totalBlocks += i;
      
      /* Allocate the memory (the chunks) needed to satisfy the request. */
      okChunk = psnLinkedListPeakFirst( &currentGroup->freeList, &dummy );
      if ( okChunk ) {
         currentNode = (psnFreeBufferNode*)( 
            (unsigned char*)dummy + offsetof(psnFreeBufferNode,node));
         numChunks = ((psnFreeBufferNode*)currentNode)->numBuffers;
         remainingChunks = numChunks - requestedChunks;
         
         if ( remainingChunks == 0 ) {
            /* Remove the node from the list */
            psnLinkedListRemoveItem( &currentGroup->freeList, 
                                      &currentNode->node );
         }
         else {
            newNode = (psnFreeBufferNode*)
                      ((unsigned char*) currentNode + 
                      (requestedChunks*PSN_ALLOCATION_UNIT));
            newNode->numBuffers = remainingChunks;
            psnLinkNodeInit( &newNode->node );
            psnLinkedListReplaceItem( &currentGroup->freeList, 
                                       &currentNode->node, 
                                       &newNode->node );
            /*
             * Put the offset of the first free chunk on the last free 
             * chunk. This makes it simpler/faster to rejoin groups 
             * of free chunks. But only if there is more than one free
             * chunk.
             */
            if ( remainingChunks > 1 ) {
               ptr = (unsigned char*) newNode + 
                  (remainingChunks-1) * PSN_ALLOCATION_UNIT; 
               *((ptrdiff_t *)ptr) = SET_OFFSET(newNode);
            }
         }

         currentGroup->freeBytes -= requestedChunks*PSN_ALLOCATION_UNIT;
         /* Set the bitmap */
         psnSetBufferAllocated( &currentGroup->bitmap, 
                                 SET_OFFSET(currentNode), 
                                 requestedChunks*PSN_ALLOCATION_UNIT );
                              
         return (unsigned char*) currentNode;
      }
   }
   
   /** 
    * \todo Eventually, it might be a good idea to separate "no memory"
    * versus a lack of a chunk big enough to accomodate the # of requested
    * blocks.
    */
   pscSetError( &pContext->errorHandler, 
                 g_vdsErrorHandle, 
                 VDS_NOT_ENOUGH_VDS_MEMORY );

   return NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psnFree( psnMemObject*      pMemObj,
               unsigned char*      ptr, 
               size_t              numBytes,
               psnSessionContext* pContext )
{
   psnBlockGroup* oldGroup, *currentGroup, *goodGroup = NULL;
   psnLinkNode* dummy;
   bool otherBufferisFree = false;
   unsigned char* p;
   psnFreeBufferNode* otherNode;
   ptrdiff_t offset;
   size_t numBlocks;
   bool ok;
   
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pMemObj  != NULL );
   VDS_PRE_CONDITION( ptr      != NULL );
   VDS_PRE_CONDITION( numBytes > 0 );

   /*
    * Ajust numBytes so that it is a multiple of PSN_ALLOCATION_UNIT,
    * without truncating it, of course...
    */
   numBytes = ((numBytes-1)/PSN_ALLOCATION_UNIT+1)*PSN_ALLOCATION_UNIT;
   
   /* Find to which blockgroup ptr belongs to */
   ok = psnLinkedListPeakFirst( &pMemObj->listBlockGroup, &dummy );
   while ( ok ) {
      currentGroup = (psnBlockGroup*)( 
         (unsigned char*)dummy - offsetof(psnBlockGroup,node));

      offset = currentGroup->bitmap.baseAddressOffset;
      if ( ptr >= GET_PTR_FAST( offset, unsigned char) && 
           ptr < GET_PTR_FAST( offset+ (currentGroup->numBlocks << PSN_BLOCK_SHIFT), 
                          unsigned char ) ) {
         goodGroup = currentGroup;
         break;
      }
      oldGroup = currentGroup;
      ok = psnLinkedListPeakNext( &pMemObj->listBlockGroup,
                                   &oldGroup->node,
                                   &dummy );
   }
   VDS_PRE_CONDITION( goodGroup != NULL );

   goodGroup->freeBytes += numBytes;

   /* 
    * Check if the chunk before the current chunk (ptr)
    * is in the freeList or not.
    */
   p = ptr - PSN_ALLOCATION_UNIT;
   otherBufferisFree = psnIsBufferFree( &goodGroup->bitmap, SET_OFFSET(p) );
   if ( otherBufferisFree ) {
      /* Find the start of that free group of chunks */
      if ( psnIsBufferFree( &goodGroup->bitmap, 
         SET_OFFSET( ptr - 2*PSN_ALLOCATION_UNIT ) ) ) {
         /* The free group has more than one chunk */
         offset = *((ptrdiff_t*)p);
         GET_PTR( p, offset, unsigned char);
      }
      
      /*
       * The node is already in the linked list! All we need to do is
       * to modify the number of chunks that this node has.
       */
      ((psnFreeBufferNode*)p)->numBuffers += (numBytes/PSN_ALLOCATION_UNIT);
   }
   else {
      /*
       * The previous buffer is not free, so we need to insert our released
       * buffer as a new node in the list.
       */
      p = ptr; /* This is needed further down */
      ((psnFreeBufferNode*)p)->numBuffers = numBytes/PSN_ALLOCATION_UNIT;
      psnLinkNodeInit( &((psnFreeBufferNode*)p)->node );
      psnLinkedListPutLast( &goodGroup->freeList, 
                             &((psnFreeBufferNode*)p)->node );
   }

   /* 
    * Check if the chunk after the current group-of-chunks-to-be-released
    * is in the freeList or not.
    *
    * Note: "p" at this point represents the node which contain our buffer
    *       and, possibly, the previous buffer if it was free.
    */
   otherNode = (psnFreeBufferNode*)( ptr + numBytes);
   otherBufferisFree = psnIsBufferFree( &goodGroup->bitmap, 
                                         SET_OFFSET(otherNode) );
   if ( otherBufferisFree ) {
      ((psnFreeBufferNode*)p)->numBuffers += otherNode->numBuffers;
      psnLinkedListRemoveItem( &goodGroup->freeList, 
                                &otherNode->node );
      memset( otherNode, 0, sizeof(psnFreeBufferNode) );
   }

   if ( ((psnFreeBufferNode*)p)->numBuffers*PSN_ALLOCATION_UNIT == 
      goodGroup->maxFreeBytes && goodGroup->isDeletable ) {

      numBlocks = goodGroup->numBlocks;
      pMemObj->totalBlocks -= numBlocks;
      /* Remove the blockGroup to the list of groups of the memObject */
      psnLinkedListRemoveItem( &pMemObj->listBlockGroup, &goodGroup->node );

      psnBlockGroupFini( goodGroup );
      
      psnFreeBlocks( pContext->pAllocator,
                      PSN_ALLOC_ANY,
                      (unsigned char*)goodGroup,
                      numBlocks,
                      pContext );
   }
   else {
      /* Set the bitmap */
      psnSetBufferFree( &goodGroup->bitmap, SET_OFFSET(ptr), numBytes );
   
      /*
       * Put the offset of the first free block on the last free block.
       * This makes it simpler/faster to rejoin groups of free blocks. But 
       * only if the number of blocks in the group > 1.
       */
      if ( ((psnFreeBufferNode*)p)->numBuffers > 1 ) {
         /* Warning - we reuse ptr here */
          ptr = p + 
             (((psnFreeBufferNode*)p)->numBuffers-1) * PSN_ALLOCATION_UNIT; 
          *((ptrdiff_t *)ptr) = SET_OFFSET(p);
      }
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psnMemObjectStatus( psnMemObject * pMemObj, 
                          vdsObjStatus  * pStatus )
{
   psnLinkNode * dummy;
   psnBlockGroup * pGroup;
   bool ok;
   
   VDS_PRE_CONDITION( pMemObj != NULL );
   VDS_PRE_CONDITION( pStatus != NULL );

   pStatus->numBlocks = pMemObj->totalBlocks;
   pStatus->numBlockGroup = pMemObj->listBlockGroup.currentSize;
   pStatus->freeBytes = 0;
   
   /*
    * We retrieve the first node
    */
   ok = psnLinkedListPeakFirst( &pMemObj->listBlockGroup, &dummy );
   VDS_PRE_CONDITION( ok );

   while ( ok ) {
      pGroup = (psnBlockGroup*)( 
         (unsigned char*)dummy - offsetof(psnBlockGroup,node));
      pStatus->freeBytes += pGroup->freeBytes;

      ok = psnLinkedListPeakNext( &pMemObj->listBlockGroup,
                                   dummy,
                                   &dummy );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

