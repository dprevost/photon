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

#include "Common/Common.h"
#include "API/CommonObject.h"
#include "API/Session.h"
#include <vdsf/vds.h>
#include <vdsf/vdsErrors.h>

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsaCommonObjectOpen( vdsaCommonObject   * pObject,
                          enum vdsObjectType   objectType, 
                          const char         * objectName,
                          size_t               nameLengthInBytes )
{
   int errcode = VDS_OBJECT_CANNOT_GET_LOCK;
   vdseObjectDescriptor * pDesc;
   
   VDS_PRE_CONDITION( pObject    != NULL );
   VDS_PRE_CONDITION( objectName != NULL );
   VDS_PRE_CONDITION( objectType > 0 && objectType < VDS_LAST_OBJECT_TYPE );
   VDS_PRE_CONDITION( pObject->pObjectContext == NULL );
   VDS_PRE_CONDITION( nameLengthInBytes > 0 );

   if ( pObject->pSession == NULL )
      return VDS_PROCESS_NOT_INITIALIZED;

   if ( vdsaCommonLock( pObject ) == 0 )
   {
      errcode = vdsaSessionOpenObj( pObject->pSession,
                                    objectType, 
                                    objectName,
                                    nameLengthInBytes,
                                    pObject );
      if ( errcode == 0 )
      {
         GET_PTR( pDesc, pObject->folderItem.pHashItem->dataOffset,
                          vdseObjectDescriptor );
         GET_PTR( pObject->pMyVdsObject, pDesc->offset, void );
      }
      vdsaCommonUnlock( pObject );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsCommonObjectClose( vdsaCommonObject * pObject )
{
   int errcode = 0;

   VDS_PRE_CONDITION( pObject != NULL );

//   if ( pObject->pObjectContext == NULL )
//      return VDS_OBJECT_NOT_INITIALIZED;
   
   if ( pObject->pSession == NULL )
      return VDS_PROCESS_NOT_INITIALIZED;

   /* No need to lock the api session. The caller already did it! */
   errcode = vdsaSessionCloseObj( pObject->pSession, pObject );
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

