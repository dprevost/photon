/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of vdsf (Virtual Data Space Framework).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation and appearing in the file COPYING included in the
 * packaging of this library.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "sessionTest.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   vdseSession * pSession;
   vdseSessionContext context;
   int errcode;
   void * pApiObject = (void *) &errcode; /* dummy pointer */
   ptrdiff_t objOffset;
   vdseObjectContext * pObject;
   bool ok;
   
   pSession = initSessionTest( expectedToPass, &context );

   ok = vdseSessionInit( pSession, pApiObject, &context );
   if ( ! ok ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   objOffset = SET_OFFSET( pSession ); /* Dummy offset */
   
   ok = vdseSessionAddObj( pSession,
                           objOffset, 
                           VDS_FOLDER,
                           pApiObject,
                           &pObject,
                           &context );
   if ( ! ok ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }

   errcode = vdseSessionGetFirst( pSession,
                                  &pObject,
                                  &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
