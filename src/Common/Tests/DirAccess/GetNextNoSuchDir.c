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
    * of the first file (FindFirst).
    */
#if ! defined(WIN32)
   return 0;
#else

   int errcode;
   vdscDirIterator iterator;
   vdscErrorHandler errorHandler;
   const char* str;

   vdscInitErrorDefs();
   vdscInitDir( &iterator );
   vdscInitErrorHandler( &errorHandler );
   
   errcode = vdscOpenDir( &iterator, "abc123", &errorHandler );
   if ( errcode != 0 ) {
      /* OpenDir cannot fail on Win32 but someone might update
       * the code eventually...
       */
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }

   str = vdscDirGetNextFileName( &iterator, &errorHandler );

   if ( str != NULL ) {
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }
   if ( ! vdscAnyErrors( &errorHandler ) ) {
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }
   
   vdscCloseDir( &iterator );

   vdscFiniDir( &iterator );
   vdscFiniErrorHandler( &errorHandler );
   vdscFiniErrorDefs();

   return 0;

#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

