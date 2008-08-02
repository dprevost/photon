/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* An object is marked as destroyed as soon as the API call to destroy 
 * it is processed. Once it is marked as destroyed, you can't open it
 * but any sessions using can still continue to use it, without problems.
 * If the call is rollback, the flag VDSE_MARKED_AS_DESTROYED is removed.
 * However, if committed, the flag VDSE_REMOVE_IS_COMMITTED is set and the last 
 * session which access the object (in any form, either a simple close
 * or a rollback or commit ops on the data of the object) will removed it.
 */

#define VDSE_TXS_OK                   0x00
#define VDSE_TXS_DESTROYED            0x01
#define VDSE_TXS_ADDED                0x02
#define VDSE_TXS_EDIT                 0x04
#define VDSE_TXS_REPLACED             0x08
#define VDSE_TXS_DESTROYED_COMMITTED  0x10
#define VDSE_TXS_EDIT_COMMITTED       0x20

#if 0
enum vdseTxStatusEnum
{
   VDSE_TXS_OK = 0,
   VDSE_TXS_DESTROYED,
   VDSE_TXS_ADDED,
   VDSE_TXS_EDIT,
   VDSE_TXS_REPLACED, /* When a data item is replaced */
   VDSE_TXS_DESTROYED_COMMITTED,
   /* When a new version of an object is committed */
   VDSE_TXS_VERSION_REPLACED 
};
typedef enum vdseTxStatusEnum vdseTxStatusEnum;
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct vdseTxStatus
{
   /** The offset of the current transaction */
   ptrdiff_t txOffset;

   /** 
    * An object is marked as destroyed as soon as the API call to destroy 
    * it is processed. Once it is marked as destroyed, you can't open it
    * but any sessions using it can still continue to use it, without problems.
    * If the call is rollback, the flag VDSE_TXS_DESTROYED is removed.
    * However, if committed, the flag VDSE_TXS_DESTROYED_COMMITTED is set and the 
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

typedef struct vdseTxStatus vdseTxStatus;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void vdseTxStatusCommitEdit( vdseTxStatus * pOldStatus, 
                             vdseTxStatus * pNewStatus )
{
   VDS_PRE_CONDITION( pOldStatus != NULL );
   VDS_PRE_CONDITION( pNewStatus != NULL );
   VDS_PRE_CONDITION( pNewStatus->txOffset != VDSE_NULL_OFFSET );
   VDS_PRE_CONDITION( pNewStatus->status & VDSE_TXS_EDIT );

   /* Remove the EDIT bit */
   pNewStatus->status = pOldStatus->status & (uint32_t )(~VDSE_TXS_EDIT);

   pOldStatus->status |= VDSE_TXS_EDIT_COMMITTED;

   pNewStatus->txOffset = VDSE_NULL_OFFSET;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void vdseTxStatusRollbackEdit( vdseTxStatus * pOldStatus )
{
   VDS_PRE_CONDITION( pOldStatus != NULL );

   /* Remove the EDIT bit */
   pOldStatus->status &= (uint32_t )(~VDSE_TXS_EDIT);

   if ( pOldStatus->status == 0 ) {
      pOldStatus->txOffset = VDSE_NULL_OFFSET;
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline 
void vdseTxStatusInit( vdseTxStatus * pStatus, ptrdiff_t txOffset )
{
   VDS_PRE_CONDITION( pStatus != NULL );
   
   pStatus->txOffset = txOffset;
   pStatus->status = VDSE_TXS_OK;
   pStatus->usageCounter = 0;
   pStatus->parentCounter = 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void vdseTxStatusSetTx( vdseTxStatus * pStatus, ptrdiff_t txOffset )
{
   VDS_PRE_CONDITION( pStatus != NULL );

   pStatus->txOffset = txOffset;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline 
void vdseTxStatusFini( vdseTxStatus * pStatus )
{
   VDS_PRE_CONDITION( pStatus != NULL );
   VDS_PRE_CONDITION( pStatus->status == VDSE_TXS_OK );
   VDS_PRE_CONDITION( pStatus->usageCounter == 0 );

   pStatus->txOffset = VDSE_NULL_OFFSET;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
bool vdseTxStatusIsValid( vdseTxStatus * pStatus, ptrdiff_t txOffset )
{
   VDS_PRE_CONDITION( pStatus  != NULL );
   VDS_PRE_CONDITION( txOffset != VDSE_NULL_OFFSET );

   if ( pStatus->txOffset == VDSE_NULL_OFFSET ) return true;
   if ( pStatus->txOffset == txOffset ) return true;

   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void vdseTxStatusClearTx( vdseTxStatus * pStatus )
{
   VDS_PRE_CONDITION( pStatus != NULL );
   VDS_PRE_CONDITION( pStatus->txOffset != VDSE_NULL_OFFSET );

   pStatus->txOffset = VDSE_NULL_OFFSET;
   pStatus->status = VDSE_TXS_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
   
static inline
bool vdseTxStatusIsMarkedAsDestroyed( vdseTxStatus * pStatus )
{
   VDS_PRE_CONDITION( pStatus != NULL );

   return (pStatus->status & VDSE_TXS_DESTROYED);
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
bool vdseTxStatusIsRemoveCommitted( vdseTxStatus * pStatus )
{
   VDS_PRE_CONDITION( pStatus != NULL );

   return (pStatus->status & VDSE_TXS_DESTROYED_COMMITTED);
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void vdseTxStatusMarkAsDestroyed( vdseTxStatus * pStatus )
{
   VDS_PRE_CONDITION( pStatus != NULL );

   pStatus->status |= VDSE_TXS_DESTROYED;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void vdseTxStatusCommitRemove( vdseTxStatus * pStatus )
{
   VDS_PRE_CONDITION( pStatus != NULL );
   VDS_PRE_CONDITION( pStatus->txOffset != VDSE_NULL_OFFSET );
   /* Note - do not add this:
    *    VDS_PRE_CONDITION( pStatus->status & VDSE_TXS_DESTROYED );
    *
    * This call can be reached using either a commit on a "remove" or a
    * rollback on a "add". Maybe I should fix this...
    */

   pStatus->status = VDSE_TXS_DESTROYED_COMMITTED;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void vdseTxStatusUnmarkAsDestroyed( vdseTxStatus * pStatus )
{
   VDS_PRE_CONDITION( pStatus != NULL );
   VDS_PRE_CONDITION( pStatus->txOffset != VDSE_NULL_OFFSET );

   pStatus->status &= (uint32_t )(~VDSE_TXS_DESTROYED);
   /* 
    * This function will be called by a rollback. We clear the transaction
    * itself (if not used obviously). But not the counter (which might be 
    * greater than one if some other session had access to the data before 
    * the data was marked as removed).
    */
   if ( pStatus->status == VDSE_TXS_OK ) pStatus->txOffset = VDSE_NULL_OFFSET;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
bool vdseTxStatusSelfTest( vdseTxStatus * pStatus )
{
   VDS_PRE_CONDITION( pStatus != NULL );

   if ( pStatus->txOffset != VDSE_NULL_OFFSET ) return false;

   return true;
}
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
vdsErrors vdseTxTestObjectStatus( vdseTxStatus * pStatus, ptrdiff_t txOffset )
{
   VDS_PRE_CONDITION( pStatus != NULL );
   VDS_PRE_CONDITION( txOffset != VDSE_NULL_OFFSET );

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
   if ( pStatus->txOffset != VDSE_NULL_OFFSET ) {
      if ( pStatus->status & VDSE_TXS_DESTROYED_COMMITTED ) {
         return VDS_NO_SUCH_OBJECT;
      }
      if ( pStatus->txOffset == txOffset && pStatus->status & VDSE_TXS_DESTROYED ) {
         return VDS_OBJECT_IS_DELETED;
      }
      if ( pStatus->txOffset != txOffset && pStatus->status & VDSE_TXS_ADDED ) {
         return VDS_OBJECT_IS_IN_USE;
      }
   }

   return VDS_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
         
END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSE_TX_STATUS_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

