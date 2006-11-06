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

int main( int argc, char* argv[] )
{
   int errcode = 0, rc = 0;
   vdscDirIterator iterator;
   const char* str;
   vdscErrorHandler errorHandler;

   vdscInitErrorDefs();
   vdscInitDir( &iterator );
   vdscInitErrorHandler( &errorHandler );
   
   errcode = vdscOpenDir( &iterator, ".", &errorHandler );
   if ( errcode != 0 )
   {
      fprintf( stderr, "Error opening the directory\n" );
      return 1;
   }

   str = vdscDirGetNextFileName( &iterator, &errorHandler );
   if ( str == NULL ) 
   {
      vdscCloseDir( &iterator );
      return 1;
   }
   
   /* Close and reopen */
   vdscCloseDir( &iterator );

   errcode = vdscOpenDir( &iterator, ".", &errorHandler );
   if ( errcode != 0 )
   {
      fprintf( stderr, "Error opening the directory\n" );
      return 1;
   }

   str = vdscDirGetNextFileName( &iterator, &errorHandler );
   if ( str == NULL )
   {
      vdscCloseDir( &iterator );
      return 1;
   }
   
   /* Close twice and reopen - should work */
   vdscCloseDir( &iterator );
   vdscCloseDir( &iterator );

   errcode = vdscOpenDir( &iterator, ".", &errorHandler );
   if ( errcode != 0 )
   {
      fprintf( stderr, "Error opening the directory\n" );
      return 1;
   }

   str = vdscDirGetNextFileName( &iterator, &errorHandler );
   if ( str == NULL )
   {
      vdscCloseDir( &iterator );
      return 1;
   }
   vdscCloseDir( &iterator );

   vdscFiniDir( &iterator );
   vdscFiniErrorHandler( &errorHandler );
   vdscFiniErrorDefs();

   return 0;
}
