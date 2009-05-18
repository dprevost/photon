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

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   Process process;
   Session session;
   HashMap * hashmap;
   string fname = "/cpp_hashmap_definition";
   string hname = fname + "/test";

   psoObjectDefinition mapDef = { PSO_HASH_MAP, 0, 0, 0 };
   psoFieldDefinition fields[5] = {
      { "field1", PSO_TINYINT,       {0} },
      { "field2", PSO_INTEGER,       {0} },
      { "field3", PSO_CHAR,         {30} },
      { "field4", PSO_SMALLINT,      {0} },
      { "field5", PSO_LONGVARBINARY, {0} }
   };
   psoKeyFieldDefinition keys[2] = {
      { "LastName",  PSO_KEY_CHAR,    30 },
      { "FirstName", PSO_KEY_VARCHAR, 30 }
   };
   
   DataDefinition * retDataDef = NULL;
   KeyDefinition  * retKeyDef  = NULL;
   unsigned char * retFields = NULL;
   unsigned char * retKeys   = NULL;

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
                                 "cpp_hashmap_definition",
                                 PSO_DEF_PHOTON_ODBC_SIMPLE,
                                 (unsigned char *)fields,
                                 5*sizeof(psoFieldDefinition) );
      KeyDefinition keyDefObj( session, 
                               "cpp_hashmap_definition",
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)keys,
                               2*sizeof(psoKeyFieldDefinition) );
      session.CreateMap( hname, mapDef, dataDefObj, keyDefObj );
      hashmap = new HashMap( session, hname );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   try {
      retDataDef = hashmap->GetDataDefinition();
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   try {
      if ( retDataDef->GetType() != PSO_DEF_PHOTON_ODBC_SIMPLE ) {
         cerr << "Test failed - line " << __LINE__ << endl;
         return 1;
      }
      if ( retDataDef->GetLength() != 5*sizeof(psoFieldDefinition) ) {
         cerr << "Test failed - line " << __LINE__ << endl;
         return 1;
      }
      retFields = new unsigned char [5*sizeof(psoFieldDefinition)];
      retDataDef->GetDefinition( retFields, 5*sizeof(psoFieldDefinition) );
      if ( memcmp( retFields, fields, 5*sizeof(psoFieldDefinition) ) != 0 ) {
         cerr << "Test failed - line " << __LINE__ << endl;
         return 1;
      }
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   try {
      retKeyDef = hashmap->GetKeyDefinition();
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   try {
      if ( retKeyDef->GetType() != PSO_DEF_PHOTON_ODBC_SIMPLE ) {
         cerr << "Test failed - line " << __LINE__ << endl;
         return 1;
      }
      if ( retKeyDef->GetLength() != 2*sizeof(psoKeyFieldDefinition) ) {
         cerr << "Test failed - line " << __LINE__ << endl;
         return 1;
      }
      retKeys = new unsigned char [2*sizeof(psoKeyFieldDefinition)];
      retKeyDef->GetDefinition( retKeys, 2*sizeof(psoKeyFieldDefinition) );
      if ( memcmp( retKeys, keys, 2*sizeof(psoKeyFieldDefinition) ) != 0 ) {
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

