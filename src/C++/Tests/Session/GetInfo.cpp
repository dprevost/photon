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
#include <vdsf/vds>
#include <iostream>

using namespace std;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   vdsProcess process;
   vdsSession session;
   string name = "/cpp_session_getinfo", msg;
   vdsInfo info, info2;
   size_t allocSpace;
   vdsObjectDefinition folderDef;

   memset( &folderDef, 0, sizeof folderDef );
   folderDef.type = VDS_FOLDER;

   try {
      if ( argc > 1 ) {
         process.Init( argv[1] );
      }
      else {
         process.Init( "10701" );
      }
      session.Init();
      
   }
   catch( vdsException exc ) {
      cerr << "Test failed in init phase, error = " << exc.Message() << endl;
      cerr << "Is the watchdog running?" << endl;
      return 1;
   }

   // Invalid arguments to tested function.

   try {
      session.GetInfo( NULL );
      // Should never come here
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   catch( vdsException exc ) {
      if ( exc.ErrorCode() != VDS_NULL_POINTER ) {
         cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
         return 1;
      }
   }
   
   
   // End of invalid args. This call should succeed.
   try {
      session.GetInfo( &info );
   }
   catch( vdsException exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }
   allocSpace = info.allocatedSizeInBytes;

   try {
      session.CreateObject( name, &folderDef );
   }
   catch( vdsException exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }
   
   try {
      session.GetInfo( &info2 );
   }
   catch( vdsException exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }
   
   if ( info.allocatedSizeInBytes >= info2.allocatedSizeInBytes ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   if ( info.numObjects+1 != info2.numObjects ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   if ( info.numGroups+1 != info2.numGroups ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   if ( info.numMallocs+1 != info2.numMallocs ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   if ( info.numFrees != info2.numFrees ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   
   try {
      session.Commit();
      session.DestroyObject( name );
      session.GetInfo( &info );
   }
   catch( vdsException exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   if ( info.allocatedSizeInBytes < info2.allocatedSizeInBytes ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   if ( info.numObjects != info2.numObjects ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   if ( info.numGroups != info2.numGroups ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   if ( info.numMallocs != info2.numMallocs ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   if ( info.numFrees != info2.numFrees ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   
   try {
      session.Commit();
      session.GetInfo( &info2 );
   }
   catch( vdsException exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   if ( info2.allocatedSizeInBytes != allocSpace ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   if ( info.numObjects != info2.numObjects+1 ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   if ( info.numGroups != info2.numGroups+1 ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   if ( info.numMallocs != info2.numMallocs ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   if ( info.numFrees+1 != info2.numFrees ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

