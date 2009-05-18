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
   FastMap       * hashmap;
   FastMapEditor * editor;
   string fname = "/cpp_fastmap_status";
   string hname = fname + "/test";

   const char * key1 = "My Key1";
   const char * key2 = "My Key2";
   const char * key3 = "My Key3";
   const char * data = "My Data";
   psoObjStatus status;
   psoObjectDefinition mapDef = { PSO_FAST_MAP, 0, 0, 0 };
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
      session.Init();
      session.CreateFolder( fname );

      DataDefinition dataDefObj( session, 
                                 "cpp_fastmap_status",
                                 PSO_DEF_PHOTON_ODBC_SIMPLE,
                                 (unsigned char *)fields,
                                 sizeof(psoFieldDefinition) );
      KeyDefinition keyDefObj( session,
                               "cpp_fastmap_status",
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)&keys,
                               sizeof(psoKeyFieldDefinition) );
      session.CreateMap( hname, mapDef, dataDefObj, keyDefObj );

      editor = new FastMapEditor( session, hname );
      editor->Insert( key1, 7, data, 7 );
      editor->Insert( key2, 7, data, 7 );
      editor->Insert( key3, 7, data, 7 );
      editor->Close();
      session.Commit();
      hashmap = new FastMap( session, hname );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   // Since we use a reference, there are no invalid arguments to this
   // tested function.

   // End of invalid args. This call should succeed.
   try {
      hashmap->Status( status );
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

