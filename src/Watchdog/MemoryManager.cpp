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

#include "Engine/Engine.h"
#include "Watchdog/MemoryManager.h"
#include "Common/MemoryFile.h"
#include "Engine/MemoryAllocator.h"
#include "Engine/Folder.h"
#include "Engine/ProcessManager.h"
#include "Engine/InitEngine.h"

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
   
   /* Very unlikely but just in case... */
   VDS_PRE_CONDITION( sizeof(vdseMemoryHeader) <= VDSE_BLOCK_SIZE );
   
   *ppHeader = NULL;
   m_memorySizeKB = memorySizekb;   

   vdscInitMemoryFile( &m_memory, m_memorySizeKB, memoryFileName );
   
   vdscBackStoreStatus( &m_memory, &fileStatus );
   
   errcode = vdscCreateBackstore( &m_memory, filePerms, &pContext->errorHandler );
   if ( errcode != 0 ) {
      return VDS_INTERNAL_ERROR;
   }

   errcode = vdscOpenMemFile( &m_memory, &m_pMemoryAddress, &pContext->errorHandler );   
   if ( errcode != 0 ) {
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
   if ( errcode != 0 ) {
      (*ppHeader) = NULL;
      return errcode;
   }

   /* The top folder  */
   unsigned char* ptr = vdseMallocBlocks( pAlloc, VDSE_ALLOC_API_OBJ, 1, pContext );
   if ( ptr == NULL ) {
      (*ppHeader) = NULL;
      return VDS_NOT_ENOUGH_VDS_MEMORY;
   }
   vdseFolder * pFolder = (vdseFolder *) ptr;
   
   enum ListErrors listErr;
   
   errcode = vdseMemObjectInit( &pFolder->memObject, 
                                VDSE_IDENT_FOLDER,
                                &pFolder->blockGroup,
                                1 );
   if ( errcode != VDS_OK ) {
      vdscSetError( &pContext->errorHandler,
                    g_vdsErrorHandle,
                    errcode );
      return -1;
   }
   vdseTxStatusInit( &m_pHeader->topHashItem.txStatus, NULL_OFFSET );

   pFolder->nodeObject.txCounter      = 0;
   pFolder->nodeObject.myNameLength   = 0;
   pFolder->nodeObject.myNameOffset   = NULL_OFFSET;
   pFolder->nodeObject.txStatusOffset = 
      SET_OFFSET( &m_pHeader->topHashItem.txStatus );
   pFolder->nodeObject.myParentOffset = NULL_OFFSET;

   listErr = vdseHashInit( &pFolder->hashObj, 
                           SET_OFFSET(&pFolder->memObject),
                           25, 
                           pContext );
   if ( listErr != LIST_OK ) {
      if ( listErr == LIST_NO_MEMORY ) {
         errcode = VDS_NOT_ENOUGH_VDS_MEMORY;
      }
      else {
         errcode = VDS_INTERNAL_ERROR;
      }
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
      return -1;
   }   
   (*ppHeader)->treeMgrOffset = SET_OFFSET( ptr );
   (*ppHeader)->topHashItem.dataOffset = SET_OFFSET(&(*ppHeader)->topDescriptor);
   (*ppHeader)->topDescriptor.offset = SET_OFFSET( ptr );
   (*ppHeader)->topDescriptor.nodeOffset = SET_OFFSET( &pFolder->nodeObject );
   (*ppHeader)->topDescriptor.memOffset = SET_OFFSET( &pFolder->memObject );
   (*ppHeader)->topDescriptor.apiType = VDS_FOLDER;

////////////////////////
#if 0
      descLength = offsetof(vdseObjectDescriptor, originalName) + 
          (partialLength+1) * sizeof(char);
      pDesc = (vdseObjectDescriptor *) malloc( descLength );

      memset( pDesc, 0, descLength );
      pDesc->apiType = objectType;
      pDesc->offset = SET_OFFSET( ptr );
      pDesc->nameLengthInBytes = partialLength * sizeof(char);
      memcpy( pDesc->originalName, originalName, pDesc->nameLengthInBytes );

      listErr = vdseHashInsert( &pFolder->hashObj, 
                                (unsigned char *)objectName, 
                                partialLength * sizeof(char), 
                                (void*)pDesc, 
                                descLength,
                                &pHashItem,
                                pContext );

      switch( objectType ) {
      case VDS_FOLDER:
         memObjType = VDSE_IDENT_FOLDER;
         break;

      free( pDesc ); 
      pDesc = NULL;
      
      objTxStatus = &pHashItem->txStatus;
      vdseTxStatusInit( objTxStatus, SET_OFFSET(pContext->pTransaction) );
      
      GET_PTR( pDesc, pHashItem->dataOffset, vdseObjectDescriptor );
      switch ( memObjType ) {
      case VDSE_IDENT_FOLDER:
         rc = vdseFolderInit( (vdseFolder*)ptr,
                              SET_OFFSET(pFolder),
                              numBlocks,
                              expectedNumOfChilds,
                              objTxStatus,
                              partialLength,
                              pDesc->originalName,
                              SET_OFFSET(pHashItem->key),
                              pContext );
         pDesc->nodeOffset = SET_OFFSET(ptr) + offsetof(vdseFolder,nodeObject);
         pDesc->memOffset  = SET_OFFSET(ptr) + offsetof(vdseFolder,memObject);
         break;
      
      }

#endif
//////////////////////   
   
   
   /* The Garbage Collection manager */
   ptr = vdseMallocBlocks( pAlloc, VDSE_ALLOC_ANY, 1, pContext );
   if ( ptr == NULL ) {
      (*ppHeader) = NULL;
      return VDS_NOT_ENOUGH_VDS_MEMORY;
   }

   vdseProcMgr * pManager = (vdseProcMgr *) ptr;
   
   errcode = vdseProcMgrInit( pManager, pContext );
   if ( errcode != 0 ) {
      (*ppHeader) = NULL;
      return errcode;
   }
   (*ppHeader)->processMgrOffset = SET_OFFSET( pManager );

   /* And finish with setting up the version (and eventually some "magic */
   /* cookie" to identify the file?) */

   strcpy( (*ppHeader)->cookie, "VDS" );
   (*ppHeader)->version = MEMORY_VERSION;
   (*ppHeader)->totalLength = m_memorySizeKB*1024;

   (*ppHeader)->sizeofPtr = SIZEOF_VOID_P;
#if WORDS_BIGENDIAN
   (*ppHeader)->bigEndian = true;
#else
   (*ppHeader)->bigEndian = false;
#endif
   (*ppHeader)->blockSize = VDSE_BLOCK_SIZE;
   (*ppHeader)->alignmentStruct = VDSC_ALIGNMENT_STRUCT;
   (*ppHeader)->alignmentChar   = VDSC_ALIGNMENT_CHAR;
   (*ppHeader)->alignmentInt16  = VDSC_ALIGNMENT_INT16;
   (*ppHeader)->alignmentInt32  = VDSC_ALIGNMENT_INT32;
   (*ppHeader)->alignmentInt64  = VDSC_ALIGNMENT_INT64;
   fprintf(stderr, "%d %d %d %d %d %d %d\n",
 VDSC_ALIGNMENT_STRUCT,
 VDSC_ALIGNMENT_CHAR,
 VDSC_ALIGNMENT_INT16,
 VDSC_ALIGNMENT_INT32,
 VDSC_ALIGNMENT_INT64,
 VDSC_ALIGNMENT_CHAR_ARRAY,
 VDSC_ALIGNMENT_BOOL );
#if defined(CONFIG_KERNEL_HEADERS)
   (*ppHeader)->usingSpinlocks = true;
#else
   (*ppHeader)->usingSpinlocks = false;
#endif
   (*ppHeader)->allocationUnit = VDSE_ALLOCATION_UNIT;
   strncpy( (*ppHeader)->cpu_type, MYCPU, 19 );
   strncpy( (*ppHeader)->compiler, MYCC, 19);
   strncpy( (*ppHeader)->cxxcompiler, MYCXX, 19);
   
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
   
   if ( ! fileStatus.fileExist ) {
      return VDS_BACKSTORE_FILE_MISSING;
   }
   
   errcode = vdscOpenMemFile( &m_memory, &m_pMemoryAddress, &errorHandler );   
   if ( errcode != 0 ) {
/*#if defined (VDS_DEBUG) */
      fprintf( stderr, "MMAP failure - %d %s\n", errno, memoryFileName );
/*#endif */
      return VDS_ERROR_OPENING_VDS;
   }
   
   m_pHeader = *ppHeader = (vdseMemoryHeader*) m_pMemoryAddress;

   if ( (*ppHeader)->version != MEMORY_VERSION ) {
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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

