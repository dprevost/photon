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

#ifndef VDSE_TX_STATUS_H
#define VDSE_TX_STATUS_H

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

typedef struct vdseTxStatus
{
   /* The offset of the current transaction */
   ptrdiff_t txOffset;

   /** An object is marked as destroyed as soon as the API call to destroy 
    * it is processed. Once it is marked as destroyed, you can't open it
    * but any sessions using can still continue to use it, without problems.
    * If the call is rollback, the flag MARKED_AS_DESTROYED is removed.
    * However, if committed, the flag REMOVE_IS_COMMITTED is set and the last 
    * session which access the object (in any form, either a simple close
    * or a rollback or commit ops on the data of the object) will removed it.
    */
   uint32_t statusFlag;
   
   /** 
    * Counts the number of clients who are accessing either the current
    * object and or the current data item (this is to prevent the removal
    * of data/objects while they are used).
    */
   uint32_t usageCounter;

} vdseTxStatus;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline 
void vdseTxStatusInit( vdseTxStatus* pStatus, ptrdiff_t txOffset )
{
   VDS_PRE_CONDITION( pStatus != NULL );
   
   pStatus->txOffset = txOffset;
   pStatus->statusFlag = 0;
   pStatus->usageCounter = 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void vdseTxStatusSetTx( vdseTxStatus* pStatus, ptrdiff_t txOffset )
{
   pStatus->txOffset = txOffset;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline 
void vdseTxStatusFini( vdseTxStatus* pStatus )
{
   VDS_PRE_CONDITION( pStatus != NULL );
   VDS_PRE_CONDITION( pStatus->statusFlag   == 0 );
   VDS_PRE_CONDITION( pStatus->usageCounter == 0 );

   pStatus->txOffset = NULL_OFFSET;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
bool vdseTxStatusIsValid( vdseTxStatus* pStatus, ptrdiff_t txOffset )
{
   VDS_PRE_CONDITION( pStatus  != NULL );
   VDS_PRE_CONDITION( txOffset != NULL_OFFSET );

   if ( pStatus->txOffset == NULL_OFFSET )
      return true;
   if ( pStatus->txOffset == txOffset )
      return true;
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void vdseTxStatusCommit( vdseTxStatus* pStatus )
{
   VDS_PRE_CONDITION( pStatus != NULL );
   VDS_PRE_CONDITION( pStatus->txOffset != NULL_OFFSET );

   pStatus->txOffset = NULL_OFFSET;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
   
static inline
bool vdseTxStatusIsMarkedAsDestroyed( vdseTxStatus* pStatus )
{
   VDS_PRE_CONDITION( pStatus != NULL );

   return (pStatus->statusFlag & MARKED_AS_DESTROYED);
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
bool vdseTxStatusIsRemoveCommitted( vdseTxStatus* pStatus )
{
   VDS_PRE_CONDITION( pStatus != NULL );

   return (pStatus->statusFlag & REMOVE_IS_COMMITTED );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void vdseTxStatusMarkAsDestroyed( vdseTxStatus* pStatus )
{
   VDS_PRE_CONDITION( pStatus != NULL );

   pStatus->statusFlag |= MARKED_AS_DESTROYED;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void vdseTxStatusCommitRemove( vdseTxStatus* pStatus )
{
   VDS_PRE_CONDITION( pStatus != NULL );

   pStatus->statusFlag |= REMOVE_IS_COMMITTED;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void vdseTxStatusUnmarkAsDestroyed( vdseTxStatus* pStatus )
{
   VDS_PRE_CONDITION( pStatus != NULL );

   pStatus->statusFlag = ~(~pStatus->statusFlag | MARKED_AS_DESTROYED);
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
bool vdseTxStatusSelfTest( vdseTxStatus* pStatus )
{
   VDS_PRE_CONDITION( pStatus != NULL );

   if ( pStatus->txOffset != NULL_OFFSET )
   {
#if defined (VDS_DEBUG)
      fprintf( stderr, "TransactionBase::pStatus->transactionId = %d \n", 
               pStatus->transactionId );
      VDS_ASSERT( 0 );
#endif
      return false;
   }
   return true;
}
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSE_TX_STATUS_H */

