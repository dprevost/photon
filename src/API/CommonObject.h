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

#ifndef PSA_COMMON_OBJECT_H
#define PSA_COMMON_OBJECT_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Common/ThreadLock.h"
#include "Nucleus/Folder.h"
#include "API/Session.h"
#include "Nucleus/Session.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Data common to all api objects (for data containers). 
 */
typedef struct psaCommonObject
{
   /**
    * The type of object (as seen from the API, not the engine).
    *
    * Note: always put this first to help debug (for example, in gdb:
    * "print * (int *) some_pso_handle" will show the object type).
    */
   psaObjetType type;

   /** 
    * Pointer to our own cleanup object in the VDS. This object is used by 
    * the ProcessManager to hold object-specific information that might
    * be needed in case of a crash (current state of transactions, etc.).
    *
    * Not used often, yet.
    */
   psnObjectContext *  pObjectContext;

   /** Pointer to the session we belong to. */
   struct psaSession* pSession;

   /** A folder item. It contains a pointer to the hash item in VDS memory. */
   psnFolderItem  folderItem;

   /** A pointer to the object in VDS. */   
   void * pMyVdsObject;

} psaCommonObject;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** Common function for opening data containers. */
VDSF_API_EXPORT
int psaCommonObjOpen( psaCommonObject    * pObject,
                      enum psoObjectType   objectType, 
                      psaEditMode          editMode,
                      const char         * objectName,
                      size_t               nameLengthInBytes );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** Common function for closing data containers. */
VDSF_API_EXPORT
int psaCommonObjClose( psaCommonObject * pObject );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Lock the current object. 
 *
 * \param[in] pObject Pointer to the object to lock.
*/
static inline
bool psaCommonLock( psaCommonObject * pObject )
{
   bool ok = true;
   
   PSO_PRE_CONDITION( pObject != NULL );

   if ( g_protectionIsNeeded ) {
      ok = pscTryAcquireThreadLock( &pObject->pSession->mutex, LOCK_TIMEOUT );
      PSO_POST_CONDITION( ok == true || ok == false );
   }
   
   return ok;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Unlock the current object.
 *
 * \param[in] pObject Pointer to the object to unlock.
 */
static inline
void psaCommonUnlock( psaCommonObject * pObject )
{
   PSO_PRE_CONDITION( pObject != NULL );

   if ( g_protectionIsNeeded ) {
      pscReleaseThreadLock( &pObject->pSession->mutex );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Called by the psaSession upon session termination.
 *
 * Setting psaCommonObject::pObjectContext to NULL indicates that a
 * process or a session has terminated and that no further access to
 * the VDS is allowed/possible!
 *
 * \param[in] pObject Pointer to the object to close.
 *
 * \todo Not sure if this makes sense anymore. Revisit!
 */
static inline
void psaCommonCloseObject( psaCommonObject * pObject )
{
   PSO_PRE_CONDITION( pObject != NULL );

   pObject->pObjectContext = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSA_COMMON_OBJECT_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

