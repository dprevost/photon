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

const bool expectedToPass = false;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
#if defined(USE_DBC)
   /* pDir () is used in Unix/linux, not on Windows. */
#  if defined(WIN32)
   Sleep( 10 );
   return 1;
#  else

   bool ok;
   psocDirIterator iterator;
   const char* str;
   psocErrorHandler errorHandler;
   
   psocInitErrorDefs();
   psocInitDir( &iterator );
   psocInitErrorHandler( &errorHandler );

   ok = psocOpenDir( &iterator, "..", &errorHandler );
   if ( ! ok ) {
      ERROR_EXIT( expectedToPass, &errorHandler, );
   }
   
   iterator.pDir = NULL;
   str = psocDirGetNextFileName( &iterator, &errorHandler );
   
   ERROR_EXIT( expectedToPass, NULL, );
   
#  endif
#else
   return 1;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

