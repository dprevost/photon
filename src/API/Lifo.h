/*
 * Copyright (C) 2008-2009 Daniel Prevost <dprevost@photonsoftware.org>
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
   
   uint32_t minLength;
   uint32_t maxLength;
   
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

