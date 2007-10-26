/*
 * Copyright (C) 2007 Daniel Prevost <dprevost@users.sourceforge.net>
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
#include <locale.h>
#include <vdsf/vds.h>
#include "Tests/PrintError.h"
#include "API/Folder.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * This test is very simple - it will create an object with a name chosen
 * in one locale and then try to open the object with a "different" name
 * (the same name but written with a different locale).
 * The chosen object name is "façonné" (built).
 *
 * These 2 locale must be installed on the system for this test to work
 * properly:
 *   - en_US.ISO-8859-1
 *   - en_US.utf8
 * (you can replace these locale but you will also have to change the
 * object names appropriately).
 */
 
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   VDS_HANDLE handle, sessionHandle;
   int errcode;
   vdsaCommonObject object;
   char nameIso[8];
   char nameUtf8[10];
   
   if ( argc > 1 )
      errcode = vdsInit( argv[1], 0, &handle );
   else
      errcode = vdsInit( "10701", 0, &handle );
   if ( errcode != VDS_OK )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdsInitSession( &sessionHandle );
   if ( errcode != VDS_OK )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Set the locale */
   if ( setlocale( LC_ALL, "en_US.ISO-8859-1" ) == NULL )
   {
      fprintf(stderr, "error in setlocale, errno = %d\n", errno );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   strcpy( nameIso, "faconne" );
   nameIso[2] = 0xe7; /* ç */
   nameIso[6] = 0xe9; /* é */
   nameIso[7] = 0;
   
   errcode = vdsCreateObject( sessionHandle,
                              nameIso,
                              strlen(nameIso),
                              VDS_FOLDER );
   if ( errcode != VDS_OK )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Set a different locale */
   if ( setlocale( LC_ALL, "en_US.utf8" ) == NULL )
   {
      fprintf(stderr, "error in setlocale, errno = %d\n", errno );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   nameUtf8[0] = 'f';
   nameUtf8[1] = 'a';
   nameUtf8[2] = 0xc3;
   nameUtf8[3] = 0xa7;
   nameUtf8[4] = 'o';
   nameUtf8[5] = 'n';
   nameUtf8[6] = 'n';
   nameUtf8[7] = 0xc3;
   nameUtf8[8] = 0xa9;
   nameUtf8[9] = 0;

   errcode = vdsFolderOpen( sessionHandle,
                            nameUtf8,
                            9,
                            &handle );
   if ( errcode != VDS_OK )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /*
    * Try it with the iso version of the name. 
    *
    * Note: I'm not testing the exact error code because the error is likely
    * going to change (and may change if you use a different name). The
    * problem is that the code (mbsrtowcs()) expect a unicode string but
    * what we are passing (in this case) is not a valid unicode string.
    * From the output of the debugger, it is clear that mbsrtowcs returned -1 
    * which the engine evaluates as the length of the string (unsigned), a
    * very long string indeed... The engine then sets the error code to 
    * VDS_OBJECT_NAME_TOO_LONG (should be VDS_INVALID_OBJECT_NAME).
    */
   errcode = vdsFolderOpen( sessionHandle,
                            nameIso,
                            7,
                            &handle );
   if ( errcode == VDS_OK )
   {
      fprintf( stderr, "err (should never be zero): %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

