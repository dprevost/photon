/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef PSOA_LIFO_H
#define PSOA_LIFO_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "API/CommonObject.h"
#include "Nucleus/Queue.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct psoaLifo
{
   psoaCommonObject object;
   
   /**
    * This pointer holds the result of the iteration (the pointer to
    * the current psonQueueItem).
    */
   psonQueueItem * iterator;
   
   /** A pointer to the data definition of the queue. */
   psonFieldDef * pDefinition;
   
   size_t minLength;
   size_t maxLength;
   
} psoaLifo;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Iterates through the queue - no data items are removed from the queue
 * by this function.
 */
PHOTON_API_EXPORT
int psoaLifoFirst( psoaLifo      * pLifo,
                  psoaDataEntry * pEntry );

/** 
 * Iterates through the queue - no data items are removed from the queue
 * by this function.
 */
PHOTON_API_EXPORT
int psoaLifoNext( psoaLifo      * pLifo,
                 psoaDataEntry * pEntry );

/** Remove the first inserted item from a FIFO queue. */
PHOTON_API_EXPORT
int psoaLifoRemove( psoaLifo      * pLifo,
                   psoaDataEntry * pEntry );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSOA_LIFO_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

