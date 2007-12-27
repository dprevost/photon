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
#include "Watchdog/VerifyCommon.h"
#include "Engine/Folder.h"
#include "Engine/HashMap.h"
#include "Engine/Queue.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdswCheckFolderContent( vdswVerifyStruct   * pVerify,
                            struct vdseFolder  * pFolder, 
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

   listErr = vdseHashGetFirst( &pFolder->hashObj,
                               &bucket, 
                               &offset );
   while ( listErr == LIST_OK ) {
      GET_PTR( pItem, offset, vdseHashItem );
      GET_PTR( pDesc, pItem->dataOffset, vdseObjectDescriptor );
      GET_PTR( pObject, pDesc->offset, void );
      
      memset( message, 0, VDS_MAX_NAME_LENGTH*4+30 );
      strcpy( message, "Object name: " );
#if VDS_SUPPORT_i18n
      memset( &ps, 0, sizeof(mbstate_t) );
      name = pDesc->originalName;
      lengthName = wcsrtombs( &message[strlen(message)], 
                              &name,
                              VDS_MAX_NAME_LENGTH*4,
                              &ps );
      if ( lengthName == (size_t) -1 ) {
         /* A conversion error */
         strcat( message, " wcsrtombs() conversion error... sorry" );
      }
#else
      strncat( message, pDesc->originalName, pDesc->nameLengthInBytes );
#endif
      vdswEcho( pVerify, message );
      switch( pDesc->apiType ) {
         case VDS_FOLDER:
            valid = vdswVerifyFolder( pVerify,
                                      (vdseFolder *)pObject, 
                                      pContext );
            break;
         case VDS_HASH_MAP:
            valid = vdswVerifyHashMap( pVerify,
                                       (struct vdseHashMap *)pObject, 
                                       pContext );
            break;
         case VDS_QUEUE:
            valid = vdswVerifyQueue( pVerify, (struct vdseQueue *)pObject ); 
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

      if ( valid == VDSW_DELETE_OBJECT && pVerify->doRepair ) {
         switch( pDesc->apiType ) {
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
vdswVerifyFolder( vdswVerifyStruct   * pVerify,
                  struct vdseFolder  * pFolder,
                  vdseSessionContext * pContext )
{
   vdseTxStatus * txFolderStatus;
   int rc;
   bool bTestObject = false;
   
   pVerify->spaces += 2;
   
   /* Is the object lock ? */
   if ( vdscIsItLocked( &pFolder->memObject.lock ) ) {
      vdswEcho( pVerify, "The object is locked - it might be corrupted" );
      if ( pVerify->doRepair ) {
         vdswEcho( pVerify, "Trying to reset the lock..." );
         vdscReleaseProcessLock ( &pFolder->memObject.lock );
      }
      bTestObject = true;
   }

   GET_PTR( txFolderStatus, pFolder->nodeObject.txStatusOffset, vdseTxStatus );

   if ( txFolderStatus->txOffset != NULL_OFFSET ) {
      /*
       * So we have an interrupted transaction. What kind? 
       *   FLAG                      ACTION          
       *   TXS_ADDED                 remove object   
       *   TXS_DESTROYED             reset txStatus  
       *   TXS_DESTROYED_COMMITTED   remove object
       *
       * Action is the equivalent of what a rollback would do.
       */
      if ( txFolderStatus->enumStatus == VDSE_TXS_ADDED ) {
         vdswEcho( pVerify, "Object added but not committed - object removed" );
         return VDSW_DELETE_OBJECT;
      }         
      if ( txFolderStatus->enumStatus == VDSE_TXS_DESTROYED_COMMITTED ) {
         vdswEcho( pVerify, "Object deleted and committed - object removed" );
         return VDSW_DELETE_OBJECT;
      }
      vdswEcho( pVerify, "Object deleted but not committed - object is kept" );
      
      if ( pVerify->doRepair) {
         txFolderStatus->txOffset = NULL_OFFSET;
         txFolderStatus->enumStatus = VDSE_TXS_OK;
      }
   }
   
   if ( txFolderStatus->usageCounter != 0 ) {
      if (pVerify->doRepair) txFolderStatus->usageCounter = 0;
      vdswEcho( pVerify, "Usage counter set to zero" );
   }
   if ( txFolderStatus->parentCounter != 0 ) {
      if (pVerify->doRepair) txFolderStatus->parentCounter = 0;
      vdswEcho( pVerify, "Parent counter set to zero" );
   }
   if ( pFolder->nodeObject.txCounter != 0 ) {
      if (pVerify->doRepair) pFolder->nodeObject.txCounter = 0;
      vdswEcho( pVerify, "Transaction counter set to zero" );
   }

   if ( bTestObject )
      vdswVerifyHash( pVerify, 
                      &pFolder->hashObj, 
                      SET_OFFSET(&pFolder->memObject) );
   
   rc = vdswCheckFolderContent( pVerify, pFolder, pContext );
   pVerify->spaces -= 2;

   return VDSW_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

