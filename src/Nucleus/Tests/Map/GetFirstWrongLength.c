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
   int errcode;
   psonTxStatus status;
   char * key  = "my key";
   char * data = "my data";
   psonFashMapItem item;
   psoObjectDefinition def = { PSO_FAST_MAP, 1 };
   psoKeyDefinition keyDef = { PSO_KEY_VAR_STRING, 0, 1, 100 };
   const char * fields =  "A dummy definition";
   
   pHashMap = initHashMapTest( expectedToPass, &context );

   psonTxStatusInit( &status, SET_OFFSET( context.pTransaction ) );
   
   ok = psonMapInit( pHashMap, 0, 1, 0, &status, 4, "Map1", 
                     SET_OFFSET(pHashMap), &def, &keyDef, fields, &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }

   ok = psonMapInsert( pHashMap,
                       (const void *) key,
                       6,
                       (const void *) data,
                       7,
                       &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonMapGetFirst( pHashMap,
                         &item,
                         6,
                         6,
                         &context );
   if ( ok == true ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psocGetLastError( &context.errorHandler );
   if ( errcode != PSO_INVALID_LENGTH ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
