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

#include "queueTest.h"

const bool expectedToPass = false;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
#if defined(USE_DBC)
   psonQueue * pQueue;
   psonSessionContext context;
   bool ok;
   psonTxStatus txStatus;
   psoBasicObjectDef def = { 
      PSO_QUEUE, 
      1, 
      { 0, 0, 0, 0}
   };
   psoFieldDefinition fields[1] =  { 
      { "Field_1", PSO_VAR_STRING, 0, 4, 10, 0, 0 } 
   };
   
   pQueue = initQueueTest( expectedToPass, &context );

   psonTxStatusInit( &txStatus, SET_OFFSET( context.pTransaction ) );
   
   ok = psonQueueInit( pQueue, 
                       0, 1, &txStatus, 4, 
                       "Queue1", SET_OFFSET(pQueue), &def, fields, &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   psonQueueStatus( pQueue, NULL );

   ERROR_EXIT( expectedToPass, NULL, ; );
#else
   return 1;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
