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
#include "Watchdog/ValidateFolder.h"
#include "Watchdog/ValidateHashMap.h"
#include "Watchdog/ValidateQueue.h"
#include "Engine/Folder.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdswCheckFolderContent( struct vdseFolder  * pFolder, 
                            int                  verbose,
                            int                  spaces,
                            vdseSessionContext * pContext )
{
   enum ListErrors listErr;
   size_t bucket, previousBucket;
   ptrdiff_t offset, previousOffset;
   vdseHashItem * pItem;
   vdseObjectDescriptor* pDesc = NULL;
   void * pObject;
   int pDesc_invalid_api_type = 0;
   enum vdswValidation valid;
   char message[VDS_MAX_NAME_LENGTH*4 + 30];
   size_t lengthName;
#if VDS_SUPPORT_i18n
   mbstate_t ps;
   const wchar_t * name;
#endif
   
   /* The easy case */
   if ( pFolder->hashObj.numberOfItems == 0 )
      return 0;
   
   /*
    * We loop on all the hash items until either:
    * - we find one item which is not marked as deleted by the
    *   current transaction (we return false)
    * - or the end (we return true)
    */

   listErr = vdseHashGetFirst( &pFolder->hashObj,
                               &bucket, 
                               &offset );
   while ( listErr == LIST_OK )
   {
      GET_PTR( pItem, offset, vdseHashItem );
      GET_PTR( pDesc, pItem->dataOffset, vdseObjectDescriptor );
      GET_PTR( pObject, pDesc->offset, void );
      
      if ( verbose )
      {
         memset( message, 0, VDS_MAX_NAME_LENGTH*4+30 );
         strcpy( message, "Object name: " );
#if VDS_SUPPORT_i18n
         memset( &ps, 0, sizeof(mbstate_t) );
         name = pDesc->originalName;
         lengthName = wcsrtombs( &message[strlen(message)], 
                                 &name,
                                 VDS_MAX_NAME_LENGTH*4,
                                 &ps );
         if ( lengthName == (size_t) -1 )
         {
            /* A conversion error */
            strcat( message, " wcsrtombs() conversion error... sorry" );
         }
#else
         strncat( message, pDesc->originalName, pDesc->nameLengthInBytes );
#endif
         vdswEcho( spaces, message );
      }
      switch( pDesc->apiType )
      {
         case VDS_FOLDER:
            valid = vdswValidateFolder( (vdseFolder *)pObject, verbose, 
                                        spaces, pContext );
            break;
         case VDS_HASH_MAP:
            valid = vdswValidateHashMap( (struct vdseHashMap *)pObject, 
                                         verbose, spaces, pContext );
            break;
         case VDS_QUEUE:
            valid = vdswValidateQueue( (struct vdseQueue *)pObject, 
                                       verbose, spaces );
            break;
         default:
            VDS_INV_CONDITION( pDesc_invalid_api_type );
      }
      
      previousBucket = bucket;
      previousOffset = offset;
      listErr = vdseHashGetNext( &pFolder->hashObj,
                                 previousBucket,
                                 previousOffset,
                                 &bucket, 
                                 &offset );

      if ( valid == VDSW_DELETE_OBJECT )
      {
         switch( pDesc->apiType )
         {
            case VDS_FOLDER:
               vdseFolderFini( (vdseFolder *)pObject, pContext );
               break;
            case VDS_HASH_MAP:
               vdseHashMapFini( (struct vdseHashMap *)pObject, pContext );
               break;
            case VDS_QUEUE:
               vdseQueueFini( (struct vdseQueue *)pObject, pContext );
               break;
            default:
               VDS_INV_CONDITION( pDesc_invalid_api_type );
         }
         vdseHashDeleteAt( &pFolder->hashObj,
                           previousBucket,
                           pItem,
                           pContext );
      }
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum vdswValidation 
vdswValidateFolder( struct vdseFolder  * pFolder,
                    int                  verbose,
                    int                  spaces,
                    vdseSessionContext * pContext )
{
   vdseTxStatus * txFolderStatus;
   int rc;
      
   spaces += 2;
   
   GET_PTR( txFolderStatus, pFolder->nodeObject.txStatusOffset, vdseTxStatus );

   if ( txFolderStatus->txOffset != NULL_OFFSET )
   {
      /*
       * So we have an interrupted transaction. What kind? 
       *   FLAG                 ACTION          Comment
       *   0                    remove object   Added object non-committed
       *   MARKED_AS_DESTROYED  reset txStatus  
       *   REMOVE_IS_COMMITTED  remove object
       *
       * Action is the equivalent of what a rollback would do.
       */
      if ( txFolderStatus->statusFlag == 0 )
      {
         if ( verbose )
            vdswEcho( spaces, "Object added but not committed - object removed" );
         return VDSW_DELETE_OBJECT;
      }         
      if ( txFolderStatus->statusFlag == VDSE_REMOVE_IS_COMMITTED )
      {
         if ( verbose )
            vdswEcho( spaces, "Object deleted and committed - object removed" );
         return VDSW_DELETE_OBJECT;
      }
      if ( verbose )
         vdswEcho( spaces, "Object deleted but not committed - object is kept" );
      
      txFolderStatus->txOffset = NULL_OFFSET;
      txFolderStatus->statusFlag = 0;
   }
   
   if ( txFolderStatus->usageCounter != 0 )
   {
      txFolderStatus->usageCounter = 0;
      if ( verbose )
         vdswEcho( spaces, "Usage counter set to zero" );
   }
   if ( txFolderStatus->parentCounter != 0 )
   {
      txFolderStatus->parentCounter = 0;
      if ( verbose )
         vdswEcho( spaces, "Parent counter set to zero" );
   }
   if ( pFolder->nodeObject.txCounter != 0 )
   {
      pFolder->nodeObject.txCounter = 0;
      if ( verbose )
         vdswEcho( spaces, "Transaction counter set to zero" );
   }

   rc = vdswCheckFolderContent( pFolder, verbose, spaces, pContext );

   return VDSW_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

