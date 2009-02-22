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

#include "hashMapTest.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   psonMap * pHashMap;
   psonSessionContext context;
   bool ok;
   psonTxStatus txStatus;
   char * key1  = "my key1";
   char * key2  = "my key2";
   char * key3  = "my key3";
   char * data  = "my data";
   psoObjStatus status;
   psoObjectDefinition def = { PSO_FAST_MAP, 1 };
   psoKeyDefinition keyDef = { PSO_KEY_VAR_STRING, 0, 1, 100 };
   const char * fields =  "A dummy definition";
   
   pHashMap = initHashMapTest( expectedToPass, &context );

   psonTxStatusInit( &txStatus, SET_OFFSET( context.pTransaction ) );
   
   ok = psonMapInit( pHashMap, 0, 1, 0, &txStatus, 4, "Map1", 
                     SET_OFFSET(pHashMap), &def, &keyDef, fields, &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonMapInsert( pHashMap,
                       (const void *) key1,
                       7,
                       (const void *) data,
                       7,
                       &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonMapInsert( pHashMap,
                       (const void *) key2,
                       7,
                       (const void *) data,
                       7,
                       &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonMapInsert( pHashMap,
                       (const void *) key3,
                       7,
                       (const void *) data,
                       7,
                       &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   psonMapStatus( pHashMap, &status );

   if ( status.numDataItem != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

