/*
 * Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#ifndef PSOA_COMMON_OBJECT_H
#define PSOA_COMMON_OBJECT_H

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
typedef struct psoaCommonObject
{
   /**
    * The type of object (as seen from the API, not the engine).
    *
    * Note: always put this first to help debug (for example, in gdb:
    * "print * (int *) some_pso_handle" will show the object type).
    */
   psoaObjetType type;

   /** 
    * Pointer to our own cleanup object in memory. This object is used by 
    * the ProcessManager to hold object-specific information that might
    * be needed in case of a crash (current state of transactions, etc.).
    *
    * Not used often, yet.
    */
   psonObjectContext *  pObjectContext;

   /** Pointer to the session we belong to. */
   struct psoaSession* pSession;

   /** A folder item. It contains a pointer to the hash item in memory. */
   psonFolderItem  folderItem;
   
   /** A pointer to the object in shared memory. */   
   void * pMyMemObject;

} psoaCommonObject;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** Common function for opening data containers. */
PHOTON_API_EXPORT
int psoaCommonObjOpen( psoaCommonObject  * pObject,
                      enum psoObjectType   objectType, 
                      psoaEditMode         editMode,
                      const char         * objectName,
                      uint32_t             nameLengthInBytes );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** Common function for closing data containers. */
PHOTON_API_EXPORT
int psoaCommonObjClose( psoaCommonObject * pObject );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Called by the psoaSession upon session termination.
 *
 * Setting psoaCommonObject::pObjectContext to NULL indicates that a
 * process or a session has terminated and that no further access to
 * the shared memory is allowed/possible!
 *
 * \param[in] pObject Pointer to the object to close.
 *
 * \todo Not sure if this makes sense anymore. Revisit!
 */
static inline
void psoaCommonCloseObject( psoaCommonObject * pObject )
{
   PSO_PRE_CONDITION( pObject != NULL );

   pObject->pObjectContext = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSOA_COMMON_OBJECT_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

