/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of photon (photonsoftware.org).
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

#ifndef PSN_SESSION_CONTEXT_H
#define PSN_SESSION_CONTEXT_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Nucleus/Engine.h"
#include "Common/ErrorHandler.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This structure holds information that's either process wide specific
 * or session specific. Regrouping this information in this single
 * structure simplify the code. 
 *
 * This struct is allocated in the process space, not in the VDS memory.
 *
 * The pointers are to void* to avoid problems of circular dependency.
 */
struct psnSessionContext
{
   pscErrorHandler errorHandler;
   
   /** Normally set to the process id (pid) of the process */
   pid_t pidLocker;

   /** Offset to the currently locked object, if any.*/
   ptrdiff_t lockObject;

   void* pTransaction;

   void* pSession;

   ptrdiff_t * lockOffsets;
   int       * numLocks;
   
   /** 
    * For requesting/freeing memory pages from the global allocator (used
    * by the allocator of the MemObject).
    */
   void *pAllocator;
   
   void* pLogFile;
   
};

typedef struct psnSessionContext psnSessionContext;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline 
void psnInitSessionContext( psnSessionContext * pContext )
{
   PSO_PRE_CONDITION( pContext != NULL );
   
   pscInitErrorHandler( &pContext->errorHandler );
   
   pContext->pidLocker    = getpid();
   pContext->lockObject   = PSN_NULL_OFFSET;
   pContext->pTransaction = NULL;
   pContext->pAllocator   = NULL;
   pContext->pLogFile     = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void psnSessionAddLock( psnSessionContext * pSession,
                         ptrdiff_t            memObjectOffset )
{
   PSO_PRE_CONDITION( pSession != NULL );
   PSO_PRE_CONDITION( *pSession->numLocks < PSN_MAX_LOCK_DEPTH );
   PSO_PRE_CONDITION( memObjectOffset != PSN_NULL_OFFSET );
   
   pSession->lockOffsets[*pSession->numLocks] = memObjectOffset;
   (*pSession->numLocks)++;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void psnSessionRemoveLock( psnSessionContext * pSession,
                            ptrdiff_t            memObjectOffset )
{
   int i, j, n;
   
   PSO_PRE_CONDITION( pSession != NULL );
   PSO_PRE_CONDITION( *pSession->numLocks > 0 );
   PSO_PRE_CONDITION( memObjectOffset != PSN_NULL_OFFSET );
   
   n = *pSession->numLocks;

   for ( i = 0; i < *pSession->numLocks; ++i ) {
      if ( pSession->lockOffsets[i] == memObjectOffset ) {
         /* Shift the following pointers */
         for ( j = i+1; j < *pSession->numLocks; ++j ) {
            pSession->lockOffsets[j-1] = pSession->lockOffsets[j];
         }
         
         pSession->lockOffsets[*pSession->numLocks-1] = PSN_NULL_OFFSET;         
         (*pSession->numLocks)--;
      }
   }
   
   /* Will fail if pMemObject was not in the array */
   PSO_POST_CONDITION( n == (*pSession->numLocks + 1) );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSN_SESSION_CONTEXT_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
