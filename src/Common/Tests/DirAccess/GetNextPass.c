/*
 * Copyright (C) 2006-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#include "Common/Common.h"
#include "Common/DirAccess.h"
#include "Tests/PrintError.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   bool ok;
   psocDirIterator iterator;
   const char* str;
   psocErrorHandler errorHandler;
   
   psocInitErrorDefs();
   psocInitDir( &iterator );
   psocInitErrorHandler( &errorHandler );

   ok = psocOpenDir( &iterator, "..", &errorHandler );
   if ( ! ok ) {
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }
   
   str = psocDirGetNextFileName( &iterator, &errorHandler );

   if ( str == NULL ) {
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }
   if ( psocAnyErrors( &errorHandler ) ) {
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }
   
   psocCloseDir( &iterator );

   psocFiniDir( &iterator );
   psocFiniErrorHandler( &errorHandler );
   psocFiniErrorDefs();

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

