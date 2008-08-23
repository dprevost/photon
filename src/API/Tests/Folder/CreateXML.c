/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of photon (photonsoftware.org).
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
#include <photon/photon.h>
#include "Tests/PrintError.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   PSO_HANDLE sessionHandle, folderHandle;
   int errcode;
   char buff[1000];
   char src_path[PATH_MAX] = {"../../../XML/photon_meta10.xsd"};
   psoObjectDefinition def = { 
      PSO_FOLDER, 
      0, 
      { 0, 0, 0, 0}, 
      { { "", 0, 0, 0, 0, 0, 0} } 
   };
   
   if ( argc > 1 ) {
      errcode = psoInit( argv[1], 0 );
   }
   else {
      errcode = psoInit( "10701", 0 );
   }
   if ( argc > 2 ) {
      strcpy( src_path, argv[2] );
      strcat( src_path, "/src/XML/photon_meta10.xsd" );
   }
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = psoInitSession( &sessionHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoCreateObject( sessionHandle,
                              "/afcx",
                              strlen("/afcx"),
                              &def );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFolderOpen( sessionHandle,
                            "/afcx",
                            strlen("/afcx"),
                            &folderHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Invalid arguments to tested function. */

   strcpy( buff, "<?xml version=\"1.0\"?>\n"
      "<folder xmlns=\"http://photonsoftware.org/photon_meta\" "
      "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" "
      "xsi:schemaLocation=\"http://photonsoftware.org/photon_meta ");
   strcat( buff, src_path);
   strcat( buff, "\" "
      "objName=\"My_name\" > "
      "</folder2>" );
//fprintf( stderr, "%s\n", buff );

   errcode = psoFolderCreateObjectXML( folderHandle,
                                       NULL,
                                       strlen(buff) );
   if ( errcode != PSO_NULL_POINTER ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFolderCreateObjectXML( folderHandle,
                                       buff,
                                       0 );
   if ( errcode != PSO_INVALID_LENGTH ) { // 
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* The closing tag is </folder2> instead of folder. */
   errcode = psoFolderCreateObjectXML( folderHandle,
                                       buff,
                                       strlen(buff) );
   if ( errcode != PSO_XML_READ_ERROR ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   strcpy( buff, "<?xml version=\"1.0\"?>\n"
      "<folder xmlns=\"http://photonsoftware.org/photon_meta\" "
      "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" "
      "xsi:schemaLocation=\"http://photonsoftware.org/photon_meta ");
   strcat( buff, src_path);
   strcat( buff, "\" "
      "objName=\"My_name\" >"
      "</folder>" );
   errcode = psoFolderCreateObjectXML( folderHandle,
                                       buff,
                                       strlen(buff) );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   psoExit();

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

