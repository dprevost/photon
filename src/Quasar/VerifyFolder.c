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
#include "Quasar/VerifyCommon.h"
#include "Nucleus/Folder.h"
#include "Nucleus/HashMap.h"
#include "Nucleus/Queue.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum qsrRecoverError
qsrCheckFolderContent( qsrVerifyStruct   * pVerify,
                        struct psonFolder  * pFolder, 
                        psonSessionContext * pContext )
{
   ptrdiff_t offset, previousOffset;
   psonHashTxItem * pItem;
   psonObjectDescriptor* pDesc = NULL;
   void * pObject;
   int pDesc_invalid_api_type = 0;
   char message[PSO_MAX_NAME_LENGTH*4 + 30];
   enum qsrRecoverError rc = QSR_REC_OK, valid;
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
      qsrEcho( pVerify, message );
      switch( pDesc->apiType ) {
         case PSO_FOLDER:
            valid = qsrVerifyFolder( pVerify,
                                      (psonFolder *)pObject, 
                                      pContext );
            break;
         case PSO_HASH_MAP:
            valid = qsrVerifyHashMap( pVerify,
                                       (struct psonHashMap *)pObject, 
                                       pContext );
            break;
         case PSO_QUEUE:
         case PSO_LIFO:
            valid = qsrVerifyQueue( pVerify, 
                                     (struct psonQueue *)pObject,
                                     pContext ); 
            break;
         case PSO_FAST_MAP:
            valid = qsrVerifyFastMap( pVerify,
                                       (struct psonFastMap *)pObject, 
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
      case QSR_REC_OK:
         pVerify->numObjectsOK++;
         break;
      case QSR_REC_CHANGES:
         pVerify->numObjectsRepaired++;
         break;
      case QSR_REC_DELETED_OBJECT:
         pVerify->numObjectsDeleted++;
         break;
      default: /* other errors */
         pVerify->numObjectsError++;
         break;
      }

      if ( valid == QSR_REC_DELETED_OBJECT && pVerify->doRepair ) {
         rc = QSR_REC_CHANGES;
         pVerify->spaces += 2;
         qsrEcho( pVerify, "Removing the object from shared memory" );
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
         psonHashTxDelete( &pFolder->hashObj,
                           pItem,
                           pContext );
      }
   }

   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum qsrRecoverError
qsrVerifyFolder( qsrVerifyStruct   * pVerify,
                  struct psonFolder  * pFolder,
                  psonSessionContext * pContext )
{
   psonTxStatus * txFolderStatus;
   enum qsrRecoverError rc = QSR_REC_OK, rc2;
   bool bTestObject = false;
   
   pVerify->spaces += 2;
   
   /* Is the object lock ? */
   if ( psocIsItLocked( &pFolder->memObject.lock ) ) {
      qsrEcho( pVerify, "The object is locked - it might be corrupted" );
      if ( pVerify->doRepair ) {
         qsrEcho( pVerify, "Trying to reset the lock..." );
         psocReleaseProcessLock ( &pFolder->memObject.lock );
      }
      rc = qsrVerifyMemObject( pVerify, &pFolder->memObject, pContext );
      if ( rc > QSR_REC_START_ERRORS ) {
         pVerify->spaces -= 2;
         return rc;
      }
      rc = QSR_REC_CHANGES;
      bTestObject = true;
   }

   qsrPopulateBitmap( pVerify, &pFolder->memObject, pContext );

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
         qsrEcho( pVerify, "Object added but not committed" );
         pVerify->spaces -= 2;
         return QSR_REC_DELETED_OBJECT;
      }
      if ( txFolderStatus->status & PSON_TXS_DESTROYED_COMMITTED ) {
         qsrEcho( pVerify, "Object deleted and committed" );
         pVerify->spaces -= 2;
         return QSR_REC_DELETED_OBJECT;
      }

      qsrEcho( pVerify, "Object deleted but not committed" );      
      rc = QSR_REC_CHANGES;
      if ( pVerify->doRepair) {
         qsrEcho( pVerify, "Object deleted but not committed - resetting the delete flags" );
         txFolderStatus->txOffset = PSON_NULL_OFFSET;
         txFolderStatus->status = PSON_TXS_OK;
      }
   }
   
   if ( txFolderStatus->usageCounter != 0 ) {
      rc = QSR_REC_CHANGES;
      qsrEcho( pVerify, "Usage counter is not zero" );
      if (pVerify->doRepair) {
         txFolderStatus->usageCounter = 0;
         qsrEcho( pVerify, "Usage counter set to zero" );
      }
   }
   if ( txFolderStatus->parentCounter != 0 ) {
      rc = QSR_REC_CHANGES;
      qsrEcho( pVerify, "Parent counter is not zero" );
      if (pVerify->doRepair) {
         txFolderStatus->parentCounter = 0;
         qsrEcho( pVerify, "Parent counter set to zero" );
      }
   }
   if ( pFolder->nodeObject.txCounter != 0 ) {
      rc = QSR_REC_CHANGES;
      qsrEcho( pVerify, "Transaction counter is not zero" );
      if (pVerify->doRepair) {
         pFolder->nodeObject.txCounter = 0;
         qsrEcho( pVerify, "Transaction counter set to zero" );
      }
   }

   if ( bTestObject ) {
      rc2 = qsrVerifyHashTx( pVerify, 
                              &pFolder->hashObj, 
                              SET_OFFSET(&pFolder->memObject) );
      if ( rc2 > QSR_REC_START_ERRORS ) {
         pVerify->spaces -= 2;
         return rc2;
      }
      /* At this point rc is either 0 or QSR_REC_CHANGES - same for rc2 */
      if ( rc2 > rc ) rc = rc2;
   }
   
   rc2 = qsrCheckFolderContent( pVerify, pFolder, pContext );
   /* At this point rc is either 0 or QSR_REC_CHANGES */
   if ( rc2 > rc ) rc = rc2;
   pVerify->spaces -= 2;

   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

