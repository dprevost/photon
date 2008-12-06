/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@photonsoftware.org>
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
   /* For non-win32 platforms (linux, Unix), the test for a missing
    * directory is done when the directory is open. On Windows, the
    * handle to the OS iterator is returned when retrieving the name
    * of the first file (FindFirst).
    */
#if ! defined(WIN32)
   return 0;
#else

   bool ok;
   psocDirIterator iterator;
   psocErrorHandler errorHandler;
   const char* str;

   psocInitErrorDefs();
   psocInitDir( &iterator );
   psocInitErrorHandler( &errorHandler );
   
   ok = psocOpenDir( &iterator, "abc123", &errorHandler );
   if ( ! ok ) {
      /* OpenDir cannot fail on Win32 but someone might update
       * the code eventually...
       */
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }

   str = psocDirGetNextFileName( &iterator, &errorHandler );

   if ( str != NULL ) {
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }
   if ( ! psocAnyErrors( &errorHandler ) ) {
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }
   
   psocCloseDir( &iterator );

   psocFiniDir( &iterator );
   psocFiniErrorHandler( &errorHandler );
   psocFiniErrorDefs();

   return 0;

#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

