/*
 * Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
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
#include <photon/photon.h>
#include "Tests/PrintError.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Macro to simplify the input of test xml buffers
 * Note: the '\n' in the text are for debugging (printf of buff), to
 *       make the output easier to read.
 */
#define BUILD_XML(SRC_PATH,TRAILER)                                 \
   strcpy( buff, "<?xml version=\"1.0\"?>\n"                        \
      "<hashmap xmlns=\"http://photonsoftware.org/photon_meta\" \n"   \
      "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" \n"  \
      "xsi:schemaLocation=\"http://photonsoftware.org/photon_meta "); \
   strcat( buff, SRC_PATH);                                         \
   strcat( buff, "\"\n objName=\"My_Map\" mode=\"dynamic\">\n" );                   \
   strcat( buff, TRAILER );                                         \
   strcat( buff, "\n</hashmap>" );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   PSO_HANDLE sessionHandle, folderHandle;
   int errcode;
   char buff[1000];
   char src_path[PATH_MAX] = {"../../../XML/photon_meta10.xsd"};
   psoObjectDefinition def = { PSO_FOLDER, 0 };
   
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
                              "/ahmcx",
                              strlen("/ahmcx"),
                              &def,
                              NULL,
                              0,
                              NULL,
                              0 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFolderOpen( sessionHandle,
                            "/ahmcx",
                            strlen("/ahmcx"),
                            &folderHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Invalid arguments to tested function. */

   BUILD_XML(src_path, 
      "<key><integer size=\"4\" /></key>\n"
      "<field name=\"junk1\"><boolean /></field>\n"
      "<field name=\"junk3\"><string length=\"100\"/></field>\n"
      "<lastField name=\"junk2\"><integer size=\"4\" /></lastField>" );

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
   if ( errcode != PSO_INVALID_LENGTH ) { 
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Empty definition, obviously. */
   BUILD_XML(src_path, "" );
   errcode = psoFolderCreateObjectXML( folderHandle,
                                       buff,
                                       strlen(buff) );
   if ( errcode != PSO_XML_VALIDATION_FAILED ) { 
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* lastfield not the last one */
   BUILD_XML(src_path, 
      "<key><integer size=\"4\" /></key>\n"
      "<field name=\"junk1\"><boolean /></field>\n"
      "<lastField name=\"junk2\"><integer size=\"4\" /></lastField>\n"
      "<field name=\"junk3\"><string length=\"100\"/></field>" );
   errcode = psoFolderCreateObjectXML( folderHandle,
                                       buff,
                                       strlen(buff) );
   if ( errcode != PSO_XML_VALIDATION_FAILED ) { 
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* same names */
   BUILD_XML(src_path, 
      "<key><integer size=\"4\" /></key>\n"
      "<field name=\"junk1\"><boolean /></field>\n"
      "<field name=\"junk2\"><string length=\"100\"/></field>\n"
      "<lastField name=\"junk2\"><integer size=\"4\" /></lastField>" );
   errcode = psoFolderCreateObjectXML( folderHandle,
                                       buff,
                                       strlen(buff) );
   if ( errcode != PSO_XML_VALIDATION_FAILED ) { 
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* var string not in lastField */
   BUILD_XML(src_path, 
      "<key><integer size=\"4\" /></key>\n"
      "<field name=\"junk1\"><varString minLength=\"0\" maxLength=\"100\" /></field>\n"
      "<field name=\"junk2\"><string length=\"100\"/></field>\n"
      "<lastField name=\"junk2\"><integer size=\"4\" /></lastField>" );
   errcode = psoFolderCreateObjectXML( folderHandle,
                                       buff,
                                       strlen(buff) );
   if ( errcode != PSO_XML_VALIDATION_FAILED ) { 
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* decimal with missing attributes */
   BUILD_XML(src_path, 
      "<key><integer size=\"4\" /></key>\n"
      "<field name=\"junk1\"><decimal /></field>\n"
      "<field name=\"junk2\"><string length=\"100\"/></field>\n"
      "<lastField name=\"junk3\"><integer size=\"4\" /></lastField>" );
   errcode = psoFolderCreateObjectXML( folderHandle,
                                       buff,
                                       strlen(buff) );
   if ( errcode != PSO_XML_VALIDATION_FAILED ) { 
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* decimal with precision > max permitted */
   BUILD_XML(src_path, 
      "<key><integer size=\"4\" /></key>\n"
      "<field name=\"junk1\"><decimal precision=\"100\" scale=\"0\" /></field>\n"
      "<field name=\"junk2\"><string length=\"100\"/></field>\n"
      "<lastField name=\"junk3\"><integer size=\"4\" /></lastField>" );
   errcode = psoFolderCreateObjectXML( folderHandle,
                                       buff,
                                       strlen(buff) );
   if ( errcode != PSO_XML_VALIDATION_FAILED ) { 
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* decimal with scale > precision */
   BUILD_XML(src_path, 
      "<key><integer size=\"4\" /></key>\n"
      "<field name=\"junk1\"><decimal precision=\"10\" scale=\"11\" /></field>\n"
      "<field name=\"junk2\"><string length=\"100\"/></field>\n"
      "<lastField name=\"junk3\"><integer size=\"4\" /></lastField>" );
   errcode = psoFolderCreateObjectXML( folderHandle,
                                       buff,
                                       strlen(buff) );
   if ( errcode != PSO_INVALID_SCALE ) { 
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* integer with invalid size */
   BUILD_XML(src_path, 
      "<key><integer size=\"4\" /></key>\n"
      "<field name=\"junk1\"><integer size=\"11\" /></field>\n"
      "<field name=\"junk2\"><string length=\"100\"/></field>\n"
      "<lastField name=\"junk3\"><integer size=\"4\" /></lastField>" );
   errcode = psoFolderCreateObjectXML( folderHandle,
                                       buff,
                                       strlen(buff) );
   if ( errcode != PSO_XML_VALIDATION_FAILED ) { 
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* integer with invalid attribute name */
   BUILD_XML(src_path, 
      "<key><integer size=\"4\" /></key>\n"
      "<field name=\"junk1\"><integer length=\"4\" /></field>\n"
      "<field name=\"junk2\"><string length=\"100\"/></field>\n"
      "<lastField name=\"junk3\"><integer size=\"4\" /></lastField>" );
   errcode = psoFolderCreateObjectXML( folderHandle,
                                       buff,
                                       strlen(buff) );
   if ( errcode != PSO_XML_VALIDATION_FAILED ) { 
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* key with a boolean */
   BUILD_XML(src_path, 
      "<key><boolean /></key>\n"
      "<field name=\"junk1\"><integer size=\"4\" /></field>\n"
      "<field name=\"junk2\"><string length=\"100\"/></field>\n"
      "<lastField name=\"junk3\"><integer size=\"4\" /></lastField>" );
   errcode = psoFolderCreateObjectXML( folderHandle,
                                       buff,
                                       strlen(buff) );
   if ( errcode != PSO_XML_VALIDATION_FAILED ) { 
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* key with a decimal */
   BUILD_XML(src_path, 
      "<key><decimal precision=\"10\" scale=\"2\" /></key>\n"
      "<field name=\"junk1\"><integer size=\"4\" /></field>\n"
      "<field name=\"junk2\"><string length=\"100\"/></field>\n"
      "<lastField name=\"junk3\"><integer size=\"4\" /></lastField>" );
   errcode = psoFolderCreateObjectXML( folderHandle,
                                       buff,
                                       strlen(buff) );
   if ( errcode != PSO_XML_VALIDATION_FAILED ) { 
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* 2 keys */
   BUILD_XML(src_path, 
      "<key><integer size=\"8\" /></key>\n"
      "<key><integer size=\"4\" /></key>\n"
      "<field name=\"junk1\"><integer size=\"4\" /></field>\n"
      "<field name=\"junk2\"><string length=\"100\"/></field>\n"
      "<lastField name=\"junk3\"><integer size=\"4\" /></lastField>" );
   errcode = psoFolderCreateObjectXML( folderHandle,
                                       buff,
                                       strlen(buff) );
   if ( errcode != PSO_XML_VALIDATION_FAILED ) { 
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* key out-of-order */
   BUILD_XML(src_path, 
      "<field name=\"junk1\"><integer size=\"4\" /></field>\n"
      "<key><integer size=\"4\" /></key>\n"
      "<field name=\"junk2\"><string length=\"100\"/></field>\n"
      "<lastField name=\"junk3\"><integer size=\"4\" /></lastField>" );
   errcode = psoFolderCreateObjectXML( folderHandle,
                                       buff,
                                       strlen(buff) );
   if ( errcode != PSO_XML_VALIDATION_FAILED ) { 
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   BUILD_XML(src_path, 
      "<key><integer size=\"4\" /></key>\n"
      "<field name=\"junk1\"><integer size=\"4\" /></field>\n"
      "<field name=\"junk2\"><string length=\"100\"/></field>\n"
      "<field name=\"junk3\"><decimal precision=\"10\" scale=\"2\" /></field>\n"
      "<field name=\"junk4\"><integer size=\"1\" /></field>\n"
      "<field name=\"junk5\"><boolean /></field>\n"
      "<field name=\"junk6\"><binary length=\"200\" /></field>\n"
      "<lastField name=\"junk7\"><varBinary minLength=\"10\" maxLength=\"0\" /></lastField>" );
   errcode = psoFolderCreateObjectXML( folderHandle,
                                       buff,
                                       strlen(buff) );
   if ( errcode != PSO_OK ) { 
      fprintf(stderr, "buff = \n%s\n", buff );
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   psoExit();

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

