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
#include "Nucleus/Engine.h"
#include <photon/photon>
#include <iostream>

using namespace std;
using namespace pso;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   Process process;
   Session session;
   Queue queue(session);
   string fname = "/cpp_queue_status";
   string qname = fname + "/test";
   const char * data1 = "My Data1";
   const char * data2 = "My Data2";
   const char * data3 = "My Data3";
   psoObjStatus status;
   psoObjectDefinition folderDef;
   psoObjectDefinition queueDef = { PSO_QUEUE, PSO_DEF_USER_DEFINED };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
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
      session.Init();
      session.CreateObject( fname, folderDef, NULL, 0, NULL, 0 );
      session.CreateObject( qname,
                            queueDef,
                            NULL,
                            0,
                            (unsigned char *)fields,
                            sizeof(psoFieldDefinition) );
      queue.Open( qname );
      queue.Push( data1, strlen(data1) );
      queue.Push( data2, strlen(data2) );
      queue.Push( data3, strlen(data3) );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed in init phase, error = " << exc.Message() << endl;
      cerr << "Is the server running?" << endl;
      return 1;
   }

   // End of invalid args. This call should succeed.
   try {
      queue.Status( status );
   }
   catch( pso::Exception exc ) {
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
   if ( status.freeBytes == 0 || status.freeBytes >=PSON_BLOCK_SIZE ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

