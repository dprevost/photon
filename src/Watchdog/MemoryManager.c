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

void vdswMemoryManagerInit( vdswMemoryManager * pManager )
{
   VDS_PRE_CONDITION( pManager != NULL );

   pManager->memorySizeKB = 0;
   pManager->pMemoryAddress = NULL;
   pManager->pHeader = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdswMemoryManagerFini( vdswMemoryManager * pManager )
{
   VDS_PRE_CONDITION( pManager != NULL );

   pManager->memorySizeKB = 0;
   pManager->pMemoryAddress = NULL;
   pManager->pHeader = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdswCreateVDS( vdswMemoryManager  * pManager,
                   const char         * memoryFileName,
                   size_t               memorySizekb,
                   int                  filePerms,
                   vdseMemoryHeader  ** ppHeader,
                   vdseSessionContext * pContext )
{
   int errcode = 0;
   vdscMemoryFileStatus fileStatus;
   vdseMemAlloc * pAlloc;
   unsigned char * ptr;
   vdseFolder * pFolder;
   vdseProcMgr * processManager;
   bool ok;
   
   /* Very unlikely but just in case... */
   VDS_PRE_CONDITION( pManager       != NULL );
   VDS_PRE_CONDITION( memoryFileName != NULL );
   VDS_PRE_CONDITION( ppHeader       != NULL );
   VDS_PRE_CONDITION( pContext       != NULL );
   VDS_PRE_CONDITION( memorySizekb > 0 );

   VDS_INV_CONDITION( sizeof(vdseMemoryHeader) <= VDSE_BLOCK_SIZE );
   
   *ppHeader = NULL;
   pManager->memorySizeKB = memorySizekb;   

   vdscInitMemoryFile( &pManager->memory, pManager->memorySizeKB, memoryFileName );
   
   vdscBackStoreStatus( &pManager->memory, &fileStatus );
   
   errcode = vdscCreateBackstore( &pManager->memory, filePerms, &pContext->errorHandler );
   if ( errcode != 0 ) {
      return VDS_INTERNAL_ERROR;
   }

   errcode = vdscOpenMemFile( &pManager->memory, &pManager->pMemoryAddress, &pContext->errorHandler );   
   if ( errcode != 0 ) {
/*#if defined (VDS_DEBUG) */
/*        fprintf( stderr, "MMAP failure - %d\n", errno ); */
/*#endif */
      return VDS_INTERNAL_ERROR;
   }
      
   pManager->pHeader = *ppHeader = (vdseMemoryHeader*) pManager->pMemoryAddress;
   (*ppHeader)->running = 1;

   /* Sets the global base address */
   g_pBaseAddr = (unsigned char *) pManager->pMemoryAddress;
   
   /* The memory allocator starts after the header */
   unsigned char* pStart = (unsigned char*)pManager->pMemoryAddress +
      VDSE_BLOCK_SIZE;
   
   (*ppHeader)->allocatorOffset = SET_OFFSET( pStart );
   pAlloc = (vdseMemAlloc *) pStart;
   
   errcode = vdseMemAllocInit( pAlloc,
                               g_pBaseAddr,
                               pManager->memorySizeKB*1024,
                               pContext );
   if ( errcode != 0 ) {
      (*ppHeader) = NULL;
      return errcode;
   }

   /* The top folder  */
   ptr = vdseMallocBlocks( pAlloc, VDSE_ALLOC_API_OBJ, 1, pContext );
   if ( ptr == NULL ) {
      (*ppHeader) = NULL;
      return VDS_NOT_ENOUGH_VDS_MEMORY;
   }
   pFolder = (vdseFolder *) ptr;

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
   vdseTxStatusInit( &pManager->pHeader->topHashItem.txStatus, VDSE_NULL_OFFSET );

   pFolder->nodeObject.txCounter      = 0;
   pFolder->nodeObject.myNameLength   = 0;
   pFolder->nodeObject.myNameOffset   = VDSE_NULL_OFFSET;
   pFolder->nodeObject.txStatusOffset = 
      SET_OFFSET( &pManager->pHeader->topHashItem.txStatus );
   pFolder->nodeObject.myParentOffset = VDSE_NULL_OFFSET;

   errcode = vdseHashInit( &pFolder->hashObj, 
                      SET_OFFSET(&pFolder->memObject),
                      25, 
                      pContext );
   if ( errcode != VDS_OK ) {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
      return -1;
   }   
   (*ppHeader)->treeMgrOffset = SET_OFFSET( ptr );
   (*ppHeader)->topHashItem.dataOffset = SET_OFFSET(&(*ppHeader)->topDescriptor);
   (*ppHeader)->topDescriptor.offset = SET_OFFSET( ptr );
   (*ppHeader)->topDescriptor.nodeOffset = SET_OFFSET( &pFolder->nodeObject );
   (*ppHeader)->topDescriptor.memOffset = SET_OFFSET( &pFolder->memObject );
   (*ppHeader)->topDescriptor.apiType = VDS_FOLDER;

   /* The Garbage Collection manager */
   ptr = vdseMallocBlocks( pAlloc, VDSE_ALLOC_ANY, 1, pContext );
   if ( ptr == NULL ) {
      (*ppHeader) = NULL;
      return VDS_NOT_ENOUGH_VDS_MEMORY;
   }

   processManager = (vdseProcMgr *) ptr;
   
   errcode = vdseProcMgrInit( processManager, pContext );
   if ( errcode != 0 ) {
      (*ppHeader) = NULL;
      return errcode;
   }
   (*ppHeader)->processMgrOffset = SET_OFFSET( processManager );

   /* And finish with setting up the version (and eventually some "magic */
   /* cookie" to identify the file?) */

   strcpy( (*ppHeader)->cookie, "VDS" );
   (*ppHeader)->version = VDSE_MEMORY_VERSION;
   (*ppHeader)->totalLength = pManager->memorySizeKB*1024;

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
#if 0
   fprintf(stderr, "%d %d %d %d %d %d %d\n",
 VDSC_ALIGNMENT_STRUCT,
 VDSC_ALIGNMENT_CHAR,
 VDSC_ALIGNMENT_INT16,
 VDSC_ALIGNMENT_INT32,
 VDSC_ALIGNMENT_INT64,
 VDSC_ALIGNMENT_CHAR_ARRAY,
 VDSC_ALIGNMENT_BOOL );
#endif
   (*ppHeader)->usingSpinlocks = false;
   (*ppHeader)->allocationUnit = VDSE_ALLOCATION_UNIT;
   strncpy( (*ppHeader)->cpu_type, MYCPU, 19 );
   strncpy( (*ppHeader)->compiler, MYCC, 19);
   strncpy( (*ppHeader)->cxxcompiler, MYCXX, 19);
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdswOpenVDS( vdswMemoryManager * pManager, 
                 const char        * memoryFileName,
                 size_t              memorySizekb,
                 vdseMemoryHeader ** ppHeader )
{
   int errcode = 0;
   vdscMemoryFileStatus fileStatus;
   vdscErrorHandler errorHandler;
   
   VDS_PRE_CONDITION( pManager       != NULL );
   VDS_PRE_CONDITION( memoryFileName != NULL );
   VDS_PRE_CONDITION( ppHeader       != NULL );
   VDS_PRE_CONDITION( memorySizekb > 0 );

   *ppHeader = NULL;
   pManager->memorySizeKB = memorySizekb;
   
   vdscInitErrorHandler( &errorHandler );
   
   vdscInitMemoryFile( &pManager->memory, pManager->memorySizeKB, memoryFileName );
   
   vdscBackStoreStatus( &pManager->memory, &fileStatus );
   
   if ( ! fileStatus.fileExist ) {
      return VDS_BACKSTORE_FILE_MISSING;
   }
   
   errcode = vdscOpenMemFile( &pManager->memory, &pManager->pMemoryAddress, &errorHandler );   
   if ( errcode != 0 ) {
/*#if defined (VDS_DEBUG) */
      fprintf( stderr, "MMAP failure - %d %s\n", errno, memoryFileName );
/*#endif */
      return VDS_ERROR_OPENING_VDS;
   }
   
   pManager->pHeader = *ppHeader = (vdseMemoryHeader*) pManager->pMemoryAddress;

   if ( (*ppHeader)->version != VDSE_MEMORY_VERSION ) {
      (*ppHeader) = NULL;
      return VDS_INCOMPATIBLE_VERSIONS;
   }

   /* Sets the global base address */
   g_pBaseAddr = (unsigned char *) pManager->pMemoryAddress;
  
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdswCloseVDS( vdswMemoryManager * pManager,
                   vdscErrorHandler  * pError )
{
   VDS_PRE_CONDITION( pManager != NULL );
   VDS_PRE_CONDITION( pError   != NULL );

   vdscCloseMemFile( &pManager->memory, pError );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

