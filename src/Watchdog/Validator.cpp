/*
 * Copyright (C) 2007 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of vdsf (Virtual Data Space Framework).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation and appearing in the file COPYING included in the
 * packaging of this library.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#include "Common/Common.h"
#include <iostream>

#include "Watchdog/Validator.h"
#include "Watchdog/ValidateFolder.h"

#include "Engine/MemoryHeader.h"

#include "Engine/ProcessManager.h"
//#include "Engine/SessionContext.h"
//#include "Engine/InitEngine.h"
#include "Engine/Folder.h"
#include "Engine/MemoryAllocator.h"
#include "Engine/Queue.h"

using namespace std;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

vdsValidator::vdsValidator( vdseMemoryHeader * pMemoryAddress )
   : m_bTestAllocator( false ),
     m_pMemoryAddress( pMemoryAddress )
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

vdsValidator::~vdsValidator()
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsValidator::echo( char * message )
{
   cerr << message << endl;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int vdsValidator::Validate()
{
   vdseProcessManager * processMgr;
   vdseFolder * topFolder;
   vdseMemAlloc * memAllocator;
   enum vdswValidation valid;
   vdseSessionContext context;
   
   GET_PTR( processMgr, m_pMemoryAddress->processMgrOffset, vdseProcessManager );
   GET_PTR( topFolder, m_pMemoryAddress->treeMgrOffset, vdseFolder );
   GET_PTR( memAllocator, m_pMemoryAddress->allocatorOffset, vdseMemAlloc );

   // Test the lock of the allocator
   if ( vdscIsItLocked( &memAllocator->memObj.lock ) )
   {
      cerr << "Warning! The memory allocator is locked - the VDS might" << endl;
      cerr << "be corrupted. Trying to reset it..." << endl;
      vdscReleaseProcessLock ( &memAllocator->memObj.lock );
      m_bTestAllocator = true;
   }

   vdseInitSessionContext( &context );
   context.pAllocator = (void *) memAllocator;
   valid = vdswValidateFolder( topFolder, 1, &context );
   
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int vdsValidator::ValidateFolder( vdseFolder * pFolder )
{
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int vdsValidator::ValidateHashMap( vdseHashMap * pHashMap )
{
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int vdsValidator::ValidateList( vdseLinkedList * pList )
{
   vdseLinkNode * next, * previous;
   ptrdiff_t headOffset;
   size_t forwardChainLen = 0, backwardChainLen = 0;
   bool breakInForwardChain = false;
   bool breakInBackwardChain = false;
   
   headOffset = SET_OFFSET( &pList->head );

   // We loop forward until we come back to head or until we clearly have
   // a discontinuity in the chain.
   next = &pList->head;
   while ( next->nextOffset != headOffset )
   {
      if ( ValidateOffset( next->nextOffset ) )
         ++forwardChainLen;
      else
      {
         next->nextOffset = headOffset;
         breakInForwardChain = true;
         break;
      }
      GET_PTR( next, next->nextOffset, vdseLinkNode );
   }

   // Same as before but we loop backward.
   next = &pList->head;
   while ( next->previousOffset != headOffset )
   {
      if ( ValidateOffset( next->previousOffset ) )
         ++backwardChainLen;
      else
      {
         next->previousOffset = headOffset;
         breakInBackwardChain = true;
         break;
      }
      GET_PTR( next, next->previousOffset, vdseLinkNode );
   }
   
   // So how did it go?... we have multiple possibilities here.
   if ( ! breakInForwardChain && ! breakInBackwardChain )
   {
      if ( backwardChainLen == forwardChainLen ) // all is well !
      {
         if ( pList->currentSize != forwardChainLen )         
         {
            echo( "Small correction in the number of elements of linked list");
            pList->currentSize = forwardChainLen;
         }
         return 0;
      }
      echo( "Warning - counts in foward and backward chains differ:" );
      echo( "          using the longest chain to rebuild" );
      if ( backwardChainLen > forwardChainLen )
         breakInForwardChain = true;
      else
         breakInBackwardChain = true;
   }
   
   if ( breakInForwardChain && breakInBackwardChain )
   {
      echo( "Both chains broken - don't know how to repair" );
      return -1;
   }
   
   if ( breakInBackwardChain )
   {
      echo( "Repairing list using forward chain" );
      // We loop forward until we come back to head.
      
      previous = &pList->head;
      do
      {
         GET_PTR( next, previous->nextOffset, vdseLinkNode );
         next->previousOffset = SET_OFFSET( previous );
         // prepare for next round
         previous = next;
         
      } while ( next != &pList->head );
      
      pList->currentSize = forwardChainLen;
   }

   if ( breakInForwardChain )
   {
      echo( "Repairing list using backward chain" );
      // We loop backward until we come back to head.
      
      previous = &pList->head;
      do
      {
         GET_PTR( next, previous->previousOffset, vdseLinkNode );
         next->nextOffset = SET_OFFSET( previous );
         // prepare for next round
         previous = next;
         
      } while ( next != &pList->head );
      
      pList->currentSize = backwardChainLen;
   }
   
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#if 0
int vdsValidator::ValidateMemObject( vdseMemObject * pMemObject )
{
   int rc;
   size_t numBlocks = 0;
   
   rc = ValidateList( &pMemObject->listBlockGroup );
   if ( rc == 0 )
   {
      if ( pMemObject->listBlockGroup.currentSize == 0 )
      {
         echo("listBlockGroup with zero size!");
         return -1;
      }

      // Retrieve all the blockgroups and add up the # blocks

      headOffset = SET_OFFSET( &pMemObject->listBlockGroup.head );
      next = &pMemObject->listBlockGroup.head;
      while ( next->nextOffset != headOffset )
      {
         
         numBlocks += ;
         GET_PTR( next, next->nextOffset, vdseLinkNode );
      }
   }
   
   return rc;
}
#endif

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int vdsValidator::ValidateQueue( vdseQueue * pQueue )
{
   bool testQueue = false;
   int rc = 0;
   
   if ( vdscIsItLocked( &pQueue->memObject.lock ) )
   {
      vdscReleaseProcessLock ( &pQueue->memObject.lock );
      testQueue = true;
   }
   if ( pQueue->memObject.objType != VDSE_IDENT_QUEUE )
   {
      echo( "Object identifier is wrong! Applying correction." );
      pQueue->memObject.objType = VDSE_IDENT_QUEUE;
   }
   
   if ( testQueue )
   {
      rc = ValidateList( &pQueue->memObject.listBlockGroup );
   //   rc = ValidateMemObject( &pQueue->memObject );
      rc = ValidateList( &pQueue->listOfElements );
   }
   
   // 
   return rc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
