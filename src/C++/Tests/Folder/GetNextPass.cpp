/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@users.sourceforge.net>
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
#include <photon/vds>
#include "Tests/PrintError.h"
#include <iostream>

using namespace std;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   vdsProcess process;
   vdsSession session;
   vdsFolder folder(session);
   string name = "/cpp_folder_getnext";
   string sub1name = name + "/f1";
   string sub2name = name + "/f2";
   vdsFolderEntry entry;
   int rc;
   vdsObjectDefinition def; 

   memset( &def, 0, sizeof def );
   def.type = VDS_FOLDER;
   
   try {
      if ( argc > 1 ) {
         process.Init( argv[1] );
      }
      else {
         process.Init( "10701" );
      }
      session.Init();
      session.CreateObject( name, &def );
      session.CreateObject( sub1name, &def );
      session.CreateObject( sub2name, &def );
      folder.Open( name );
   }
   catch( vdsException exc ) {
      cerr << "Test failed in init phase, error = " << exc.Message() << endl;
      cerr << "Is the watchdog running?" << endl;
      return 1;
   }

   // No GetFirst
   try {
      folder.GetNext( &entry );
   }
   catch( vdsException exc ) {
      if ( exc.ErrorCode() != VDS_INVALID_ITERATOR ) {
         cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
         return 1;
      }
   }

   try {
      folder.GetFirst( &entry );
   }
   catch( vdsException exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   // Invalid arguments to tested function.

   try {
      folder.GetNext( NULL );
   }
   catch( vdsException exc ) {
      if ( exc.ErrorCode() != VDS_NULL_POINTER ) {
         cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
         return 1;
      }
   }
   
   // End of invalid args. This call should succeed.
   try {
      rc = folder.GetNext( &entry );
   }
   catch( vdsException exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }
   if ( rc != 0 ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   
   try {
      rc = folder.GetNext( &entry );
   }
   catch( vdsException exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }
   if ( rc != VDS_REACHED_THE_END ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

