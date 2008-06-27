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

#ifndef VDSA_COMMON_OBJECT_H
#define VDSA_COMMON_OBJECT_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Common/ThreadLock.h"
#include "Engine/Folder.h"
#include "API/Session.h"
#include "Engine/Session.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Data common to all api objects (for data containers). 
 */
typedef struct vdsaCommonObject
{
   /**
    * The type of object (as seen from the API, not the engine).
    *
    * Note: always put this first to help debug (for example, in gdb:
    * "print * (int *) some_vds_handle" will show the object type).
    */
   vdsaObjetType type;

   /** 
    * Pointer to our own cleanup object in the VDS. This object is used by 
    * the ProcessManager to hold object-specific information that might
    * be needed in case of a crash (current state of transactions, etc.).
    *
    * Not used yet.
    */
   vdseObjectContext *  pObjectContext;

   /** Pointer to the session we belong to. */
   struct vdsaSession* pSession;

   /** A folder item. It contains a pointer to the hash item in VDS memory. */
   vdseFolderItem  folderItem;

   /** A pointer to the object in VDS. */   
   void * pMyVdsObject;

} vdsaCommonObject;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** Common function for opening data containers. */
VDSF_API_EXPORT
int vdsaCommonObjOpen( vdsaCommonObject   * pObject,
                       enum vdsObjectType   objectType, 
                       bool                 editMode,
                       const char         * objectName,
                       size_t               nameLengthInBytes );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** Common function for closing data containers. */
VDSF_API_EXPORT
int vdsaCommonObjClose( vdsaCommonObject * pObject );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Lock the current object. 
 *
 * \param[in] pObject Pointer to the object to lock.
*/
static inline
int vdsaCommonLock( vdsaCommonObject * pObject )
{
   VDS_PRE_CONDITION( pObject != NULL );

   if ( g_protectionIsNeeded ) {
      return vdscTryAcquireThreadLock( &pObject->pSession->mutex, 
                                       LOCK_TIMEOUT );
   }
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Unlock the current object.
 *
 * \param[in] pObject Pointer to the object to unlock.
 */
static inline
void vdsaCommonUnlock( vdsaCommonObject * pObject )
{
   VDS_PRE_CONDITION( pObject != NULL );

   if ( g_protectionIsNeeded ) {
      vdscReleaseThreadLock( &pObject->pSession->mutex );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Called by the vdsaSession upon session termination.
 *
 * Setting vdsaCommonObject::pObjectContext to NULL indicates that a
 * process or a session has terminated and that no further access to
 * the VDS is allowed/possible!
 *
 * \param[in] pObject Pointer to the object to close.
 *
 * \todo Not sure if this makes sense anymore. Revisit!
 */
static inline
void vdsaCommonCloseObject( vdsaCommonObject * pObject )
{
   VDS_PRE_CONDITION( pObject != NULL );

   pObject->pObjectContext = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSA_COMMON_OBJECT_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

