/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@photonsoftware.org>
 *
 * This file is part of Photon (photonsoftware.org).
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
#include <photon/photon>
#include <iostream>

using namespace std;
using namespace pso;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   Process process;
   Session session;
   Folder folder(session);
   string name = "/cpp_folder_createXML";
   string subname = "test";
   psoObjectDefinition def; 
   
   char buff[1000];
   char src_path[PATH_MAX] = {"../../../XML/photon_meta10.xsd"};

   memset( &def, 0, sizeof def );
   def.type = PSO_FOLDER;
   
   try {
      if ( argc > 1 ) {
         process.Init( argv[1] );
      }
      else {
         process.Init( "10701" );
      }
      if ( argc > 2 ) {
         strcpy( src_path, argv[2] );
         strcat( src_path, "/src/XML/photon_meta10.xsd" );
      }
      session.Init();
      session.CreateObject( name, def );
      folder.Open( name );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed in init phase, error = " << exc.Message() << endl;
      cerr << "Is the watchdog running?" << endl;
      return 1;
   }
   
   // Invalid arguments to tested function.
   
   strcpy( buff, "<?xml version=\"1.0\"?>\n"
      "<folder xmlns=\"http://photonsoftware.org/photon_meta\" "
      "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" "
      "xsi:schemaLocation=\"http://photonsoftware.org/photon_meta ");
   strcat( buff, src_path);
   strcat( buff, "\" "
      "objName=\"My_name\" > "
      "</folder2>" );
//fprintf( stderr, "%s\n", buff );

   try {
      folder.CreateObjectXML( NULL, strlen(buff) );
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   catch( pso::Exception exc ) {
      if ( exc.ErrorCode() != PSO_NULL_POINTER ) {
         cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
         return 1;
      }
   }

   try {
      folder.CreateObjectXML( buff, 0 );
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   catch( pso::Exception exc ) {
      if ( exc.ErrorCode() != PSO_INVALID_LENGTH ) {
         cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
         return 1;
      }
   }

   // The closing tag is </folder2> instead of folder.
   try {
      folder.CreateObjectXML( buff, strlen(buff) );
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   catch( pso::Exception exc ) {
      if ( exc.ErrorCode() != PSO_XML_READ_ERROR ) {
         cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
         return 1;
      }
   }

   // End of invalid args. This call should succeed.

   strcpy( buff, "<?xml version=\"1.0\"?>\n"
      "<folder xmlns=\"http://photonsoftware.org/photon_meta\" "
      "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" "
      "xsi:schemaLocation=\"http://photonsoftware.org/photon_meta ");
   strcat( buff, src_path);
   strcat( buff, "\" "
      "objName=\"My_name\" >"
      "</folder>" );

   try {
      folder.CreateObjectXML( buff, strlen(buff) );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

