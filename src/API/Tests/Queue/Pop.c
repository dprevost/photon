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
   VDS_HANDLE sessionHandle, objHandle;
   VDS_HANDLE objHandle2, sessionHandle2;
   int errcode;
   const char * data1 = "My Data1";
   char buffer[200];
   size_t length;

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
   errcode = vdsInitSession( &sessionHandle2 );
   if ( errcode != VDS_OK )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsCreateObject( sessionHandle,
                              "/aqpopp",
                              strlen("/aqpopp"),
                              VDS_FOLDER );
   if ( errcode != VDS_OK )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsCreateObject( sessionHandle,
                              "/aqpopp/test",
                              strlen("/aqpopp/test"),
                              VDS_QUEUE );
   if ( errcode != VDS_OK )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsCommit( sessionHandle );
   if ( errcode != VDS_OK )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsQueueOpen( sessionHandle,
                           "/aqpopp/test",
                           strlen("/aqpopp/test"),
                           &objHandle );
   if ( errcode != VDS_OK )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = vdsQueueOpen( sessionHandle2,
                           "/aqpopp/test",
                           strlen("/aqpopp/test"),
                           &objHandle2 );
   if ( errcode != VDS_OK )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsQueuePush( objHandle, data1, strlen(data1) );
   if ( errcode != VDS_OK )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsCommit( sessionHandle );
   if ( errcode != VDS_OK )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdsQueuePop( NULL,
                          buffer,
                          200,
                          &length );
   if ( errcode != VDS_NULL_HANDLE )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdsQueuePop( objHandle,
                          NULL,
                          200,
                          &length );
   if ( errcode != VDS_NULL_POINTER )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdsQueuePop( objHandle,
                          buffer,
                          2,
                          &length );
   if ( errcode != VDS_INVALID_LENGTH )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdsQueuePop( objHandle,
                          buffer,
                          200,
                          NULL );
   if ( errcode != VDS_NULL_POINTER )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdsQueuePop( objHandle,
                          buffer,
                          200,
                          &length );
   if ( errcode != VDS_OK )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( memcmp( buffer, data1, strlen(data1) ) != 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );

   /*
    * Additional stuff to check while the Pop is uncommitted:
    *  - cannot get access to the item from first session.
    *  - can get access to the item from second session.
    *  - cannot modify it from second session.
    */
   errcode = vdsQueueGetFirst( objHandle,
                               buffer,
                               200,
                               &length );
   if ( errcode != VDS_ITEM_IS_IN_USE )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = vdsQueueGetFirst( objHandle2,
                               buffer,
                               200,
                               &length );
   if ( errcode != VDS_OK )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = vdsQueuePop( objHandle2,
                          buffer,
                          200,
                          &length );
   if ( errcode != VDS_ITEM_IS_IN_USE )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /*
    * Additional stuff to check after the commit:
    *  - cannot get access to the item from first session.
    *  - cannot get access to the item from second session.
    *  And that the error is VDS_EMPTY.
    *
    * Note to make sure that the deleted item is still in the VDS,
    * we first call GetFirst to get a pointer to the item from
    * session 2 (the failed call to Pop just above release that
    * internal pointer).
    */
   errcode = vdsQueueGetFirst( objHandle2,
                               buffer,
                               200,
                               &length );
   if ( errcode != VDS_OK )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = vdsCommit( sessionHandle );
   if ( errcode != VDS_OK )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsQueueGetFirst( objHandle,
                               buffer,
                               200,
                               &length );
   if ( errcode != VDS_IS_EMPTY )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdsQueueGetFirst( objHandle2,
                               buffer,
                               200,
                               &length );
   if ( errcode != VDS_IS_EMPTY )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

