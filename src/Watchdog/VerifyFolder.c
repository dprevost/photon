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

#include "Common/Common.h"
#include "Watchdog/VerifyCommon.h"
#include "Nucleus/Folder.h"
#include "Nucleus/HashMap.h"
#include "Nucleus/Queue.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum vdswRecoverError
vdswCheckFolderContent( vdswVerifyStruct   * pVerify,
                        struct psnFolder  * pFolder, 
                        psnSessionContext * pContext )
{
   ptrdiff_t offset, previousOffset;
   psnHashItem * pItem;
   psnObjectDescriptor* pDesc = NULL;
   void * pObject;
   int pDesc_invalid_api_type = 0;
   char message[PSO_MAX_NAME_LENGTH*4 + 30];
   enum vdswRecoverError rc = VDSWR_OK, valid;
   bool found;
   
   /* The easy case */
   if ( pFolder->hashObj.numberOfItems == 0 ) return rc;

   found = psnHashGetFirst( &pFolder->hashObj, &offset );
   while ( found ) {
      GET_PTR( pItem, offset, psnHashItem );
      GET_PTR( pDesc, pItem->dataOffset, psnObjectDescriptor );
      GET_PTR( pObject, pDesc->offset, void );
      
      memset( message, 0, PSO_MAX_NAME_LENGTH*4+30 );
      strcpy( message, "Object name: " );
      strncat( message, pDesc->originalName, pDesc->nameLengthInBytes );
      vdswEcho( pVerify, message );
      switch( pDesc->apiType ) {
         case PSO_FOLDER:
            valid = vdswVerifyFolder( pVerify,
                                      (psnFolder *)pObject, 
                                      pContext );
            break;
         case PSO_HASH_MAP:
            valid = vdswVerifyHashMap( pVerify,
                                       (struct psnHashMap *)pObject, 
                                       pContext );
            break;
         case PSO_QUEUE:
         case PSO_LIFO:
            valid = vdswVerifyQueue( pVerify, 
                                     (struct psnQueue *)pObject,
                                     pContext ); 
            break;
         case PSO_FAST_MAP:
            valid = vdswVerifyFastMap( pVerify,
                                       (struct psnMap *)pObject, 
                                       pContext );
            break;
         default:
            PSO_INV_CONDITION( pDesc_invalid_api_type );
      }
      
      previousOffset = offset;
      found = psnHashGetNext( &pFolder->hashObj,
                               previousOffset,
                               &offset );

      switch ( valid ) {
      case VDSWR_OK:
         pVerify->numObjectsOK++;
         break;
      case VDSWR_CHANGES:
         pVerify->numObjectsRepaired++;
         break;
      case VDSWR_DELETED_OBJECT:
         pVerify->numObjectsDeleted++;
         break;
      default: /* other errors */
         pVerify->numObjectsError++;
         break;
      }

      if ( valid == VDSWR_DELETED_OBJECT && pVerify->doRepair ) {
         rc = VDSWR_CHANGES;
         pVerify->spaces += 2;
         vdswEcho( pVerify, "Removing the object from the VDS" );
         pVerify->spaces -= 2;
         switch( pDesc->apiType ) {
            case PSO_FOLDER:
               psnFolderFini( (psnFolder *)pObject, pContext );
               break;
            case PSO_HASH_MAP:
               psnHashMapFini( (struct psnHashMap *)pObject, pContext );
               break;
            case PSO_QUEUE:
            case PSO_LIFO:
               psnQueueFini( (struct psnQueue *)pObject, pContext );
               break;
            default:
               PSO_INV_CONDITION( pDesc_invalid_api_type );
         }
         psnHashDelWithItem( &pFolder->hashObj,
                              pItem,
                              pContext );
      }
   }

   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum vdswRecoverError
vdswVerifyFolder( vdswVerifyStruct   * pVerify,
                  struct psnFolder  * pFolder,
                  psnSessionContext * pContext )
{
   psnTxStatus * txFolderStatus;
   enum vdswRecoverError rc = VDSWR_OK, rc2;
   bool bTestObject = false;
   
   pVerify->spaces += 2;
   
   /* Is the object lock ? */
   if ( pscIsItLocked( &pFolder->memObject.lock ) ) {
      vdswEcho( pVerify, "The object is locked - it might be corrupted" );
      if ( pVerify->doRepair ) {
         vdswEcho( pVerify, "Trying to reset the lock..." );
         pscReleaseProcessLock ( &pFolder->memObject.lock );
      }
      rc = vdswVerifyMemObject( pVerify, &pFolder->memObject, pContext );
      if ( rc > VDSWR_START_ERRORS ) {
         pVerify->spaces -= 2;
         return rc;
      }
      rc = VDSWR_CHANGES;
      bTestObject = true;
   }

   vdswPopulateBitmap( pVerify, &pFolder->memObject, pContext );

   GET_PTR( txFolderStatus, pFolder->nodeObject.txStatusOffset, psnTxStatus );

   if ( txFolderStatus->txOffset != PSN_NULL_OFFSET ) {
      /*
       * So we have an interrupted transaction. What kind? 
       *   FLAG                      ACTION          
       *   TXS_ADDED                 remove object   
       *   TXS_DESTROYED             reset txStatus  
       *   TXS_DESTROYED_COMMITTED   remove object
       *
       * Action is the equivalent of what a rollback would do.
       */
      if ( txFolderStatus->status & PSN_TXS_ADDED ) {
         vdswEcho( pVerify, "Object added but not committed" );
         pVerify->spaces -= 2;
         return VDSWR_DELETED_OBJECT;
      }
      if ( txFolderStatus->status & PSN_TXS_DESTROYED_COMMITTED ) {
         vdswEcho( pVerify, "Object deleted and committed" );
         pVerify->spaces -= 2;
         return VDSWR_DELETED_OBJECT;
      }

      vdswEcho( pVerify, "Object deleted but not committed" );      
      rc = VDSWR_CHANGES;
      if ( pVerify->doRepair) {
         vdswEcho( pVerify, "Object deleted but not committed - resetting the delete flags" );
         txFolderStatus->txOffset = PSN_NULL_OFFSET;
         txFolderStatus->status = PSN_TXS_OK;
      }
   }
   
   if ( txFolderStatus->usageCounter != 0 ) {
      rc = VDSWR_CHANGES;
      vdswEcho( pVerify, "Usage counter is not zero" );
      if (pVerify->doRepair) {
         txFolderStatus->usageCounter = 0;
         vdswEcho( pVerify, "Usage counter set to zero" );
      }
   }
   if ( txFolderStatus->parentCounter != 0 ) {
      rc = VDSWR_CHANGES;
      vdswEcho( pVerify, "Parent counter is not zero" );
      if (pVerify->doRepair) {
         txFolderStatus->parentCounter = 0;
         vdswEcho( pVerify, "Parent counter set to zero" );
      }
   }
   if ( pFolder->nodeObject.txCounter != 0 ) {
      rc = VDSWR_CHANGES;
      vdswEcho( pVerify, "Transaction counter is not zero" );
      if (pVerify->doRepair) {
         pFolder->nodeObject.txCounter = 0;
         vdswEcho( pVerify, "Transaction counter set to zero" );
      }
   }

   if ( bTestObject ) {
      rc2 = vdswVerifyHash( pVerify, 
                            &pFolder->hashObj, 
                            SET_OFFSET(&pFolder->memObject) );
      if ( rc2 > VDSWR_START_ERRORS ) {
         pVerify->spaces -= 2;
         return rc2;
      }
      /* At this point rc is either 0 or VDSWR_CHANGES - same for rc2 */
      if ( rc2 > rc ) rc = rc2;
   }
   
   rc2 = vdswCheckFolderContent( pVerify, pFolder, pContext );
   /* At this point rc is either 0 or VDSWR_CHANGES */
   if ( rc2 > rc ) rc = rc2;
   pVerify->spaces -= 2;

   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

