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

#ifndef VDSA_SESSION_H
#define VDSA_SESSION_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "API/api.h"
#include "Common/ThreadLock.h"
#include "Engine/SessionContext.h"
#include "API/Process.h"
#include "API/ListReaders.h"

#if !defined(LOCK_TIMEOUT)
# define LOCK_TIMEOUT 10000 
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct vdseSession;
struct vdseMemoryHeader;
struct vdsaCommonObject;

/**
 * This class handles transactions and other session wide concerns. For 
 * example, opening, closing, creating and destroying shared memory 
 * objects.
 */

typedef struct vdsaSession
{
   vdsaObjetType type;
   
   vdseSessionContext context;

   /** Pointer to the header of the shared memory */
   struct vdseMemoryHeader* pHeader;

   struct vdseSession* pCleanup;

   bool terminated;
   
   int numberOfObjects;
   
   int numberOfEdits;
   
   /** Our lock to serialize access to this object, if needed. */
   vdscThreadLock  mutex;
   
   vdsaListReaders listReaders;
   
} vdsaSession;


/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * No lock is taken on the vdsaSession.
 */
VDSF_API_EXPORT
int vdsaCloseSession( vdsaSession* pSession );

VDSF_API_EXPORT
int vdsaSessionCloseObj( vdsaSession             * pSession,
                         struct vdsaCommonObject * pObject );

VDSF_API_EXPORT
int vdsaSessionOpenObj( vdsaSession             * pSession,
                        vdsObjectType             objectType,
                        vdsaEditMode              editMode,
                        const char              * objectName,
                        size_t                    nameLengthInBytes,
                        struct vdsaCommonObject * pObject );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
bool vdsaSessionLock( vdsaSession * pSession )
{
   bool ok = true;

   VDS_PRE_CONDITION( pSession != NULL );
   
   if ( g_protectionIsNeeded ) {
      ok = vdscTryAcquireThreadLock( &pSession->mutex, LOCK_TIMEOUT );
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
void vdsaSessionUnlock( vdsaSession* pSession )
{
   if ( g_protectionIsNeeded ) {
      vdscReleaseThreadLock( &pSession->mutex );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSA_SESSION_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

