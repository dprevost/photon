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

#include "Common/Common.h"
#include "API/CommonObject.h"
#include "API/Session.h"
#include <photon/photon.h>
#include <photon/psoErrors.h>

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaCommonObjOpen( psoaCommonObject  * pObject,
                      enum psoObjectType   objectType,
                      psoaEditMode         editMode,
                      const char         * objectName,
                      uint32_t             nameLengthInBytes )
{
   int errcode = PSO_OBJECT_CANNOT_GET_LOCK;
   psonObjectDescriptor * pDesc;
   
   PSO_PRE_CONDITION( pObject    != NULL );
   PSO_PRE_CONDITION( objectName != NULL );
   PSO_PRE_CONDITION( objectType > 0 && objectType < PSO_LAST_OBJECT_TYPE );
   PSO_PRE_CONDITION( pObject->pObjectContext == NULL );
   PSO_PRE_CONDITION( nameLengthInBytes > 0 );

   if ( pObject->pSession == NULL ) return PSO_PROCESS_NOT_INITIALIZED;

   errcode = psoaSessionOpenObj( pObject->pSession,
                                 objectType,
                                 editMode,
                                 objectName,
                                 nameLengthInBytes,
                                 pObject );
   if ( errcode == 0 ) {
      GET_PTR( pDesc, pObject->folderItem.pHashItem->dataOffset,
                       psonObjectDescriptor );
      GET_PTR( pObject->pMyMemObject, pDesc->offset, void );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaCommonObjClose( psoaCommonObject * pObject )
{
   int errcode = 0;

   PSO_PRE_CONDITION( pObject != NULL );
   PSO_PRE_CONDITION( pObject->pObjectContext != NULL );
   
   if ( pObject->pSession == NULL ) return PSO_PROCESS_NOT_INITIALIZED;

   errcode = psoaSessionCloseObj( pObject->pSession, pObject );
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

