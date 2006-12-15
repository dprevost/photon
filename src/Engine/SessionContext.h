/*
 * Copyright (C) 2006 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef VDSE_SESSION_CONTEXT_H
#define VDSE_SESSION_CONTEXT_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Engine.h"
#include "ErrorHandler.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This structure holds information that's either process wide specific
 * or session specific. Regrouping this information in this single
 * structure simplify the code. 
 *
 * The pointers are to void* to avoid problems of circular dependency.
 */

typedef struct vdseSessionContext
{
   vdscErrorHandler errorHandler;
   
   /** Normally set to the process id (pid) of the process */
   vds_lock_T       lockValue;

   /** Offset to the currently locked object, if any.*/
   ptrdiff_t        lockObject;

   void* pTransaction;

   /** 
    * For allocating/freeing memory in sub-objects using the allocator
    * of the MemObject.
    */
   void* pCurrentMemObject;

   /** 
    * For requesting/freeing memory pages from the global allocator (used
    * by the allocator of the MemObject).
    */
   void *pAllocator;
   
   void* pLogFile;
   
} vdseSessionContext;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline void
vdseInitSessionContext( vdseSessionContext* pContext )
{
   VDS_PRE_CONDITION( pContext != NULL );
   
   vdscInitErrorHandler( &pContext->errorHandler );
   
   pContext->lockValue = getpid();
   pContext->lockObject = NULL_OFFSET;
   pContext->pTransaction      = NULL;
   pContext->pCurrentMemObject = NULL;
   pContext->pAllocator        = NULL;
   pContext->pLogFile          = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSE_SESSION_CONTEXT_H */

