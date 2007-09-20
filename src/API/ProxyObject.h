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

#ifndef VDSA_OBJECT_H
#define VDSA_OBJECT_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Common/ThreadLock.h"
#include "Engine/TreeNode.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Forward declaration */
struct vdsaSession;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct vdsaProxyObject
{
   /** 
    * Pointer to our own cleanup object in the VDS. This object is used by 
    * the ProcessManager to hold object-specific information that might
    * be needed in case of a crash (current state of transactions, etc.).
    */
   void*  pObjectContext;

   /** Pointer to the session we belong to. */
   vdsaSession* pSession;

   /** Our copy of the descriptor object. */
   vdseObjectDescriptor desc;

   void * pIterator;
   
} vdsaProxyObject;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** Lock the current object. */
int vdsaProxyLock( vdsaProxyObject * pObject )
{
   if ( g_protectionIsNeeded )
   {
      return vdscTryAcquireThreadLock( &pObject->pSession->mutex, 
                                       LOCK_TIMEOUT );
   }
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** Lock the current object. */
void vdsaProxyUnlock( vdsaProxyObject * pObject )
{
   if ( g_protectionIsNeeded )
   {
      vdscReleaseThreadLock( &pObject->pSession->mutex );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Called by the vdsaSession upon session termination. 
 * Setting vdsaProxyObject.pObjectContext to NULL indicates that a 
 * process or a session has terminated and that no further access to 
 * the VDS is allowed/possible!
 *
 * \todo Not sure if this makes sense anymore. Revisit!
 */
void vdsaProxyCloseObject( vdsaProxyObject * pObject )
{
   pObject->pObjectContext = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSA_OBJECT_H */

