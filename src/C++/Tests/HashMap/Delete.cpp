/*
 * Copyright (C) 2008-2009 Daniel Prevost <dprevost@photonsoftware.org>
 *
 * This file is part of Photon (photonsoftware.org).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 or version 3 as published by the 
 * Free Software Foundation and appearing in the file COPYING.GPL2 and 
 * COPYING.GPL3 included in the packaging of this software.
 *
 * Licensees holding a valid Photon Commercial license can use this file 
 * in accordance with the terms of their license.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Common/Common.h"
#include <photon/photon>
#include <iostream>

using namespace std;
using namespace pso;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   Process process;
   Session session1, session2;
   HashMap map1, map2;
   string fname = "/cpp_hashmap_delete";
   string hname = fname + "/test";

   const char * key  = "My Key";
   const char * data = "My Data";
   uint32_t length, keyLength;
   char buffer[20], keyBuff[20];
   int rc;
   psoObjectDefinition mapDef = { PSO_HASH_MAP, 0, 0, 0 };
   psoKeyFieldDefinition keys = { "MyKey", PSO_KEY_VARBINARY, 20 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} } 
   };

   try {
      if ( argc > 1 ) {
         process.Init( argv[1], argv[0] );
      }
      else {
         process.Init( "10701", argv[0] );
      }
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed in init phase, error = " << exc.Message() << endl;
      cerr << "Is the server running?" << endl;
      return 1;
   }
   
   try {
      session1.Init();
      session2.Init();
      session1.CreateFolder( fname );

      DataDefinition dataDefObj( session1, 
                                 "cpp_hashmap_delete",
                                 PSO_DEF_PHOTON_ODBC_SIMPLE,
                                 (unsigned char *)fields,
                                 5*sizeof(psoFieldDefinition) );
      KeyDefinition keyDefObj( session1, 
                               "cpp_hashmap_delete",
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)&keys,
                               2*sizeof(psoKeyFieldDefinition) );

      session1.CreateMap( hname,
                          mapDef,
                          dataDefObj,
                          keyDefObj );
      map1.Open( session1, hname );
      map1.Insert( key, 6, data, 7 );
      session1.Commit();
      map2.Open( session2, hname );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   // Invalid arguments to tested function.

   try {
      map1.Delete( NULL, 6 );
      // Should never come here
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   catch( pso::Exception exc ) {
      if ( exc.ErrorCode() != PSO_NULL_POINTER ) {
         cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
         return 1;
      }
   }

   try {
      map1.Delete( key, 0 );
      // Should never come here
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   catch( pso::Exception exc ) {
      if ( exc.ErrorCode() != PSO_INVALID_LENGTH ) {
         cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
         return 1;
      }
   }

   // End of invalid args. This call should succeed.
   try {
      map1.Delete( key, 6 );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   /*
    * Additional stuff to check while the Delete() is uncommitted:
    *  - cannot get access to the item from first session.
    *  - can get access to the item from second session.
    *  - cannot modify it from second session.
    */
   try { 
      map1.Get( key, 6, buffer, 20, length );
      // Should never come here
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   catch( pso::Exception exc ) {
      if ( exc.ErrorCode() != PSO_ITEM_IS_DELETED ) {
         cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
         return 1;
      }
   }

   try {
      map2.Get( key, 6, buffer, 20, length );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   try { 
      map2.Delete( key, 6 );
      // Should never come here
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   catch( pso::Exception exc ) {
      if ( exc.ErrorCode() != PSO_ITEM_IS_IN_USE ) {
         cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
         return 1;
      }
   }
   
   try {
      map2.GetFirst( keyBuff, 20, buffer, 20, keyLength, length );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   try {
      rc = map1.GetFirst( keyBuff, 20, buffer, 20, keyLength, length );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }
   if ( rc != PSO_IS_EMPTY ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   
   /*
    * Additional stuff to check after the commit:
    *  - cannot get access to the item from first session.( no such item)
    *  - cannot get access to the item from second session.
    *  - can insert new item with same key.
    *
    * Note to make sure that the deleted item is still in shared memory,
    * we will act on the first session (the second session holds
    * an internal pointer to the data record).
    */
   try {
      session1.Commit();
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }
   
   try { 
      map1.Get( key, 6, buffer, 20, length );
      // Should never come here
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   catch( pso::Exception exc ) {
      if ( exc.ErrorCode() != PSO_NO_SUCH_ITEM ) {
         cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
         return 1;
      }
   }

   try {
      map1.Insert( key, 6, data, 7 );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }
   // rollback the insert to be able to test the Get() with session 2.
   try {
      session1.Rollback();
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }
   
   try { 
      map2.Get( key, 6, buffer, 20, length );
      // Should never come here
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   catch( pso::Exception exc ) {
      if ( exc.ErrorCode() != PSO_NO_SUCH_ITEM ) {
         cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
         return 1;
      }
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

