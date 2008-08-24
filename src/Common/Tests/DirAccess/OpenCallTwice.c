/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#include "Common/Common.h"
#include "Common/DirAccess.h"
#include "Tests/PrintError.h"

const bool expectedToPass = false;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
#if defined(USE_DBC)
   /* 
    * This test handles the precondition that pDir must be NULL on 
    * entry to the Open function. This is done so that we don't 
    * loose our "handle" and therefore our access to the data. It
    * would also waste OS resources. 
    *
    * This test is a bit useless on Windows but... it does not hurt!
    */

   bool ok;
   pscDirIterator iterator;
   pscErrorHandler errorHandler;

   pscInitErrorDefs();
   pscInitDir( &iterator );
   pscInitErrorHandler( &errorHandler );
   
   ok = pscOpenDir( &iterator, "..", &errorHandler );

   if ( ! ok ) {
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }
   if ( pscAnyErrors( &errorHandler ) ) {
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }
   
   ok = pscOpenDir( &iterator, "..", &errorHandler );

   ERROR_EXIT( expectedToPass, NULL, ; );
#else
   return 1;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

