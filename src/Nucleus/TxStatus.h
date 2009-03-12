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

#ifndef PSON_TX_STATUS_H
#define PSON_TX_STATUS_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Engine.h"       

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* An object is marked as destroyed as soon as the API call to destroy 
 * it is processed. Once it is marked as destroyed, you can't open it
 * but any sessions using can still continue to use it, without problems.
 * If the call is rollback, the flag PSON_MARKED_AS_DESTROYED is removed.
 * However, if committed, the flag PSON_REMOVE_IS_COMMITTED is set and the last 
 * session which access the object (in any form, either a simple close
 * or a rollback or commit ops on the data of the object) will removed it.
 */

#define PSON_TXS_OK                   0x00
#define PSON_TXS_DESTROYED            0x01
#define PSON_TXS_ADDED                0x02
#define PSON_TXS_EDIT                 0x04
#define PSON_TXS_REPLACED             0x08
#define PSON_TXS_DESTROYED_COMMITTED  0x10
#define PSON_TXS_EDIT_COMMITTED       0x20

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psonTxStatus
{
   /** The offset of the current transaction */
   ptrdiff_t txOffset;

   /** 
    * An object is marked as destroyed as soon as the API call to destroy 
    * it is processed. Once it is marked as destroyed, you can't open it
    * but any sessions using it can still continue to use it, without problems.
    * If the call is rollback, the flag PSON_TXS_DESTROYED is removed.
    * However, if committed, the flag PSON_TXS_DESTROYED_COMMITTED is set and the 
    * last session which access the object (in any form, either a simple close
    * or a rollback or commit ops on the data of the object) will removed it.
    */
   uint32_t status;
   
   /** 
    * Counts the number of clients who are accessing either the current
    * object (or one of its data item) and/or the current data item (this 
    * is to prevent the removal of data/objects while they are used).
    */
   uint32_t usageCounter;

   /** 
    * Counts access done by the parent folder or through the parent folder. 
    * We use a different counter for efficiency reasons - if we already have
    * a lock on the parent folder (and no locks on the current object) we
    * increment/decrement the parentCounter. This counter is use when an
    * object is open/close or when iterating through all objects in the parent
    * folder.
    *
    * To repeat, this counter should only be used when the parent folder is
    * locked.
    */
   uint32_t parentCounter;

};

typedef struct psonTxStatus psonTxStatus;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void psonTxStatusCommitEdit( psonTxStatus * pOldStatus, 
                             psonTxStatus * pNewStatus )
{
   PSO_PRE_CONDITION( pOldStatus != NULL );
   PSO_PRE_CONDITION( pNewStatus != NULL );
   PSO_PRE_CONDITION( pNewStatus->txOffset != PSON_NULL_OFFSET );
   PSO_PRE_CONDITION( pNewStatus->status & PSON_TXS_EDIT );

   /* Remove the EDIT bit */
   pNewStatus->status = pOldStatus->status & (uint32_t )(~PSON_TXS_EDIT);

   pOldStatus->status |= PSON_TXS_EDIT_COMMITTED;

   pNewStatus->txOffset = PSON_NULL_OFFSET;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void psonTxStatusRollbackEdit( psonTxStatus * pOldStatus )
{
   PSO_PRE_CONDITION( pOldStatus != NULL );

   /* Remove the EDIT bit */
   pOldStatus->status &= (uint32_t )(~PSON_TXS_EDIT);

   if ( pOldStatus->status == 0 ) {
      pOldStatus->txOffset = PSON_NULL_OFFSET;
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline 
void psonTxStatusInit( psonTxStatus * pStatus, ptrdiff_t txOffset )
{
   PSO_PRE_CONDITION( pStatus != NULL );
   
   pStatus->txOffset = txOffset;
   pStatus->status = PSON_TXS_OK;
   pStatus->usageCounter = 0;
   pStatus->parentCounter = 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void psonTxStatusSetTx( psonTxStatus * pStatus, ptrdiff_t txOffset )
{
   PSO_PRE_CONDITION( pStatus != NULL );

   pStatus->txOffset = txOffset;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline 
void psonTxStatusFini( psonTxStatus * pStatus )
{
   PSO_PRE_CONDITION( pStatus != NULL );
   PSO_PRE_CONDITION( pStatus->status == PSON_TXS_OK );
   PSO_PRE_CONDITION( pStatus->usageCounter == 0 );

   pStatus->txOffset = PSON_NULL_OFFSET;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
bool psonTxStatusIsValid( psonTxStatus * pStatus, ptrdiff_t txOffset )
{
   PSO_PRE_CONDITION( pStatus  != NULL );
   PSO_PRE_CONDITION( txOffset != PSON_NULL_OFFSET );

   if ( pStatus->txOffset == PSON_NULL_OFFSET ) return true;
   if ( pStatus->txOffset == txOffset ) return true;

   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void psonTxStatusClearTx( psonTxStatus * pStatus )
{
   PSO_PRE_CONDITION( pStatus != NULL );
   PSO_PRE_CONDITION( pStatus->txOffset != PSON_NULL_OFFSET );

   pStatus->txOffset = PSON_NULL_OFFSET;
   pStatus->status = PSON_TXS_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
   
static inline
bool psonTxStatusIsMarkedAsDestroyed( psonTxStatus * pStatus )
{
   PSO_PRE_CONDITION( pStatus != NULL );

   return (pStatus->status & PSON_TXS_DESTROYED);
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
bool psonTxStatusIsRemoveCommitted( psonTxStatus * pStatus )
{
   PSO_PRE_CONDITION( pStatus != NULL );

   if (pStatus->status & PSON_TXS_DESTROYED_COMMITTED) return true;
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void psonTxStatusMarkAsDestroyed( psonTxStatus * pStatus )
{
   PSO_PRE_CONDITION( pStatus != NULL );

   pStatus->status |= PSON_TXS_DESTROYED;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void psonTxStatusCommitRemove( psonTxStatus * pStatus )
{
   PSO_PRE_CONDITION( pStatus != NULL );
   PSO_PRE_CONDITION( pStatus->txOffset != PSON_NULL_OFFSET );
   /* Note - do not add this:
    *    PSO_PRE_CONDITION( pStatus->status & PSON_TXS_DESTROYED );
    *
    * This call can be reached using either a commit on a "remove" or a
    * rollback on a "add". Maybe I should fix this...
    */

   pStatus->status = PSON_TXS_DESTROYED_COMMITTED;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void psonTxStatusUnmarkAsDestroyed( psonTxStatus * pStatus )
{
   PSO_PRE_CONDITION( pStatus != NULL );
   PSO_PRE_CONDITION( pStatus->txOffset != PSON_NULL_OFFSET );

   pStatus->status &= (uint32_t )(~PSON_TXS_DESTROYED);
   /* 
    * This function will be called by a rollback. We clear the transaction
    * itself (if not used obviously). But not the counter (which might be 
    * greater than one if some other session had access to the data before 
    * the data was marked as removed).
    */
   if ( pStatus->status == PSON_TXS_OK ) pStatus->txOffset = PSON_NULL_OFFSET;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
bool psonTxStatusSelfTest( psonTxStatus * pStatus )
{
   PSO_PRE_CONDITION( pStatus != NULL );

   if ( pStatus->txOffset != PSON_NULL_OFFSET ) return false;

   return true;
}
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
psoErrors psonTxTestObjectStatus( psonTxStatus * pStatus, ptrdiff_t txOffset )
{
   PSO_PRE_CONDITION( pStatus != NULL );
   PSO_PRE_CONDITION( txOffset != PSON_NULL_OFFSET );

   /* 
    * If the transaction id of the object is equal to the 
    * current transaction id AND the object is marked as deleted... error.
    *
    * If the transaction id of the object is NOT equal to the 
    * current transaction id AND the object is added... error.
    *
    * If the object is flagged as deleted and committed, it does not exists
    * from the API point of view.
    */
   if ( pStatus->txOffset != PSON_NULL_OFFSET ) {
      if ( pStatus->status & PSON_TXS_DESTROYED_COMMITTED ) {
         return PSO_NO_SUCH_OBJECT;
      }
      if ( pStatus->txOffset == txOffset && pStatus->status & PSON_TXS_DESTROYED ) {
         return PSO_OBJECT_IS_DELETED;
      }
      if ( pStatus->txOffset != txOffset && pStatus->status & PSON_TXS_ADDED ) {
         return PSO_OBJECT_IS_IN_USE;
      }
   }

   return PSO_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
         
END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSON_TX_STATUS_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

