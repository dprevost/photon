/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@photonsoftware.org>
 *
 * This file is part of Photon (photonsoftware.org).
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
#include "Quasar/VerifyCommon.h"
#include "Nucleus/Folder.h"
#include "Nucleus/HashMap.h"
#include "Nucleus/Queue.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum psoqRecoverError
psoqCheckFolderContent( psoqVerifyStruct   * pVerify,
                        struct psonFolder  * pFolder, 
                        psonSessionContext * pContext )
{
   ptrdiff_t offset, previousOffset;
   psonHashTxItem * pItem;
   psonObjectDescriptor* pDesc = NULL;
   void * pObject;
   int pDesc_invalid_api_type = 0;
   char message[PSO_MAX_NAME_LENGTH*4 + 30];
   enum psoqRecoverError rc = PSOQ_REC_OK, valid;
   bool found;
   
   /* The easy case */
   if ( pFolder->hashObj.numberOfItems == 0 ) return rc;

   found = psonHashTxGetFirst( &pFolder->hashObj, &offset );
   while ( found ) {
      GET_PTR( pItem, offset, psonHashTxItem );
      GET_PTR( pDesc, pItem->dataOffset, psonObjectDescriptor );
      GET_PTR( pObject, pDesc->offset, void );
      
      memset( message, 0, PSO_MAX_NAME_LENGTH*4+30 );
      strcpy( message, "Object name: " );
      strncat( message, pDesc->originalName, pDesc->nameLengthInBytes );
      psoqEcho( pVerify, message );
      switch( pDesc->apiType ) {
         case PSO_FOLDER:
            valid = psoqVerifyFolder( pVerify,
                                      (psonFolder *)pObject, 
                                      pContext );
            break;
         case PSO_HASH_MAP:
            valid = psoqVerifyHashMap( pVerify,
                                       (struct psonHashMap *)pObject, 
                                       pContext );
            break;
         case PSO_QUEUE:
         case PSO_LIFO:
            valid = psoqVerifyQueue( pVerify, 
                                     (struct psonQueue *)pObject,
                                     pContext ); 
            break;
         case PSO_FAST_MAP:
            valid = psoqVerifyFastMap( pVerify,
                                       (struct psonMap *)pObject, 
                                       pContext );
            break;
         default:
            PSO_INV_CONDITION( pDesc_invalid_api_type );
      }
      
      previousOffset = offset;
      found = psonHashTxGetNext( &pFolder->hashObj,
                               previousOffset,
                               &offset );

      switch ( valid ) {
      case PSOQ_REC_OK:
         pVerify->numObjectsOK++;
         break;
      case PSOQ_REC_CHANGES:
         pVerify->numObjectsRepaired++;
         break;
      case PSOQ_REC_DELETED_OBJECT:
         pVerify->numObjectsDeleted++;
         break;
      default: /* other errors */
         pVerify->numObjectsError++;
         break;
      }

      if ( valid == PSOQ_REC_DELETED_OBJECT && pVerify->doRepair ) {
         rc = PSOQ_REC_CHANGES;
         pVerify->spaces += 2;
         psoqEcho( pVerify, "Removing the object from shared memory" );
         pVerify->spaces -= 2;
         switch( pDesc->apiType ) {
            case PSO_FOLDER:
               psonFolderFini( (psonFolder *)pObject, pContext );
               break;
            case PSO_HASH_MAP:
               psonHashMapFini( (struct psonHashMap *)pObject, pContext );
               break;
            case PSO_QUEUE:
            case PSO_LIFO:
               psonQueueFini( (struct psonQueue *)pObject, pContext );
               break;
            default:
               PSO_INV_CONDITION( pDesc_invalid_api_type );
         }
         psonHashTxDelWithItem( &pFolder->hashObj,
                              pItem,
                              pContext );
      }
   }

   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum psoqRecoverError
psoqVerifyFolder( psoqVerifyStruct   * pVerify,
                  struct psonFolder  * pFolder,
                  psonSessionContext * pContext )
{
   psonTxStatus * txFolderStatus;
   enum psoqRecoverError rc = PSOQ_REC_OK, rc2;
   bool bTestObject = false;
   
   pVerify->spaces += 2;
   
   /* Is the object lock ? */
   if ( psocIsItLocked( &pFolder->memObject.lock ) ) {
      psoqEcho( pVerify, "The object is locked - it might be corrupted" );
      if ( pVerify->doRepair ) {
         psoqEcho( pVerify, "Trying to reset the lock..." );
         psocReleaseProcessLock ( &pFolder->memObject.lock );
      }
      rc = psoqVerifyMemObject( pVerify, &pFolder->memObject, pContext );
      if ( rc > PSOQ_REC_START_ERRORS ) {
         pVerify->spaces -= 2;
         return rc;
      }
      rc = PSOQ_REC_CHANGES;
      bTestObject = true;
   }

   psoqPopulateBitmap( pVerify, &pFolder->memObject, pContext );

   GET_PTR( txFolderStatus, pFolder->nodeObject.txStatusOffset, psonTxStatus );

   if ( txFolderStatus->txOffset != PSON_NULL_OFFSET ) {
      /*
       * So we have an interrupted transaction. What kind? 
       *   FLAG                      ACTION          
       *   TXS_ADDED                 remove object   
       *   TXS_DESTROYED             reset txStatus  
       *   TXS_DESTROYED_COMMITTED   remove object
       *
       * Action is the equivalent of what a rollback would do.
       */
      if ( txFolderStatus->status & PSON_TXS_ADDED ) {
         psoqEcho( pVerify, "Object added but not committed" );
         pVerify->spaces -= 2;
         return PSOQ_REC_DELETED_OBJECT;
      }
      if ( txFolderStatus->status & PSON_TXS_DESTROYED_COMMITTED ) {
         psoqEcho( pVerify, "Object deleted and committed" );
         pVerify->spaces -= 2;
         return PSOQ_REC_DELETED_OBJECT;
      }

      psoqEcho( pVerify, "Object deleted but not committed" );      
      rc = PSOQ_REC_CHANGES;
      if ( pVerify->doRepair) {
         psoqEcho( pVerify, "Object deleted but not committed - resetting the delete flags" );
         txFolderStatus->txOffset = PSON_NULL_OFFSET;
         txFolderStatus->status = PSON_TXS_OK;
      }
   }
   
   if ( txFolderStatus->usageCounter != 0 ) {
      rc = PSOQ_REC_CHANGES;
      psoqEcho( pVerify, "Usage counter is not zero" );
      if (pVerify->doRepair) {
         txFolderStatus->usageCounter = 0;
         psoqEcho( pVerify, "Usage counter set to zero" );
      }
   }
   if ( txFolderStatus->parentCounter != 0 ) {
      rc = PSOQ_REC_CHANGES;
      psoqEcho( pVerify, "Parent counter is not zero" );
      if (pVerify->doRepair) {
         txFolderStatus->parentCounter = 0;
         psoqEcho( pVerify, "Parent counter set to zero" );
      }
   }
   if ( pFolder->nodeObject.txCounter != 0 ) {
      rc = PSOQ_REC_CHANGES;
      psoqEcho( pVerify, "Transaction counter is not zero" );
      if (pVerify->doRepair) {
         pFolder->nodeObject.txCounter = 0;
         psoqEcho( pVerify, "Transaction counter set to zero" );
      }
   }

   if ( bTestObject ) {
      rc2 = psoqVerifyHashTx( pVerify, 
                              &pFolder->hashObj, 
                              SET_OFFSET(&pFolder->memObject) );
      if ( rc2 > PSOQ_REC_START_ERRORS ) {
         pVerify->spaces -= 2;
         return rc2;
      }
      /* At this point rc is either 0 or PSOQ_REC_CHANGES - same for rc2 */
      if ( rc2 > rc ) rc = rc2;
   }
   
   rc2 = psoqCheckFolderContent( pVerify, pFolder, pContext );
   /* At this point rc is either 0 or PSOQ_REC_CHANGES */
   if ( rc2 > rc ) rc = rc2;
   pVerify->spaces -= 2;

   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

