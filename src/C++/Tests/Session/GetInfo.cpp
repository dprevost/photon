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
   Session session;
   string name = "/cpp_session_getinfo", msg;
   psoInfo info, info2;
   size_t allocSpace;

   try {
      if ( argc > 1 ) {
         process.Init( argv[1], argv[0] );
      }
      else {
         process.Init( "10701", argv[0] );
      }
      session.Init();
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed in init phase, error = " << exc.Message() << endl;
      cerr << "Is the server running?" << endl;
      return 1;
   }

   try {
      session.GetInfo( info );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }
   allocSpace = info.allocatedSizeInBytes;

   try {
      session.CreateFolder( name );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }
   
   try {
      session.GetInfo( info2 );
   }
   catch( pso::Exception exc ) {
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
      session.GetInfo( info );
   }
   catch( pso::Exception exc ) {
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
      session.GetInfo( info2 );
   }
   catch( pso::Exception exc ) {
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

