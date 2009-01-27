/* -*- C++ -*- */
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

#ifndef QSR_MEMORY_MANAGER_H
#define QSR_MEMORY_MANAGER_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Nucleus/Engine.h"
#include "Nucleus/MemoryHeader.h"
#include "Common/MemoryFile.h"
#include "Nucleus/SessionContext.h"

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct qsrMemoryManager
{
   psocMemoryFile memory;

   size_t memorySizeKB;
   
   void * pMemoryAddress;

   psonMemoryHeader * pHeader;
   
};

typedef struct qsrMemoryManager qsrMemoryManager;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void qsrMemoryManagerInit( qsrMemoryManager * pManager );

void qsrMemoryManagerFini( qsrMemoryManager * pManager );
   
/** This function creates the shared memory. */
bool qsrCreateMem( qsrMemoryManager   * pManager,
                   const char         * memoryFileName,
                   size_t               memorySize,
                   int                  filePerms,
                   psonMemoryHeader  ** ppMemoryAddress,
                   psonSessionContext * pContext );

/**
 * This function opens an existing shared memory. 
 */
bool qsrOpenMem( qsrMemoryManager   * pManager, 
                 const char         * memoryFileName,
                 size_t               memorySize,
                 psonMemoryHeader  ** ppMemoryAddress,
                 psonSessionContext * pContext );

void qsrCloseMem( qsrMemoryManager * pManager,
                  psocErrorHandler * pError );

#if 0

/* ::msync() is called with the MS_SYNC flag by default */
static inline
bool qsrSyncMem( qsrMemoryManager * pManager,
                 psocErrorHandler * pError ) 
{
   return psocSyncMemFile( &pManager->memory, pError );
}

#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

#endif /* QSR_MEMORY_MANAGER_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

