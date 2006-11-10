/*
 * Copyright (C) 2006 Daniel Prevost <dprevost@users.sourceforge.net>
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

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#include "Common.h"
#include "DirAccess.h"

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int main()
{
   /* pDir () is used in Unix/linux, not on Windows. */
#if defined(WIN32)
   Sleep( 10 );
   return 1;
#else

   int errcode;
   vdscDirIterator iterator;
   const char* str;
   vdscErrorHandler errorHandler;
   
   vdscInitErrorDefs();
   vdscInitDir( &iterator );
   vdscInitErrorHandler( &errorHandler );

   errcode = vdscOpenDir( &iterator, "..", &errorHandler );
   if ( errcode != 0 )
   {
      fprintf( stderr, "Error opening the directory\n" );
      return 0;
   }

   iterator.pDir = NULL;
   str = vdscDirGetNextFileName( &iterator, &errorHandler );
   
   vdscCloseDir( &iterator );

   vdscFiniDir( &iterator );
   vdscFiniErrorHandler( &errorHandler );
   vdscFiniErrorDefs();

   return 0;

#endif
}
