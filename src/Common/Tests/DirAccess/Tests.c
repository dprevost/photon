/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@photonsoftware.org>
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
   psocDirIterator iterator;
   const char* str;
   psocErrorHandler errorHandler;

   psocInitErrorDefs();
   psocInitDir( &iterator );
   psocInitErrorHandler( &errorHandler );
   
   ok = psocOpenDir( &iterator, ".", &errorHandler );
   if ( ! ok ) {
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }
   
   str = psocDirGetNextFileName( &iterator, &errorHandler );
   if ( str == NULL ) {
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }
   
   /* Close and reopen */
   psocCloseDir( &iterator );

   ok = psocOpenDir( &iterator, ".", &errorHandler );
   if ( ! ok ) {
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }
   
   str = psocDirGetNextFileName( &iterator, &errorHandler );
   if ( str == NULL ) {
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }
   
   /* Close twice and reopen - should work */
   psocCloseDir( &iterator );
   psocCloseDir( &iterator );

   ok = psocOpenDir( &iterator, ".", &errorHandler );
   if ( ! ok ) {
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }
   
   str = psocDirGetNextFileName( &iterator, &errorHandler );
   if ( str == NULL ) {
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }
   
   psocCloseDir( &iterator );

   psocFiniDir( &iterator );
   psocFiniErrorHandler( &errorHandler );
   psocFiniErrorDefs();

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

