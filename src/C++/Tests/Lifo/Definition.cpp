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
   Lifo queue(session);
   string fname = "/cpp_lifo_definition";
   string hname = fname + "/test";

   size_t len;
   psoObjectDefinition folderDef = {
      PSO_FOLDER, PSO_DEF_NONE, PSO_DEF_NONE };
   psoObjectDefinition queueDef = { 
      PSO_LIFO, PSO_DEF_PHOTON_ODBC_SIMPLE, PSO_DEF_NONE };
   FieldDefinitionODBC fieldDef( 5 );
   unsigned char * fields = NULL;
   uint32_t fieldsLength = 0;
   psoObjectDefinition retDef;
   
   try {
      fieldDef.AddField( "field1", 6, PSO_TINYINT,       0, 0, 0 );
      fieldDef.AddField( "field2", 6, PSO_INTEGER,       0, 0, 0 );
      fieldDef.AddField( "field3", 6, PSO_CHAR,         30, 0, 0 );
      fieldDef.AddField( "field4", 6, PSO_SMALLINT,      0, 0, 0 );
      fieldDef.AddField( "field5", 6, PSO_LONGVARBINARY, 0, 0, 0 );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   try {
      queue.Definition( retDef, fields, fieldsLength );
      // Should never come here
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   catch( pso::Exception exc ) {
      if ( exc.ErrorCode() != PSO_NULL_HANDLE ) {
         cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
         return 1;
      }
   }

   try {
      if ( argc > 1 ) {
         process.Init( argv[1] );
      }
      else {
         process.Init( "10701" );
      }
      session.Init();
      session.CreateObject( fname, folderDef, NULL, 0, NULL, 0 );
      session.CreateObject( hname, queueDef, NULL, &fieldDef );
      queue.Open( hname );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed in init phase, error = " << exc.Message() << endl;
      cerr << "Is the server running?" << endl;
      return 1;
   }

   try {
      // This is valid
      queue.Definition( retDef, NULL, 0 );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   len = 5 * sizeof(psoFieldDefinition);
   try {
      queue.DefinitionLength( &fieldsLength );
      if ( fieldsLength != len ) {
         cerr << "Test failed - line " << __LINE__ << endl;
         return 1;
      }
      fields = new unsigned char [fieldsLength];
      queue.Definition( retDef, fields, fieldsLength );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   if ( memcmp( fieldDef.GetDefinition(), fields, len ) != 0 ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

