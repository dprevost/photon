/*
 * Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#include "Nucleus/Engine.h"
#include "Quasar/MemoryManager.h"
#include "Common/MemoryFile.h"
#include "Nucleus/MemoryAllocator.h"
#include "Nucleus/Folder.h"
#include "Nucleus/ProcessManager.h"
#include "Nucleus/InitEngine.h"
#include "Quasar/quasarErrors.h"

extern psocErrMsgHandle g_qsrErrorHandle;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define MEMORY_HEADER_SPACE 4096

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void qsrMemoryManagerInit( qsrMemoryManager * pManager )
{
   PSO_PRE_CONDITION( pManager != NULL );

   pManager->memorySizeKB = 0;
   pManager->pMemoryAddress = NULL;
   pManager->pHeader = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void qsrMemoryManagerFini( qsrMemoryManager * pManager )
{
   PSO_PRE_CONDITION( pManager != NULL );

   pManager->memorySizeKB = 0;
   pManager->pMemoryAddress = NULL;
   pManager->pHeader = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool qsrCreateMem( qsrMemoryManager   * pManager,
                   const char         * memoryFileName,
                   size_t               memorySizekb,
                   int                  filePerms,
                   psonMemoryHeader  ** ppHeader,
                   psonSessionContext * pContext )
{
   int errcode = 0;
   psocMemoryFileStatus fileStatus;
   psonMemAlloc * pAlloc;
   unsigned char * ptr;
   psonFolder * pFolder;
   psonProcMgr * processManager;
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

   PSO_INV_CONDITION( sizeof(psonMemoryHeader) <= PSON_BLOCK_SIZE );
   
   *ppHeader = NULL;
   pManager->memorySizeKB = memorySizekb;   

   psocInitMemoryFile( &pManager->memory, pManager->memorySizeKB, memoryFileName );
   
   psocBackStoreStatus( &pManager->memory, &fileStatus );
   
   ok = psocCreateBackstore( &pManager->memory, filePerms, &pContext->errorHandler );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) {
      psocChainError( &pContext->errorHandler,
                      g_qsrErrorHandle,
                      QSR_CREATE_BACKSTORE_FAILURE );
      return false;
   }

   ok = psocOpenMemFile( &pManager->memory, &pManager->pMemoryAddress, &pContext->errorHandler );   
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) {
      psocChainError( &pContext->errorHandler,
                      g_qsrErrorHandle,
                      QSR_OPEN_BACKSTORE_FAILURE );
      return false;
   }
      
   pManager->pHeader = *ppHeader = (psonMemoryHeader*) pManager->pMemoryAddress;
   (*ppHeader)->running = 1;

   /* Sets the global base address */
   g_pBaseAddr = (unsigned char *) pManager->pMemoryAddress;
   
   /* The memory allocator starts after the header */
   pStart = (unsigned char*)pManager->pMemoryAddress + PSON_BLOCK_SIZE;
   
   (*ppHeader)->allocatorOffset = SET_OFFSET( pStart );
   pAlloc = (psonMemAlloc *) pStart;
   
   errcode = psonMemAllocInit( pAlloc,
                               g_pBaseAddr,
                               pManager->memorySizeKB*1024,
                               pContext );
   if ( errcode != 0 ) {
      (*ppHeader) = NULL;
      psocSetError( &pContext->errorHandler,
                    g_psoErrorHandle,
                    errcode );
      return false;
   }

   /* The top folder  */
   ptr = psonMallocBlocks( pAlloc, PSON_ALLOC_API_OBJ, 1, pContext );
   if ( ptr == NULL ) {
      (*ppHeader) = NULL;
      psocSetError( &pContext->errorHandler,
                    g_psoErrorHandle,
                    PSO_NOT_ENOUGH_PSO_MEMORY );
      return false;
   }
   pFolder = (psonFolder *) ptr;

   errcode = psonMemObjectInit( &pFolder->memObject, 
                                PSON_IDENT_FOLDER,
                                &pFolder->blockGroup,
                                1 );
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler,
                    g_qsrErrorHandle,
                    errcode );
      return false;
   }
   psonTxStatusInit( &pManager->pHeader->topHashItem.txStatus, PSON_NULL_OFFSET );

   pFolder->nodeObject.txCounter      = 0;
   pFolder->nodeObject.myNameLength   = 0;
   pFolder->nodeObject.myNameOffset   = PSON_NULL_OFFSET;
   pFolder->nodeObject.txStatusOffset = 
      SET_OFFSET( &pManager->pHeader->topHashItem.txStatus );
   pFolder->nodeObject.myParentOffset = PSON_NULL_OFFSET;

   errcode = psonHashTxInit( &pFolder->hashObj, 
                             SET_OFFSET(&pFolder->memObject),
                             25, 
                             pContext );
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
      return false;
   }
   (*ppHeader)->treeMgrOffset = SET_OFFSET( ptr );
   (*ppHeader)->topHashItem.dataOffset = SET_OFFSET(&(*ppHeader)->topDescriptor);
   (*ppHeader)->topDescriptor.offset = SET_OFFSET( ptr );
   (*ppHeader)->topDescriptor.nodeOffset = SET_OFFSET( &pFolder->nodeObject );
   (*ppHeader)->topDescriptor.memOffset = SET_OFFSET( &pFolder->memObject );
   (*ppHeader)->topDescriptor.apiType = PSO_FOLDER;

   /* The Garbage Collection manager */
   ptr = psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 1, pContext );
   if ( ptr == NULL ) {
      (*ppHeader) = NULL;
      psocSetError( &pContext->errorHandler,
                    g_psoErrorHandle,
                    PSO_NOT_ENOUGH_PSO_MEMORY );
      return false;
   }

   processManager = (psonProcMgr *) ptr;
   
   ok = psonProcMgrInit( processManager, pContext );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) {
      (*ppHeader) = NULL;
      /* The error is set in the function itself */
      return false;
   }
   (*ppHeader)->processMgrOffset = SET_OFFSET( processManager );

   /* And finish with setting up the version (and eventually some "magic */
   /* cookie" to identify the file?) */

   strcpy( (*ppHeader)->cookie, "PSO" );
   (*ppHeader)->version = PSON_MEMORY_VERSION;
   (*ppHeader)->totalLength = pManager->memorySizeKB*1024;

   (*ppHeader)->sizeofPtr = SIZEOF_VOID_P;
#if WORDS_BIGENDIAN
   (*ppHeader)->bigEndian = true;
#else
   (*ppHeader)->bigEndian = false;
#endif
   (*ppHeader)->blockSize = PSON_BLOCK_SIZE;
   (*ppHeader)->alignmentStruct = PSOC_ALIGNMENT_STRUCT;
   (*ppHeader)->alignmentChar   = PSOC_ALIGNMENT_CHAR;
   (*ppHeader)->alignmentInt16  = PSOC_ALIGNMENT_INT16;
   (*ppHeader)->alignmentInt32  = PSOC_ALIGNMENT_INT32;
   (*ppHeader)->alignmentInt64  = PSOC_ALIGNMENT_INT64;

   (*ppHeader)->allocationUnit = PSON_ALLOCATION_UNIT;
   strncpy( (*ppHeader)->cpu_type, BUILD_CPU, 19 );
   strncpy( (*ppHeader)->compiler, COMPILER_NAME, 19);
   strncpy( (*ppHeader)->cxxcompiler, COMPILER_NAME, 19);
   strncpy( (*ppHeader)->quasarVersion, PACKAGE_VERSION, 10 );

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

bool qsrOpenMem( qsrMemoryManager   * pManager, 
                 const char         * memoryFileName,
                 size_t               memorySizekb,
                 psonMemoryHeader  ** ppHeader,
                 psonSessionContext * pContext )
{
   bool ok;
   
   psocMemoryFileStatus fileStatus;
   psocErrorHandler errorHandler;
   
   PSO_PRE_CONDITION( pManager       != NULL );
   PSO_PRE_CONDITION( memoryFileName != NULL );
   PSO_PRE_CONDITION( ppHeader       != NULL );
   PSO_PRE_CONDITION( pContext       != NULL );
   PSO_PRE_CONDITION( memorySizekb > 0 );

   *ppHeader = NULL;
   pManager->memorySizeKB = memorySizekb;
   
   psocInitErrorHandler( &errorHandler );
   
   psocInitMemoryFile( &pManager->memory, pManager->memorySizeKB, memoryFileName );
   
   psocBackStoreStatus( &pManager->memory, &fileStatus );
   
   if ( ! fileStatus.fileExist ) {
      psocSetError( &pContext->errorHandler,
                    g_qsrErrorHandle,
                    QSR_BACKSTORE_FILE_MISSING );
      return false;
   }
   
   ok = psocOpenMemFile( &pManager->memory, 
                         &pManager->pMemoryAddress, 
                         &errorHandler );   
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) {
      psocChainError( &pContext->errorHandler,
                      g_qsrErrorHandle,
                      QSR_ERROR_OPENING_MEMORY );
      return false;
   }
   
   pManager->pHeader = *ppHeader = (psonMemoryHeader*) pManager->pMemoryAddress;

   if ( (*ppHeader)->version != PSON_MEMORY_VERSION ) {
      (*ppHeader) = NULL;
      psocSetError( &pContext->errorHandler,
                    g_qsrErrorHandle,
                    QSR_INCOMPATIBLE_VERSIONS );
      return false;
   }

   /* Sets the global base address */
   g_pBaseAddr = (unsigned char *) pManager->pMemoryAddress;
  
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void qsrCloseMem( qsrMemoryManager * pManager,
                  psocErrorHandler * pError )
{
   PSO_PRE_CONDITION( pManager != NULL );
   PSO_PRE_CONDITION( pError   != NULL );

   psocCloseMemFile( &pManager->memory, pError );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

