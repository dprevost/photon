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

#include "Engine/Engine.h"
#include "Watchdog/MemoryManager.h"
#include "Common/MemoryFile.h"
#include "Engine/MemoryAllocator.h"
#include "Engine/Folder.h"
#include "Engine/ProcessManager.h"

//#include "TreeManager.h"
//#include "TransactionManager.h"
//#include "CleanupManager.h"
//#include "SessionContext.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define MEMORY_HEADER_SPACE 4096

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

vdswMemoryManager::vdswMemoryManager()
   : m_memorySizeKB   ( 0    ),
     m_pMemoryAddress ( NULL ),
     m_pHeader        ( NULL )
{
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

vdswMemoryManager::~vdswMemoryManager()
{
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdswMemoryManager::CreateVDS( const char         * memoryFileName,
                                  size_t               memorySizekb,
                                  int                  filePerms,
                                  vdseMemoryHeader  ** ppHeader,
                                  vdseSessionContext * pContext )
{
   int errcode = 0;
   vdscMemoryFileStatus fileStatus;
   vdseMemAlloc * pAlloc;
   
   *ppHeader = NULL;
   m_memorySizeKB = memorySizekb;   

   vdscInitMemoryFile( &m_memory, m_memorySizeKB, memoryFileName );
   
   vdscBackStoreStatus( &m_memory, &fileStatus );
   
   errcode = vdscCreateBackstore( &m_memory, filePerms, &pContext->errorHandler );
   if ( errcode != 0 )
   {
      return VDS_INTERNAL_ERROR;
   }

   errcode = vdscOpenMemFile( &m_memory, &m_pMemoryAddress, &pContext->errorHandler );   
   if ( errcode != 0 )
   {
/*#if defined (VDS_DEBUG) */
/*        fprintf( stderr, "MMAP failure - %d\n", errno ); */
/*#endif */
      return VDS_INTERNAL_ERROR;
   }
      
   m_pHeader = *ppHeader = (vdseMemoryHeader*) m_pMemoryAddress;
   (*ppHeader)->running = 1;

   /* Sets the global base address */
   g_pBaseAddr = (unsigned char *) m_pMemoryAddress;
   
   /* The memory allocator starts after the header */
   unsigned char* pStart = (unsigned char*)m_pMemoryAddress +
      VDSE_BLOCK_SIZE;
   
   (*ppHeader)->allocatorOffset = SET_OFFSET( pStart );
   pAlloc = (vdseMemAlloc *) pStart;
   
   errcode = vdseMemAllocInit( pAlloc,
                               g_pBaseAddr,
                               m_memorySizeKB*1024,
                               pContext );
   if ( errcode != 0 )
   {
      (*ppHeader) = NULL;
      return errcode;
   }

   /* The top folder  */
   unsigned char* ptr = vdseMallocBlocks( pAlloc, 1, pContext );
   if ( ptr == NULL )
   {
      (*ppHeader) = NULL;
      return VDS_NOT_ENOUGH_VDS_MEMORY;
   }
   vdseFolder * pFolder = (vdseFolder *) ptr;
   
   enum ListErrors listErr;
   
   errcode = vdseMemObjectInit( &pFolder->memObject, 
                                VDSE_IDENT_FOLDER,
                                &pFolder->blockGroup,
                                1 );
   if ( errcode != VDS_OK )
   {
      vdscSetError( &pContext->errorHandler,
                    g_vdsErrorHandle,
                    errcode );
      return -1;
   }

   pFolder->nodeObject.txCounter      = 0;
   pFolder->nodeObject.myNameLength   = 0;
   pFolder->nodeObject.myNameOffset   = NULL_OFFSET;
   pFolder->nodeObject.txStatusOffset = NULL_OFFSET;
   pFolder->nodeObject.myParentOffset = NULL_OFFSET;

   listErr = vdseHashInit( &pFolder->hashObj, 25, pContext );
   if ( listErr != LIST_OK )
   {
      if ( listErr == LIST_NO_MEMORY )
         errcode = VDS_NOT_ENOUGH_VDS_MEMORY;
      else
         errcode = VDS_INTERNAL_ERROR;
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
      return -1;
   }   
   (*ppHeader)->treeMgrOffset = SET_OFFSET( ptr );
      
   /* The Garbage Collection manager */
   ptr = vdseMallocBlocks( pAlloc, 1, pContext );
   if ( ptr == NULL )
   {
      (*ppHeader) = NULL;
      return VDS_NOT_ENOUGH_VDS_MEMORY;
   }

   vdseProcMgr * pManager = (vdseProcMgr *) ptr;
   
   errcode = vdseProcMgrInit( pManager, pContext );
   if ( errcode != 0 )
   {
      (*ppHeader) = NULL;
      return errcode;
   }
   (*ppHeader)->cleanupMgrOffset = SET_OFFSET( pManager );

   /* And finish with setting up the version (and eventually some "magic */
   /* cookie" to identify the file?) */
   (*ppHeader)->version = MEMORY_VERSION;

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdswMemoryManager::OpenVDS( const char        * memoryFileName,
                                size_t              memorySizekb,
                                vdseMemoryHeader ** ppHeader )
{
   int errcode = 0;
   vdscMemoryFileStatus fileStatus;
   vdscErrorHandler errorHandler;
   
   *ppHeader = NULL;
   m_memorySizeKB = memorySizekb;
   
   vdscInitErrorHandler( &errorHandler );
   
   vdscInitMemoryFile( &m_memory, m_memorySizeKB, memoryFileName );
   
   vdscBackStoreStatus( &m_memory, &fileStatus );
   
   if ( ! fileStatus.fileExist )
   {
      return VDS_BACKSTORE_FILE_MISSING;
   }
   
   errcode = vdscOpenMemFile( &m_memory, &m_pMemoryAddress, &errorHandler );   
   if ( errcode != 0 )
   {
/*#if defined (VDS_DEBUG) */
      fprintf( stderr, "MMAP failure - %d %s\n", errno, memoryFileName );
/*#endif */
      return VDS_ERROR_OPENING_VDS;
   }
   
   m_pHeader = *ppHeader = (vdseMemoryHeader*) m_pMemoryAddress;

   if ( (*ppHeader)->version != MEMORY_VERSION )
   {
      (*ppHeader) = NULL;
      return VDS_INCOMPATIBLE_VERSIONS;
   }

   /* Sets the global base address */
   g_pBaseAddr = (unsigned char *) m_pMemoryAddress;
  
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdswMemoryManager::Close( vdscErrorHandler* pError )
{
   vdscCloseMemFile( &m_memory, pError );
}
}
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
