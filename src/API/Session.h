/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef PSA_SESSION_H
#define PSA_SESSION_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "API/api.h"
#include "Common/ThreadLock.h"
#include "Nucleus/SessionContext.h"
#include "API/Process.h"
#include "API/ListReaders.h"

#if !defined(LOCK_TIMEOUT)
# define LOCK_TIMEOUT 10000 
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psnSession;
struct psnMemoryHeader;
struct psaCommonObject;

/**
 * This class handles transactions and other session wide concerns. For 
 * example, opening, closing, creating and destroying shared memory 
 * objects.
 */

typedef struct psaSession
{
   psaObjetType type;
   
   psnSessionContext context;

   /** Pointer to the header of the shared memory */
   struct psnMemoryHeader* pHeader;

   struct psnSession* pCleanup;

   bool terminated;
   
   int numberOfObjects;
   
   int numberOfEdits;
   
   /** Our lock to serialize access to this object, if needed. */
   pscThreadLock  mutex;
   
   psaListReaders listReaders;
   
} psaSession;


/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * No lock is taken on the psaSession.
 */
VDSF_API_EXPORT
int psaCloseSession( psaSession* pSession );

VDSF_API_EXPORT
int psaSessionCloseObj( psaSession             * pSession,
                        struct psaCommonObject * pObject );

VDSF_API_EXPORT
int psaSessionOpenObj( psaSession             * pSession,
                       vdsObjectType            objectType,
                       psaEditMode              editMode,
                       const char             * objectName,
                       size_t                   nameLengthInBytes,
                       struct psaCommonObject * pObject );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
bool psaSessionLock( psaSession * pSession )
{
   bool ok = true;

   VDS_PRE_CONDITION( pSession != NULL );
   
   if ( g_protectionIsNeeded ) {
      ok = pscTryAcquireThreadLock( &pSession->mutex, LOCK_TIMEOUT );
      VDS_POST_CONDITION( ok == true || ok == false );
   }
   
   return ok;
}
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** Unlock the current session. */
//#ifndef __cplusplus
static 
//#endif
__inline
void psaSessionUnlock( psaSession * pSession )
{
   if ( g_protectionIsNeeded ) {
      pscReleaseThreadLock( &pSession->mutex );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSA_SESSION_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

