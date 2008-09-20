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

#ifndef PSOA_SESSION_H
#define PSOA_SESSION_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "API/api.h"
#include "Common/ThreadLock.h"
#include "Nucleus/SessionContext.h"
#include "API/Process.h"
#include "API/ListReaders.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psonSession;
struct psonMemoryHeader;
struct psoaCommonObject;

/**
 * This class handles transactions and other session wide concerns. For 
 * example, opening, closing, creating and destroying shared memory 
 * objects.
 */

typedef struct psoaSession
{
   psoaObjetType type;
   
   psonSessionContext context;

   /** Pointer to the header of the shared memory */
   struct psonMemoryHeader* pHeader;

   struct psonSession* pCleanup;

   bool terminated;
   
   int numberOfObjects;
   
   int numberOfEdits;
   
   /** Our lock to serialize access to this object, if needed. */
   psocThreadLock  mutex;
   
   psoaListReaders listReaders;
   
} psoaSession;


/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * No lock is taken on the psoaSession.
 */
PHOTON_API_EXPORT
int psoaCloseSession( psoaSession* pSession );

PHOTON_API_EXPORT
int psoaSessionCloseObj( psoaSession             * pSession,
                        struct psoaCommonObject * pObject );

PHOTON_API_EXPORT
int psoaSessionOpenObj( psoaSession             * pSession,
                       psoObjectType            objectType,
                       psoaEditMode              editMode,
                       const char             * objectName,
                       size_t                   nameLengthInBytes,
                       struct psoaCommonObject * pObject );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
bool psoaSessionLock( psoaSession * pSession )
{
   bool ok = true;

   PSO_PRE_CONDITION( pSession != NULL );
   
   if ( g_protectionIsNeeded ) {
      ok = psocTryAcquireThreadLock( &pSession->mutex, PSON_LOCK_TIMEOUT );
      PSO_POST_CONDITION( ok == true || ok == false );
   }
   
   return ok;
}
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** Unlock the current session. */
//#ifndef __cplusplus
static 
//#endif
__inline
void psoaSessionUnlock( psoaSession * pSession )
{
   if ( g_protectionIsNeeded ) {
      psocReleaseThreadLock( &pSession->mutex );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSOA_SESSION_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

