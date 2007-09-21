/*
 * Copyright (C) 2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

#if !defined(LOCK_TIMEOUT)
# define LOCK_TIMEOUT 10000 
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct vdseSession;
struct vdseMemoryHeader;

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

   bool initialized;
   
   /** Our lock to serialize access to this object, if needed. */
   vdscThreadLock  mutex;
   
} vdsaSession;


/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
   
static inline
int vdsaSessionLock( vdsaSession * pSession )
{
   if ( g_protectionIsNeeded )
   {
      return vdscTryAcquireThreadLock( &pSession->mutex, 
                                       LOCK_TIMEOUT );
   }
   return 0;
}
   
/** Unlock the current session. */
static inline
void vdsaSessionUnlock( vdsaSession* pSession )
{
   if ( g_protectionIsNeeded )
      vdscReleaseThreadLock( &pSession->mutex );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSA_SESSION_H */