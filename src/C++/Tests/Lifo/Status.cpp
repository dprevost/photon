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
   Lifo queue;
   string fname = "/cpp_queue_status";
   string qname = fname + "/test";
   const char * data1 = "My Data1";
   const char * data2 = "My Data2";
   const char * data3 = "My Data3";
   psoObjStatus status;
   psoObjectDefinition queueDef = { PSO_LIFO, 0, 0, 0 };
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
      session.Init();
      session.CreateFolder( fname );

      DataDefinition dataDefObj( session,
                                 "cpp_lifo_status",
                                 PSO_DEF_PHOTON_ODBC_SIMPLE,
                                 (unsigned char *)fields,
                                 sizeof(psoFieldDefinition) );
      session.CreateQueue( qname, queueDef, dataDefObj );
      queue.Open( session, qname );
      queue.Push( data1, strlen(data1) );
      queue.Push( data2, strlen(data2) );
      queue.Push( data3, strlen(data3) );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

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

