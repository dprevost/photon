/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of Photon (photonsoftware.org).
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

#include "processTest.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   psonProcess * process;
   psonSessionContext context;
   int errcode;
   bool ok;
   psonSession * pSession1, *pSession2;
   void * pApiSession = (void *) &errcode; /* A dummy pointer */
   
   process = initProcessTest( expectedToPass, &context );

   ok = psonProcessInit( process, 12345, &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonProcessAddSession( process,
                               pApiSession,
                               &pSession1,
                               &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonProcessGetFirstSession( process,
                                    &pSession1,
                                    &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   /* Only one session - this should fail */
   ok = psonProcessGetNextSession( process,
                                   pSession1,
                                   &pSession2,
                                   &context );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }

   ok = psonProcessAddSession( process,
                               pApiSession,
                               &pSession1,
                               &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonProcessGetFirstSession( process,
                                    &pSession1,
                                    &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonProcessGetNextSession( process,
                                   pSession1,
                                   &pSession2,
                                   &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pSession1 == pSession2 ) return -1;
   
   psonProcessFini( process, &context );
                                 
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
