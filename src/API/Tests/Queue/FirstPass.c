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
#include <vdsf/vds.h>
#include "Tests/PrintError.h"
#include "API/Queue.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   VDS_HANDLE handle, sessionHandle;
   int errcode;
   const char * data1 = "My Data1";
   vdsDataEntry entry;

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

   errcode = vdsCreateObject( sessionHandle,
                              "/aqfp",
                              strlen("/aqfp"),
                              VDS_FOLDER );
   if ( errcode != VDS_OK )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsCreateObject( sessionHandle,
                              "/aqfp/test",
                              strlen("/aqfp/test"),
                              VDS_QUEUE );
   if ( errcode != VDS_OK )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsQueueOpen( sessionHandle,
                           "/aqfp/test",
                           strlen("/aqfp/test"),
                           &handle );
   if ( errcode != VDS_OK )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsQueuePush( handle, data1, strlen(data1) );
   if ( errcode != VDS_OK )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsQueueGetFirst( handle,
                               &entry );
   if ( errcode != VDS_OK )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( memcmp( entry.data, data1, strlen(data1) ) != 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

