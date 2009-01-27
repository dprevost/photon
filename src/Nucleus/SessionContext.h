/*
 * Copyright (C) 2006-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#ifndef PSON_SESSION_CONTEXT_H
#define PSON_SESSION_CONTEXT_H

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
 * This struct is allocated in the process space, not in shared memory.
 *
 * The pointers are to void* to avoid problems of circular dependency.
 */
struct psonSessionContext
{
   psocErrorHandler errorHandler;
   
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

typedef struct psonSessionContext psonSessionContext;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline 
void psonInitSessionContext( psonSessionContext * pContext )
{
   PSO_PRE_CONDITION( pContext != NULL );
   
   psocInitErrorHandler( &pContext->errorHandler );
   
   pContext->pidLocker    = getpid();
   pContext->lockObject   = PSON_NULL_OFFSET;
   pContext->pTransaction = NULL;
   pContext->pAllocator   = NULL;
   pContext->pLogFile     = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void psonSessionAddLock( psonSessionContext * pSession,
                         ptrdiff_t            memObjectOffset )
{
   PSO_PRE_CONDITION( pSession != NULL );
   PSO_PRE_CONDITION( *pSession->numLocks < PSON_MAX_LOCK_DEPTH );
   PSO_PRE_CONDITION( memObjectOffset != PSON_NULL_OFFSET );
   
   pSession->lockOffsets[*pSession->numLocks] = memObjectOffset;
   (*pSession->numLocks)++;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void psonSessionRemoveLock( psonSessionContext * pSession,
                            ptrdiff_t            memObjectOffset )
{
   int i, j, n;
   
   PSO_PRE_CONDITION( pSession != NULL );
   PSO_PRE_CONDITION( *pSession->numLocks > 0 );
   PSO_PRE_CONDITION( memObjectOffset != PSON_NULL_OFFSET );
   
   n = *pSession->numLocks;

   for ( i = 0; i < *pSession->numLocks; ++i ) {
      if ( pSession->lockOffsets[i] == memObjectOffset ) {
         /* Shift the following pointers */
         for ( j = i+1; j < *pSession->numLocks; ++j ) {
            pSession->lockOffsets[j-1] = pSession->lockOffsets[j];
         }
         
         pSession->lockOffsets[*pSession->numLocks-1] = PSON_NULL_OFFSET;         
         (*pSession->numLocks)--;
      }
   }
   
   /* Will fail if pMemObject was not in the array */
   PSO_POST_CONDITION( n == (*pSession->numLocks + 1) );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSON_SESSION_CONTEXT_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

