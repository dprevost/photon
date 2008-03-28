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
   VDS_HANDLE objHandle2, sessionHandle2;
   int errcode;
   const char * key  = "My Key";
   const char * data = "My Data";
   size_t length, keyLength;
   char buffer[20], keyBuff[20];

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
                              "/ahdp",
                              strlen("/ahdp"),
                              VDS_FOLDER );
   if ( errcode != VDS_OK )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsCreateObject( sessionHandle,
                              "/ahdp/test",
                              strlen("/ahdp/test"),
                              VDS_HASH_MAP );
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

   errcode = vdsHashMapOpen( sessionHandle,
                             "/ahdp/test",
                             strlen("/ahdp/test"),
                             &objHandle );
   if ( errcode != VDS_OK )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = vdsHashMapOpen( sessionHandle2,
                             "/ahdp/test",
                             strlen("/ahdp/test"),
                             &objHandle2 );
   if ( errcode != VDS_OK )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsHashMapInsert( objHandle,
                               key,
                               6,
                               data,
                               7 );
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
   
   errcode = vdsHashMapDelete( objHandle,
                               key,
                               6 );
   if ( errcode != VDS_OK )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /*
    * Additional stuff to check while the Delete() is uncommitted:
    *  - cannot get access to the item from first session.
    *  - can get access to the item from second session.
    *  - cannot modify it from second session.
    */
   errcode = vdsHashMapGet( objHandle, 
                            key, 
                            6,
                            buffer, 20, &length );
   if ( errcode != VDS_ITEM_IS_DELETED )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = vdsHashMapGet( objHandle2, 
                            key, 
                            6,
                            buffer, 20, &length );
   if ( errcode != VDS_OK )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = vdsHashMapDelete( objHandle2, key, 6 );
   if ( errcode != VDS_ITEM_IS_IN_USE )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = vdsHashMapGetFirst( objHandle2, keyBuff, 20, buffer, 20, 
                                 &keyLength, &length );
   if ( errcode != VDS_OK )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = vdsHashMapGetFirst( objHandle, keyBuff, 20, buffer, 20, 
                                 &keyLength, &length );
   if ( errcode != VDS_IS_EMPTY )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /*
    * Additional stuff to check after the commit:
    *  - cannot get access to the item from first session.( no such item)
    *  - cannot get access to the item from second session.
    *  - can insert new item with same key.
    *
    * Note to make sure that the deleted item is still in the VDS,
    * we will act on the first session (the second session holds
    * an internal pointer to the data record).
    */
   errcode = vdsCommit( sessionHandle );
   if ( errcode != VDS_OK )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdsHashMapGet( objHandle, 
                            key, 
                            6,
                            buffer, 20, &length );
   if ( errcode != VDS_NO_SUCH_ITEM )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = vdsHashMapInsert( objHandle,
                               key,
                               6,
                               data,
                               7 );
   if ( errcode != VDS_OK )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* rollback to be able to test the Get() with session 2 */
   errcode = vdsRollback( sessionHandle );
   if ( errcode != VDS_OK )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdsHashMapGet( objHandle2, 
                            key, 
                            6,
                            buffer, 20, &length );
   if ( errcode != VDS_NO_SUCH_ITEM )
   {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   vdsExit();

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

