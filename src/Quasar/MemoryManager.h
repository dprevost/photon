/* -*- C++ -*- */
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

#ifndef PSOQ_MEMORY_MANAGER_H
#define PSOQ_MEMORY_MANAGER_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Nucleus/Engine.h"
#include "Nucleus/MemoryHeader.h"
#include "Common/MemoryFile.h"
#include "Nucleus/SessionContext.h"

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psoqMemoryManager
{
   psocMemoryFile memory;

   size_t memorySizeKB;
   
   void * pMemoryAddress;

   psonMemoryHeader * pHeader;
   
};

typedef struct psoqMemoryManager psoqMemoryManager;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psoqMemoryManagerInit( psoqMemoryManager * pManager );

void psoqMemoryManagerFini( psoqMemoryManager * pManager );
   
/** This function creates the shared memory. */
bool psoqCreateVDS( psoqMemoryManager  * pManager,
                    const char         * memoryFileName,
                    size_t               memorySize,
                    int                  filePerms,
                    psonMemoryHeader  ** ppMemoryAddress,
                    psonSessionContext * pContext );

/**
 * This function opens an existing VDS. This is the function that should
 * be used when the shared memory already exist.
 */
bool psoqOpenVDS( psoqMemoryManager  * pManager, 
                  const char         * memoryFileName,
                  size_t               memorySize,
                  psonMemoryHeader  ** ppMemoryAddress,
                  psonSessionContext * pContext );

void psoqCloseVDS( psoqMemoryManager * pManager,
                   psocErrorHandler  * pError );

#if 0

/* ::msync() is called with the MS_SYNC flag by default */
static inline
bool psoqSyncVDS( psoqMemoryManager * pManager,
                  psocErrorHandler  * pError ) 
{
   return psocSyncMemFile( &pManager->memory, pError );
}

#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

#endif /* PSON_MEMORY_MANAGER_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

