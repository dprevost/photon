/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef VDSA_LIFO_H
#define VDSA_LIFO_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "API/CommonObject.h"
#include "Engine/Queue.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct vdsaLifo
{
   vdsaCommonObject object;
   
   /**
    * This pointer holds the result of the iteration (the pointer to
    * the current vdseQueueItem).
    */
   vdseQueueItem * iterator;
   
   /** A pointer to the data definition of the queue. */
   vdseFieldDef * pDefinition;
   
   size_t minLength;
   size_t maxLength;
   
} vdsaLifo;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Iterates through the queue - no data items are removed from the queue
 * by this function.
 */
VDSF_API_EXPORT
int vdsaLifoFirst( vdsaLifo      * pLifo,
                   vdsaDataEntry * pEntry );

/** 
 * Iterates through the queue - no data items are removed from the queue
 * by this function.
 */
VDSF_API_EXPORT
int vdsaLifoNext( vdsaLifo      * pLifo,
                  vdsaDataEntry * pEntry );

/** Remove the first inserted item from a FIFO queue. */
VDSF_API_EXPORT
int vdsaLifoRemove( vdsaLifo      * pLifo,
                    vdsaDataEntry * pEntry );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSA_LIFO_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

