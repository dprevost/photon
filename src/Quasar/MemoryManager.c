/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of Photon (photonsoftware.org).
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

#include "Nucleus/Engine.h"
#include "Quasar/MemoryManager.h"
#include "Common/MemoryFile.h"
#include "Nucleus/MemoryAllocator.h"
#include "Nucleus/Folder.h"
#include "Nucleus/ProcessManager.h"
#include "Nucleus/InitEngine.h"
#include "Quasar/quasarErrors.h"

extern pscErrMsgHandle g_wdErrorHandle;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define MEMORY_HEADER_SPACE 4096

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdswMemoryManagerInit( vdswMemoryManager * pManager )
{
   PSO_PRE_CONDITION( pManager != NULL );

   pManager->memorySizeKB = 0;
   pManager->pMemoryAddress = NULL;
   pManager->pHeader = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdswMemoryManagerFini( vdswMemoryManager * pManager )
{
   PSO_PRE_CONDITION( pManager != NULL );

   pManager->memorySizeKB = 0;
   pManager->pMemoryAddress = NULL;
   pManager->pHeader = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool vdswCreateVDS( vdswMemoryManager  * pManager,
                    const char         * memoryFileName,
                    size_t               memorySizekb,
                    int                  filePerms,
                    psnMemoryHeader  ** ppHeader,
                    psnSessionContext * pContext )
{
   int errcode = 0;
   pscMemoryFileStatus fileStatus;
   psnMemAlloc * pAlloc;
   unsigned char * ptr;
   psnFolder * pFolder;
   psnProcMgr * processManager;
   time_t t;
   struct tm formattedTime;
   unsigned char * pStart;
   bool ok;
   
   /* Very unlikely but just in case... */
   PSO_PRE_CONDITION( pManager       != NULL );
   PSO_PRE_CONDITION( memoryFileName != NULL );
   PSO_PRE_CONDITION( ppHeader       != NULL );
   PSO_PRE_CONDITION( pContext       != NULL );
   PSO_PRE_CONDITION( memorySizekb > 0 );

   PSO_INV_CONDITION( sizeof(psnMemoryHeader) <= PSN_BLOCK_SIZE );
   
   *ppHeader = NULL;
   pManager->memorySizeKB = memorySizekb;   

   pscInitMemoryFile( &pManager->memory, pManager->memorySizeKB, memoryFileName );
   
   pscBackStoreStatus( &pManager->memory, &fileStatus );
   
   ok = pscCreateBackstore( &pManager->memory, filePerms, &pContext->errorHandler );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) {
      pscChainError( &pContext->errorHandler,
                      g_wdErrorHandle,
                      PSOQ_CREATE_BACKSTORE_FAILURE );
      return false;
   }

   ok = pscOpenMemFile( &pManager->memory, &pManager->pMemoryAddress, &pContext->errorHandler );   
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) {
      pscChainError( &pContext->errorHandler,
                      g_wdErrorHandle,
                      PSOQ_OPEN_BACKSTORE_FAILURE );
      return false;
   }
      
   pManager->pHeader = *ppHeader = (psnMemoryHeader*) pManager->pMemoryAddress;
   (*ppHeader)->running = 1;

   /* Sets the global base address */
   g_pBaseAddr = (unsigned char *) pManager->pMemoryAddress;
   
   /* The memory allocator starts after the header */
   pStart = (unsigned char*)pManager->pMemoryAddress + PSN_BLOCK_SIZE;
   
   (*ppHeader)->allocatorOffset = SET_OFFSET( pStart );
   pAlloc = (psnMemAlloc *) pStart;
   
   errcode = psnMemAllocInit( pAlloc,
                               g_pBaseAddr,
                               pManager->memorySizeKB*1024,
                               pContext );
   if ( errcode != 0 ) {
      (*ppHeader) = NULL;
      pscSetError( &pContext->errorHandler,
                    g_psoErrorHandle,
                    errcode );
      return false;
   }

   /* The top folder  */
   ptr = psnMallocBlocks( pAlloc, PSN_ALLOC_API_OBJ, 1, pContext );
   if ( ptr == NULL ) {
      (*ppHeader) = NULL;
      pscSetError( &pContext->errorHandler,
                    g_psoErrorHandle,
                    PSO_NOT_ENOUGH_PSO_MEMORY );
      return false;
   }
   pFolder = (psnFolder *) ptr;

   errcode = psnMemObjectInit( &pFolder->memObject, 
                                PSN_IDENT_FOLDER,
                                &pFolder->blockGroup,
                                1 );
   if ( errcode != PSO_OK ) {
      pscSetError( &pContext->errorHandler,
                    g_wdErrorHandle,
                    errcode );
      return false;
   }
   psnTxStatusInit( &pManager->pHeader->topHashItem.txStatus, PSN_NULL_OFFSET );

   pFolder->nodeObject.txCounter      = 0;
   pFolder->nodeObject.myNameLength   = 0;
   pFolder->nodeObject.myNameOffset   = PSN_NULL_OFFSET;
   pFolder->nodeObject.txStatusOffset = 
      SET_OFFSET( &pManager->pHeader->topHashItem.txStatus );
   pFolder->nodeObject.myParentOffset = PSN_NULL_OFFSET;

   errcode = psnHashInit( &pFolder->hashObj, 
                      SET_OFFSET(&pFolder->memObject),
                      25, 
                      pContext );
   if ( errcode != PSO_OK ) {
      pscSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
      return false;
   }   
   (*ppHeader)->treeMgrOffset = SET_OFFSET( ptr );
   (*ppHeader)->topHashItem.dataOffset = SET_OFFSET(&(*ppHeader)->topDescriptor);
   (*ppHeader)->topDescriptor.offset = SET_OFFSET( ptr );
   (*ppHeader)->topDescriptor.nodeOffset = SET_OFFSET( &pFolder->nodeObject );
   (*ppHeader)->topDescriptor.memOffset = SET_OFFSET( &pFolder->memObject );
   (*ppHeader)->topDescriptor.apiType = PSO_FOLDER;

   /* The Garbage Collection manager */
   ptr = psnMallocBlocks( pAlloc, PSN_ALLOC_ANY, 1, pContext );
   if ( ptr == NULL ) {
      (*ppHeader) = NULL;
      pscSetError( &pContext->errorHandler,
                    g_psoErrorHandle,
                    PSO_NOT_ENOUGH_PSO_MEMORY );
      return false;
   }

   processManager = (psnProcMgr *) ptr;
   
   ok = psnProcMgrInit( processManager, pContext );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) {
      (*ppHeader) = NULL;
      /* The error is set in the function itself */
      return false;
   }
   (*ppHeader)->processMgrOffset = SET_OFFSET( processManager );

   /* And finish with setting up the version (and eventually some "magic */
   /* cookie" to identify the file?) */

   strcpy( (*ppHeader)->cookie, "VDS" );
   (*ppHeader)->version = PSN_MEMORY_VERSION;
   (*ppHeader)->totalLength = pManager->memorySizeKB*1024;

   (*ppHeader)->sizeofPtr = SIZEOF_VOID_P;
#if WORDS_BIGENDIAN
   (*ppHeader)->bigEndian = true;
#else
   (*ppHeader)->bigEndian = false;
#endif
   (*ppHeader)->blockSize = PSN_BLOCK_SIZE;
   (*ppHeader)->alignmentStruct = PSC_ALIGNMENT_STRUCT;
   (*ppHeader)->alignmentChar   = PSC_ALIGNMENT_CHAR;
   (*ppHeader)->alignmentInt16  = PSC_ALIGNMENT_INT16;
   (*ppHeader)->alignmentInt32  = PSC_ALIGNMENT_INT32;
   (*ppHeader)->alignmentInt64  = PSC_ALIGNMENT_INT64;

   (*ppHeader)->allocationUnit = PSN_ALLOCATION_UNIT;
   strncpy( (*ppHeader)->cpu_type, MYCPU, 19 );
   strncpy( (*ppHeader)->compiler, MYCC, 19);
   strncpy( (*ppHeader)->cxxcompiler, MYCXX, 19);
   strncpy( (*ppHeader)->watchdogVersion, PACKAGE_VERSION, 10 );

   t = time(NULL);
   localtime_r( &t, &formattedTime );
   strftime( (*ppHeader)->creationTime, 30, "%Y-%m-%d %H:%M:%S %Z", &formattedTime );

#if PSO_USE_GCC 
#  if defined(__GNUC_PATCHLEVEL__)
   sprintf( (*ppHeader)->compilerVersion, "%d.%d.%d", __GNUC__, 
      __GNUC_MINOR__, __GNUC_PATCHLEVEL__ );
#  else
   sprintf( (*ppHeader)->compilerVersion, "%d.%d", __GNUC__, __GNUC_MINOR__ );
#  endif
#endif
#if PSO_USE_XLC
   sprintf( (*ppHeader)->compilerVersion, "%d.%d", __xlc__/0xff, 
      __xlc__ % 0xff );
#endif
#if PSO_USE_ICC
   sprintf( (*ppHeader)->compilerVersion, "%d.%d.%d", __INTEL_COMPILER/100, 
      (__INTEL_COMPILER/10) % 10, __INTEL_COMPILER % 100 );
#endif
#if PSO_USE_PATHSCALE
   sprintf( (*ppHeader)->compilerVersion, "%d.%d.%d", __PATHCC__, 
      __PATHCC_MINOR__, __PATHCC_PATCHLEVEL__ );
#endif
#if defined(_MSC_VER)
   sprintf( (*ppHeader)->compilerVersion, "%d.%d",
     _MSC_VER/100, _MSC_VER % 100 );
#endif

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool vdswOpenVDS( vdswMemoryManager  * pManager, 
                  const char         * memoryFileName,
                  size_t               memorySizekb,
                  psnMemoryHeader  ** ppHeader,
                  psnSessionContext * pContext )
{
   bool ok;
   
   pscMemoryFileStatus fileStatus;
   pscErrorHandler errorHandler;
   
   PSO_PRE_CONDITION( pManager       != NULL );
   PSO_PRE_CONDITION( memoryFileName != NULL );
   PSO_PRE_CONDITION( ppHeader       != NULL );
   PSO_PRE_CONDITION( pContext       != NULL );
   PSO_PRE_CONDITION( memorySizekb > 0 );

   *ppHeader = NULL;
   pManager->memorySizeKB = memorySizekb;
   
   pscInitErrorHandler( &errorHandler );
   
   pscInitMemoryFile( &pManager->memory, pManager->memorySizeKB, memoryFileName );
   
   pscBackStoreStatus( &pManager->memory, &fileStatus );
   
   if ( ! fileStatus.fileExist ) {
      pscSetError( &pContext->errorHandler,
                    g_wdErrorHandle,
                    PSOQ_BACKSTORE_FILE_MISSING );
      return false;
   }
   
   ok = pscOpenMemFile( &pManager->memory, 
                         &pManager->pMemoryAddress, 
                         &errorHandler );   
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) {
      pscChainError( &pContext->errorHandler,
                      g_wdErrorHandle,
                      PSOQ_ERROR_OPENING_VDS );
      return false;
   }
   
   pManager->pHeader = *ppHeader = (psnMemoryHeader*) pManager->pMemoryAddress;

   if ( (*ppHeader)->version != PSN_MEMORY_VERSION ) {
      (*ppHeader) = NULL;
      pscSetError( &pContext->errorHandler,
                    g_wdErrorHandle,
                    PSOQ_INCOMPATIBLE_VERSIONS );
      return false;
   }

   /* Sets the global base address */
   g_pBaseAddr = (unsigned char *) pManager->pMemoryAddress;
  
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdswCloseVDS( vdswMemoryManager * pManager,
                   pscErrorHandler  * pError )
{
   PSO_PRE_CONDITION( pManager != NULL );
   PSO_PRE_CONDITION( pError   != NULL );

   pscCloseMemFile( &pManager->memory, pError );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

