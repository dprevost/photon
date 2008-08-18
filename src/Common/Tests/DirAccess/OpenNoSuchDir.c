/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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
   pscDirIterator iterator;
   pscErrorHandler errorHandler;

   pscInitErrorDefs();
   pscInitDir( &iterator );
   pscInitErrorHandler( &errorHandler );
   
   ok = pscOpenDir( &iterator, "abc123", &errorHandler );
   if ( ok ) {
      ERROR_EXIT( expectedToPass, NULL, );
   }
   if ( ! pscAnyErrors( &errorHandler ) ) {
      ERROR_EXIT( expectedToPass, NULL, );
   }
   
   pscCloseDir( &iterator );

   pscFiniDir( &iterator );
   pscFiniErrorHandler( &errorHandler );
   pscFiniErrorDefs();

   return 0;

#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

