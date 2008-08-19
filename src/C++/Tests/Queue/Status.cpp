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
#include "Nucleus/Engine.h"
#include <photon/vds>
#include <iostream>

using namespace std;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   vdsProcess process;
   vdsSession session;
   vdsQueue queue(session);
   string fname = "/cpp_queue_status";
   string qname = fname + "/test";
   const char * data1 = "My Data1";
   const char * data2 = "My Data2";
   const char * data3 = "My Data3";
   vdsObjStatus status;
   vdsObjectDefinition folderDef;
   vdsObjectDefinition queueDef = { 
      VDS_QUEUE,
      1, 
      { VDS_KEY_VAR_BINARY, 0, 0, 0 }, 
      { { "Field_1", VDS_VAR_STRING, 0, 4, 10, 0, 0 } } 
   };

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
      session.CreateObject( fname, &folderDef );
      session.CreateObject( qname, &queueDef );
      queue.Open( qname );
      queue.Push( data1, strlen(data1) );
      queue.Push( data2, strlen(data2) );
      queue.Push( data3, strlen(data3) );
   }
   catch( vdsException exc ) {
      cerr << "Test failed in init phase, error = " << exc.Message() << endl;
      cerr << "Is the watchdog running?" << endl;
      return 1;
   }

   // Invalid arguments to tested function.

   try {
      queue.Status( NULL );
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
      queue.Status( &status );
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
   if ( status.freeBytes == 0 || status.freeBytes >=PSN_BLOCK_SIZE ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

