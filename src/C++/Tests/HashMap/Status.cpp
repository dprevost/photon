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
   vdsHashMap hashmap(session);
   string fname = "/cpp_hashmap_status";
   string hname = fname + "/test";

   const char * key1 = "My Key1";
   const char * key2 = "My Key2";
   const char * key3 = "My Key3";
   const char * data = "My Data";
   vdsObjStatus status;

   try {
      if ( argc > 1 ) {
         process.Init( argv[1] );
      }
      else {
         process.Init( "10701" );
      }
      session.Init();
      session.CreateObject( fname, VDS_FOLDER );
      session.CreateObject( hname, VDS_HASH_MAP );
      hashmap.Open( hname );
      hashmap.Insert( key1, 7, data, 7 );
      hashmap.Insert( key2, 7, data, 7 );
      hashmap.Insert( key3, 7, data, 7 );
   }
   catch( vdsException exc ) {
      cerr << "Test failed in init phase, error = " << exc.Message() << endl;
      cerr << "Is the watchdog running?" << endl;
      return 1;
   }

   // Invalid arguments to tested function.

   try {
      hashmap.Status( NULL );
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
      hashmap.Status( &status );
   }
   catch( vdsException exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   if ( status.numDataItem != 3 ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   if ( status.numBlocks != 1 ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   if ( status.numBlockGroup != 1 ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   if ( status.freeBytes == 0 || status.freeBytes >=VDSE_BLOCK_SIZE ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
