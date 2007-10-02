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
#include <vdsf/vds_c.h>
#include "API/Folder.h"
#include "API/Session.h"
#include <vdsf/vdsErrors.h>
#include "API/CommonObject.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsFolderOpen( VDS_HANDLE  sessionHandle,
                   const char* folderName,
                   VDS_HANDLE* objectHandle )
{
   vdsaSession * pSession;
   vdsaFolder * pFolder = NULL;
   int errcode;
   
   pSession = (vdsaSession*) sessionHandle;
   if ( pSession == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pSession->type != VDSA_SESSION )
      return VDS_WRONG_TYPE_HANDLE;

   if ( folderName == NULL )
      return VDS_INVALID_OBJECT_NAME;
   
   if ( objectHandle == NULL )
      return VDS_NULL_HANDLE;
   
   *objectHandle = NULL;
   
   pFolder = (vdsaFolder *) malloc(sizeof(vdsaFolder));
   if (  pFolder == NULL )
      return VDS_NOT_ENOUGH_HEAP_MEMORY;
   
   memset( pFolder, 0, sizeof(vdsaFolder) );
   pFolder->type = VDSA_FOLDER;
   pFolder->object.pSession = pSession;

   errcode = vdsaCommonObjectOpen( &pFolder->object,
                                   VDS_FOLDER,
                                   folderName );
   if ( errcode == 0 )
      *objectHandle = (VDS_HANDLE) pFolder;

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsFolderClose( VDS_HANDLE objectHandle )
{
   vdsaFolder * pFolder;
   int errcode;
   
   pFolder = (vdsaFolder *) objectHandle;
   if ( pFolder == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pFolder->type != VDSA_FOLDER )
      return VDS_WRONG_TYPE_HANDLE;
   
   errcode = vdsCommonObjectClose( &pFolder->object );
   if ( errcode == 0 )
   {
      free( pFolder );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
#if 0

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsFolderGetFirstItem( vdsFolderItem* pFolderItem )
{
   return VDS_INTERNAL_ERROR;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsFolderGetNext( vdsFolderItem * pFolderItem )
{
   return VDS_INTERNAL_ERROR;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
#endif

