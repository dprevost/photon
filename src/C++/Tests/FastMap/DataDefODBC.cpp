/*
 * Copyright (C) 2009 Daniel Prevost <dprevost@photonsoftware.org>
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

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   Process process;
   Session session;
   FastMap * map;
   string fname = "/cpp_fastmap_fielddefODBC";
   string hname = fname + "/test";
   
   size_t len;
   psoObjectDefinition mapDef = { PSO_FAST_MAP, 0, 0, 0 };

   DataDefBuilderODBC fieldDef( 5 );
   KeyDefBuilderODBC keyDef( 3 );

   DataDefinition * returnedDataDef;
   KeyDefinition * returnedKeyDef;
   unsigned char * buffer;
   
   try {
      fieldDef.AddField( "field1", 6, PSO_TINYINT,       0, 0, 0 );
      fieldDef.AddField( "field2", 6, PSO_INTEGER,       0, 0, 0 );
      fieldDef.AddField( "field3", 6, PSO_CHAR,         30, 0, 0 );
      fieldDef.AddField( "field4", 6, PSO_SMALLINT,      0, 0, 0 );
      fieldDef.AddField( "field5", 6, PSO_LONGVARBINARY, 0, 0, 0 );
      keyDef.AddKeyField( "keyfield1", 9, PSO_KEY_INTEGER,       0 );
      keyDef.AddKeyField( "keyfield2", 9, PSO_KEY_CHAR,         30 );
      keyDef.AddKeyField( "keyfield3", 9, PSO_KEY_LONGVARBINARY, 0 );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

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
                                 "cpp_fastmap_datadef_odbc",
                                 PSO_DEF_PHOTON_ODBC_SIMPLE,
                                 fieldDef.GetDefinition(),
                                 fieldDef.GetDefLength() );
      KeyDefinition keyDefObj( session, 
                               "cpp_fastmap_datadef_odbc",
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               keyDef.GetDefinition(),
                               keyDef.GetDefLength() );

      session.CreateMap( hname, mapDef, dataDefObj, keyDefObj );

      map = new FastMap( session, hname );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   try {
      returnedDataDef = map->GetDataDefinition();
      returnedKeyDef  = map->GetKeyDefinition();
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   try {
      len = 5 * sizeof(psoFieldDefinition);
      buffer = new unsigned char [len];
      if ( len != returnedDataDef->GetLength() ) {
         cerr << "Test failed - line " << __LINE__ << endl;
         return 1;
      }
      returnedDataDef->GetDefinition( buffer, len );      
      if ( memcmp( fieldDef.GetDefinition(), buffer, len ) != 0 ) {
         cerr << "Test failed - line " << __LINE__ << endl;
         return 1;
      }
      delete [] buffer;
      
      len = 3 * sizeof(psoKeyFieldDefinition);
      buffer = new unsigned char [len];
      if ( len != returnedKeyDef->GetLength() ) {
         cerr << "Test failed - line " << __LINE__ << endl;
         return 1;
      }
      returnedKeyDef->GetDefinition( buffer, len );      
      if ( memcmp( keyDef.GetDefinition(), buffer, len ) != 0 ) {
         cerr << "Test failed - line " << __LINE__ << endl;
         return 1;
      }

   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

