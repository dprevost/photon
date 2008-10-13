/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@photonsoftware.org>
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
#include <photon/photon>
#include <iostream>

using namespace std;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   psoProcess process;
   psoSession session1, session2;
   psoQueue queue1(session1), queue2(session2);
   string fname = "/cpp_queue_open";
   string qname = fname + "/test";
   const char * c_name = "/cpp_queue_open/test";
   psoObjectDefinition folderDef;
   psoObjectDefinition queueDef = { 
      PSO_QUEUE,
      1, 
      { PSO_KEY_VAR_BINARY, 0, 0, 0 }, 
      { { "Field_1", PSO_VAR_STRING, 0, 4, 10, 0, 0 } } 
   };

   memset( &folderDef, 0, sizeof folderDef );
   folderDef.type = PSO_FOLDER;

   try {
      if ( argc > 1 ) {
         process.Init( argv[1] );
      }
      else {
         process.Init( "10701" );
      }
      session1.Init();
      session2.Init();
      session1.CreateObject( fname, folderDef );
      session1.CreateObject( qname, queueDef );
   }
   catch( psoException exc ) {
      cerr << "Test failed in init phase, error = " << exc.Message() << endl;
      cerr << "Is the watchdog running?" << endl;
      return 1;
   }

   // Invalid arguments to tested function.

   try {
      queue1.Open( NULL, strlen(c_name) );
      // Should never come here
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   catch( psoException exc ) {
      if ( exc.ErrorCode() != PSO_INVALID_OBJECT_NAME ) {
         cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
         return 1;
      }
   }

   try {
      queue1.Open( c_name, 0 );
      // Should never come here
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   catch( psoException exc ) {
      if ( exc.ErrorCode() != PSO_INVALID_LENGTH ) {
         cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
         return 1;
      }
   }

   try {
      queue1.Open( fname ); // Name of the folder
      // Should never come here
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   catch( psoException exc ) {
      if ( exc.ErrorCode() != PSO_WRONG_OBJECT_TYPE ) {
         cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
         return 1;
      }
   }
   
   try {
      queue1.Open( "" );
      // Should never come here
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   catch( psoException exc ) {
      if ( exc.ErrorCode() != PSO_INVALID_LENGTH ) {
         cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
         return 1;
      }
   }

   // End of invalid args. This call should succeed. */
   try {
      queue1.Open( qname );
   }
   catch( psoException exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }
   try {
      queue2.Open( c_name, strlen(c_name) );
      // Should never come here
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   catch( psoException exc ) {
      if ( exc.ErrorCode() != PSO_OBJECT_IS_IN_USE ) {
         cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
         return 1;
      }
   }
   
   try {
      session1.Commit();
      queue2.Open( c_name, strlen(c_name) );
   }
   catch( psoException exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

