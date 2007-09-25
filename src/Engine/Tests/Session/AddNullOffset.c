/*
 * Copyright (C) 2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

const bool expectedToPass = false;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   vdseSession * pSession;
   vdseSessionContext context;
   int errcode;
   void * pApiObject = (void *) &errcode; /* dummy pointer */
   ptrdiff_t objOffset;
   vdseObjectContext * pObject;
   
   pSession = initSessionTest( expectedToPass, &context );

   errcode = vdseSessionInit( pSession, pApiObject, &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   
   objOffset = SET_OFFSET( pSession ); /* Dummy offset */
   
   errcode = vdseSessionAddObj( pSession,
                                NULL_OFFSET, 
                                VDS_FOLDER,
                                pApiObject,
                                &pObject,
                                &context );

   ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
