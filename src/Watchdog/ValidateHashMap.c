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
#include "Watchdog/ValidateHashMap.h"
#include "Engine/HashMap.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdswCheckHashMapContent( vdseHashMap * pHashMap, 
                             int           verbose,
                             int           spaces )
{
   enum ListErrors listErr;
   size_t bucket, previousBucket;
   ptrdiff_t offset, previousOffset;
   vdseHashItem * pItem;
//   vdseObjectDescriptor* pDesc = NULL;
//   void * pObject;
//   int pDesc_invalid_api_type = 0;
//   enum vdswValidation valid;
   vdseTxStatus * txItemStatus;
   
//   ptrdiff_t txOffset = SET_OFFSET( pContext->pTransaction );
   
   /* The easy case */
   if ( pHashMap->hashObj.numberOfItems == 0 )
      return 0;
   
   /*
    * We loop on all the hash items until either:
    * - we find one item which is not marked as deleted by the
    *   current transaction (we return false)
    * - or the end (we return true)
    */
   
   listErr = vdseHashGetFirst( &pHashMap->hashObj,
                               &bucket, 
                               &offset );
   while ( listErr == LIST_OK )
   {
      GET_PTR( pItem, offset, vdseHashItem );
      txItemStatus = &pItem->txStatus;


#if 0
      GET_PTR( pDesc, pItem->dataOffset, vdseObjectDescriptor );
      GET_PTR( pObject, pDesc->offset, void );
      
      switch( pDesc->apiType )
      {
         case VDS_FOLDER:
            valid = vdswValidateFolder( (vdseFolder *)pObject, verbose, pContext );
            break;
//         case VDS_HASH_MAP:
//            vdseHashMapStatus( GET_PTR_FAST( pDesc->memOffset, vdseHashMap ),
//                               pStatus );
//            break;
         case VDS_QUEUE:
            valid = vdswValidateQueue( (struct vdseQueue *)pObject, verbose );
            break;
         default:
            VDS_INV_CONDITION( pDesc_invalid_api_type );
      }
#endif
      
      previousBucket = bucket;
      previousOffset = offset;
      listErr = vdseHashGetNext( &pHashMap->hashObj,
                                 previousBucket,
                                 previousOffset,
                                 &bucket, 
                                 &offset );

//      if ( valid == VDSW_DELETE_OBJECT )
//         vdseHashDeleteAt( &pFolder->hashObj,
//                           previousBucket,
//                           pItem,
//                           pContext );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum vdswValidation 
vdswValidateHashMap( vdseHashMap * pHashMap,
                     int           verbose,
                     int           spaces )
{
   vdseTxStatus * txHashMapStatus;
   int rc;
   
   if ( verbose )
   {
      fprintf( stderr, "\n" );
   }
   
   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, vdseTxStatus );

   if ( txHashMapStatus->txOffset != NULL_OFFSET )
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
      if ( txHashMapStatus->statusFlag == 0 ||
         txHashMapStatus->statusFlag == VDSE_REMOVE_IS_COMMITTED )
      {
         fprintf( stderr, "Object is removed\n" );
         return VDSW_DELETE_OBJECT;
      }
      txHashMapStatus->txOffset = NULL_OFFSET;
      txHashMapStatus->statusFlag = 0;
   }
   
   txHashMapStatus->usageCounter = 0;
   txHashMapStatus->parentCounter = 0;
   pHashMap->nodeObject.txCounter = 0;

   rc = vdswCheckHashMapContent( pHashMap, verbose, spaces );

   return VDSW_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

