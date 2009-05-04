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

#ifndef PSOA_QUEUE_H
#define PSOA_QUEUE_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "API/CommonObject.h"
#include "Nucleus/Queue.h"
#include "API/DataDefinition.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct psoaQueue
{
   psoaCommonObject object;
   
   /*
    * This pointer holds the result of the iteration (the pointer to
    * the current psonQueueItem).
    */
   psonQueueItem * iterator;
   
   /*
    * Pointer to the data defintion struct used to hold the data
    * definition for each retrieved record
    */
   psoaDataDefinition * pRecordDefinition;
   
} psoaQueue;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Iterates through the queue - no data items are removed from the queue
 * by this function.
 */
PHOTON_API_EXPORT
int psoaQueueFirst( psoaQueue      * pQueue,
                    unsigned char ** data,
                    uint32_t       * length );

/** 
 * Iterates through the queue - no data items are removed from the queue
 * by this function.
 */
PHOTON_API_EXPORT
int psoaQueueNext( psoaQueue      * pQueue,
                   unsigned char ** data,
                   uint32_t       * length );

/** Remove the first inserted item from a FIFO queue. */
PHOTON_API_EXPORT
int psoaQueueRemove( psoaQueue      * pQueue,
                     unsigned char ** data,
                     uint32_t       * length );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSOA_QUEUE_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

