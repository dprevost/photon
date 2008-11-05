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
#include "Nucleus/Map.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum psoqRecoverError
psoqCheckFastMapContent( psoqVerifyStruct   * pVerify,
                         psonMap            * pHashMap, 
                         psonSessionContext * pContext )
{
   enum psoqRecoverError rc = PSOQ_REC_OK;
   
   /* The easy case */
   if ( pHashMap->hashObj.numberOfItems == 0 ) return rc;
   
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum psoqRecoverError 
psoqVerifyFastMap( psoqVerifyStruct   * pVerify,
                   psonMap            * pHashMap,
                   psonSessionContext * pContext )
{
   psonTxStatus * txHashMapStatus;
   enum psoqRecoverError rc = PSOQ_REC_OK, rc2;
   bool bTestObject = false;
      
   pVerify->spaces += 2;

   /* Is the object lock ? */
   if ( psocIsItLocked( &pHashMap->memObject.lock ) ) {
      psoqEcho( pVerify, "The object is locked - it might be corrupted" );
      if ( pVerify->doRepair ) {
         psoqEcho( pVerify, "Trying to reset the lock..." );
         psocReleaseProcessLock ( &pHashMap->memObject.lock );
      }
      rc = psoqVerifyMemObject( pVerify, &pHashMap->memObject, pContext );
      if ( rc > PSOQ_REC_START_ERRORS ) {
         pVerify->spaces -= 2;
         return rc;
      }
      rc = PSOQ_REC_CHANGES;
      bTestObject = true;
   }

   /*
    * Currently, the bitmap is only use if the object was locked. This might
    * change (as it is the case for other types of objects) so populate the 
    * bitmap in all cases to be safe.
    */
   psoqPopulateBitmap( pVerify, &pHashMap->memObject, pContext );

   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, psonTxStatus );

   if ( txHashMapStatus->txOffset != PSON_NULL_OFFSET ) {
      /*
       *   FLAG                      ACTION          
       *   TXS_ADDED                 remove object   
       *   TXS_DESTROYED             reset txStatus  
       *   TXS_DESTROYED_COMMITTED   remove object
       *
       * Action is the equivalent of what a rollback would do.
       */
      if ( txHashMapStatus->status & PSON_TXS_ADDED) {
         psoqEcho( pVerify, "Object added but not committed" );
         pVerify->spaces -= 2;
         return PSOQ_REC_DELETED_OBJECT;
      }
      if ( txHashMapStatus->status & PSON_TXS_DESTROYED_COMMITTED ) {
         psoqEcho( pVerify, "Object deleted and committed" );
         pVerify->spaces -= 2;
         return PSOQ_REC_DELETED_OBJECT;
      }
      
      psoqEcho( pVerify, "Object deleted but not committed" );
      rc = PSOQ_REC_CHANGES;
      if ( pVerify->doRepair) {
         psoqEcho( pVerify, "Object deleted but not committed - resetting the delete flags" );
         txHashMapStatus->txOffset = PSON_NULL_OFFSET;
         txHashMapStatus->status = PSON_TXS_OK;
      }
   }

   if ( txHashMapStatus->usageCounter != 0 ) {
      rc = PSOQ_REC_CHANGES;
      psoqEcho( pVerify, "Usage counter is not zero" );
      if (pVerify->doRepair) {
         txHashMapStatus->usageCounter = 0;
         psoqEcho( pVerify, "Usage counter set to zero" );
      }
   }
   if ( txHashMapStatus->parentCounter != 0 ) {
      rc = PSOQ_REC_CHANGES;
      psoqEcho( pVerify, "Parent counter is not zero" );
      if (pVerify->doRepair) {
         txHashMapStatus->parentCounter = 0;
         psoqEcho( pVerify, "Parent counter set to zero" );
      }
   }
   if ( pHashMap->nodeObject.txCounter != 0 ) {
      rc = PSOQ_REC_CHANGES;
      psoqEcho( pVerify, "Transaction counter is not zero" );
      if (pVerify->doRepair) {
         pHashMap->nodeObject.txCounter = 0;
         psoqEcho( pVerify, "Transaction counter set to zero" );
      }
   }

   if ( bTestObject ) {
      rc2 = psoqVerifyHash( pVerify, 
                           &pHashMap->hashObj, 
                           SET_OFFSET(&pHashMap->memObject) );
      if ( rc2 > PSOQ_REC_START_ERRORS ) {
         pVerify->spaces -= 2;
         return rc2;
      }
      /* At this point rc is either 0 or PSOQ_REC_CHANGES - same for rc2 */
      if ( rc2 > rc ) rc = rc2;
   }

   rc2 = psoqCheckFastMapContent( pVerify, pHashMap, pContext );
   /* At this point rc is either 0 or PSOQ_REC_CHANGES */
   if ( rc2 > rc ) rc = rc2;
   pVerify->spaces -= 2;

   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

