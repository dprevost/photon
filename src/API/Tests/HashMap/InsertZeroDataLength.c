/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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
#include <vdsf/vds.h>
#include "Tests/PrintError.h"
#include "API/HashMap.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   VDS_HANDLE objHandle, sessionHandle;
   int errcode;
   const char * key  = "My Key";
   const char * data = "My Data";

   if ( argc > 1 )
      errcode = vdsInit( argv[1], 0 );
   else
      errcode = vdsInit( "10701", 0 );
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

   errcode = vdsCreateObject( sessionHandle,
                              "/ahizdl",
                              strlen("/ahizdl"),
                              VDS_FOLDER );
   if ( errcode != VDS_OK )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsCreateObject( sessionHandle,
                              "/ahizdl/test",
                              strlen("/ahizdl/test"),
                              VDS_HASH_MAP );
   if ( errcode != VDS_OK )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsHashMapOpen( sessionHandle,
                             "/ahizdl/test",
                             strlen("/ahizdl/test"),
                             &objHandle );
   if ( errcode != VDS_OK )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsHashMapInsert( objHandle,
                               key,
                               6,
                               data,
                               0 );
   if ( errcode != VDS_INVALID_LENGTH )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

