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

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   bool ok = 0;
   pscDirIterator iterator;
   const char* str;
   pscErrorHandler errorHandler;

   pscInitErrorDefs();
   pscInitDir( &iterator );
   pscInitErrorHandler( &errorHandler );
   
   ok = pscOpenDir( &iterator, ".", &errorHandler );
   if ( ! ok ) {
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }
   
   str = pscDirGetNextFileName( &iterator, &errorHandler );
   if ( str == NULL ) {
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }
   
   /* Close and reopen */
   pscCloseDir( &iterator );

   ok = pscOpenDir( &iterator, ".", &errorHandler );
   if ( ! ok ) {
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }
   
   str = pscDirGetNextFileName( &iterator, &errorHandler );
   if ( str == NULL ) {
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }
   
   /* Close twice and reopen - should work */
   pscCloseDir( &iterator );
   pscCloseDir( &iterator );

   ok = pscOpenDir( &iterator, ".", &errorHandler );
   if ( ! ok ) {
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }
   
   str = pscDirGetNextFileName( &iterator, &errorHandler );
   if ( str == NULL ) {
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }
   
   pscCloseDir( &iterator );

   pscFiniDir( &iterator );
   pscFiniErrorHandler( &errorHandler );
   pscFiniErrorDefs();

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

