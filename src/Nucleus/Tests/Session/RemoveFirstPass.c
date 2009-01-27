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

#include "sessionTest.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   psonSession * pSession;
   psonSessionContext context;
   int errcode;
   bool ok;
   void * pApiObject = (void *) &errcode; /* dummy pointer */
   ptrdiff_t objOffset;
   psonObjectContext * pObject;
   
   pSession = initSessionTest( expectedToPass, &context );

   ok = psonSessionInit( pSession, pApiObject, &context );
   if ( ! ok ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   objOffset = SET_OFFSET( pSession ); /* Dummy offset */
   
   ok = psonSessionAddObj( pSession,
                           objOffset, 
                           PSO_FOLDER,
                           pApiObject,
                           &pObject,
                           &context );
   if ( ! ok ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonSessionRemoveFirst( pSession, &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
