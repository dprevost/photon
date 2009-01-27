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
   /* For non-win32 platforms (linux, Unix), the test for a missing
    * directory is done when the directory is open. On Windows, the
    * handle to the OS iterator is returned when retrieving the name
    * of the first file (FindFirst). In other words, the test for
    * Windows is in GetNextNoSuchDir.
    */
#if defined(WIN32)
   return 0;
#else

   bool ok;
   psocDirIterator iterator;
   psocErrorHandler errorHandler;

   psocInitErrorDefs();
   psocInitDir( &iterator );
   psocInitErrorHandler( &errorHandler );
   
   ok = psocOpenDir( &iterator, "abc123", &errorHandler );
   if ( ok ) {
      ERROR_EXIT( expectedToPass, NULL, );
   }
   if ( ! psocAnyErrors( &errorHandler ) ) {
      ERROR_EXIT( expectedToPass, NULL, );
   }
   
   psocCloseDir( &iterator );

   psocFiniDir( &iterator );
   psocFiniErrorHandler( &errorHandler );
   psocFiniErrorDefs();

   return 0;

#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

