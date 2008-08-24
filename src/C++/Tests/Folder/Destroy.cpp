/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@users.sourceforge.net>
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
   psoSession session, session2;
   psoFolder folder(session), f1(session), f2(session2);
   string name = "/cpp_folder_destroy";
   string subname = "test";
   string fullname = "/cpp_folder_destroy/test";
   psoObjectDefinition def; 

   memset( &def, 0, sizeof def );
   def.type = PSO_FOLDER;
   
   try {
      if ( argc > 1 ) {
         process.Init( argv[1] );
      }
      else {
         process.Init( "10701" );
      }
      session.Init();
      session2.Init();
      session.CreateObject( name, &def );
      folder.Open( name );
   }
   catch( psoException exc ) {
      cerr << "Test failed in init phase, error = " << exc.Message() << endl;
      cerr << "Is the watchdog running?" << endl;
      return 1;
   }
   
   try {
      // Destroy non-existing object.
      folder.DestroyObject( subname.c_str(), subname.length() );
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   catch( psoException exc ) {
      if ( exc.ErrorCode() != PSO_NO_SUCH_OBJECT ) {
         cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
         return 1;
      }
   }
   
   try {
      folder.CreateObject( subname, &def );
   }
   catch( psoException exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   try {
      // Destroy without a commit - should fail
      folder.DestroyObject( subname );
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   catch( psoException exc ) {
      if ( exc.ErrorCode() != PSO_OBJECT_IS_IN_USE ) {
         cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
         return 1;
      }
   }

   try { session.Commit(); }
   catch( psoException exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   // Invalid arguments to tested function.

   try {
      folder.DestroyObject( NULL, subname.length() );
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
      folder.DestroyObject( subname.c_str(), 0 );
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
      // Invalid handle
      f1.DestroyObject( subname );
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   catch( psoException exc ) {
      if ( exc.ErrorCode() != PSO_NULL_HANDLE ) {
         cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
         return 1;
      }
   }

   // End of invalid args. This call should succeed.

   try {
      folder.DestroyObject( subname );
   }
   catch( psoException exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   // Open on the same session - should fail
   try {
      f1.Open( fullname );
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   catch( psoException exc ) {
      if ( exc.ErrorCode() != PSO_OBJECT_IS_DELETED ) {
         cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
         return 1;
      }
   }
   

   // Open with a different session - should work
   try {
      f2.Open( fullname );
   }
   catch( psoException exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

