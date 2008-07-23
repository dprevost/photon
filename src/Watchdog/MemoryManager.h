/* -*- C++ -*- */
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

#ifndef VDSW_MEMORY_MANAGER_H
#define VDSW_MEMORY_MANAGER_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Engine/Engine.h"
#include "Engine/MemoryHeader.h"
#include "Common/MemoryFile.h"
#include "Engine/SessionContext.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct vdswMemoryManager
{
   vdscMemoryFile memory;

   size_t memorySizeKB;
   
   void * pMemoryAddress;

   vdseMemoryHeader * pHeader;
   
};

typedef struct vdswMemoryManager vdswMemoryManager;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdswMemoryManagerInit( vdswMemoryManager * pManager );

void vdswMemoryManagerFini( vdswMemoryManager * pManager );
   
/** This function creates the VDS. */
int vdswCreateVDS( vdswMemoryManager  * pManager,
                   const char         * memoryFileName,
                   size_t               memorySize,
                   int                  filePerms,
                   vdseMemoryHeader  ** ppMemoryAddress,
                   vdseSessionContext * pContext );

/**
 * This function opens an existing VDS. This is the function that should
 * be used when the VDS already exist.
 */
int vdswOpenVDS( vdswMemoryManager * pManager, 
                 const char        * memoryFileName,
                 size_t              memorySize,
                 vdseMemoryHeader ** ppMemoryAddress );

void vdswCloseVDS( vdswMemoryManager * pManager,
                   vdscErrorHandler  * pError );

#if 0

/* ::msync() is called with the MS_SYNC flag by default */
static inline
int vdswSyncVDS( vdswMemoryManager * pManager,
                 vdscErrorHandler  * pError ) 
{
   int err = vdscSyncMemFile( &pManager->memory, pError );
   return err;
}

#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSE_MEMORY_MANAGER_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

