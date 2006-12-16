/* -*- c++ -*- */
/*
 * Copyright (C) 2006 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef VDSE_TRANSACTION_ITEM_H
#define VDSE_TRANSACTION_ITEM_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Engine.h"       

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */


/* An object is marked as destroyed as soon as the API call to destroy 
 * it is processed. Once it is marked as destroyed, you can't open it
 * but any sessions using can still continue to use it, without problems.
 * If the call is rollback, the flag MARKED_AS_DESTROYED is removed.
 * However, if committed, the flag REMOVE_IS_COMMITTED is set and the last 
 * session which access the object (in any form, either a simple close
 * or a rollback or commit ops on the data of the object) will removed it.
 */
#define MARKED_AS_DESTROYED  1
#define REMOVE_IS_COMMITTED  2

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct vdseTxItem
{
   /* The identifier for the current transaction */
   uint32_t transactionId;


   /** An object is marked as destroyed as soon as the API call to destroy 
    * it is processed. Once it is marked as destroyed, you can't open it
    * but any sessions using can still continue to use it, without problems.
    * If the call is rollback, the flag MARKED_AS_DESTROYED is removed.
    * However, if committed, the flag REMOVE_IS_COMMITTED is set and the last 
    * session which access the object (in any form, either a simple close
    * or a rollback or commit ops on the data of the object) will removed it.
    */
   uint32_t statusFlag;
   
   uint32_t usageCounter;

   uint32_t signature;
   
} vdseTxItem;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline 
void vdseTxItemInit( vdseTxItem* pItem, transaction_T txId )
{
   pItem->transactionId = txId;
   pItem->statusFlag = 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void vdseTxItemSetId( vdseTxItem* pItem, transaction_T txId )
{
   pItem->transactionId = txId;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline 
void vdseTxItemFini( vdseTxItem* pItem )
{
   pItem->transactionId = 0;
   pItem->statusFlag = 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
bool vdseTxItemIsValid( vdseTxItem* pItem, transaction_T transactionId )
{
   if ( pItem->transactionId == 0 )
      return true;
   if ( pItem->transactionId == transactionId )
      return true;
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void vdseTxItemCommit( vdseTxItem* pItem )
{
   pItem->transactionId = 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
   
static inline
bool vdseTxItemIsMarkedAsDestroyed( vdseTxItem* pItem )
{
   return (pItem->statusFlag & MARKED_AS_DESTROYED);
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
bool vdseTxItemIsRemoveCommitted( vdseTxItem* pItem )
{
   return (pItem->statusFlag & REMOVE_IS_COMMITTED );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void vdseTxItemMarkAsDestroyed( vdseTxItem* pItem )
{
   pItem->statusFlag |= MARKED_AS_DESTROYED;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void vdseTxItemCommitRemove( vdseTxItem* pItem )
{
   pItem->statusFlag |= REMOVE_IS_COMMITTED;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void vdseTxItemUnmarkAsDestroyed( vdseTxItem* pItem )
{
   pItem->statusFlag = ~(~pItem->statusFlag | MARKED_AS_DESTROYED);
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
bool vdseTxItemSelfTest( vdseTxItem* pItem )
{
   if ( pItem->transactionId != 0 )
   {
#if defined (VDS_DEBUG)
      fprintf( stderr, "TransactionBase::pItem->transactionId = %d \n", 
               pItem->transactionId );
      VDS_ASSERT( 0 );
#endif
      return false;
   }
   return true;
}
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSE_TRANSACTION_BASE_H */

