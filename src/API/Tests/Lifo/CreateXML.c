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
#include <vdsf/vds.h>
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
      "<lifo xmlns=\"http://vdsf.sourceforge.net/vdsf_md\" \n"     \
      "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" \n"  \
      "xsi:schemaLocation=\"http://vdsf.sourceforge.net/vdsf_md "); \
   strcat( buff, SRC_PATH);                                         \
   strcat( buff, "\"\n objName=\"My_Queue\" >\n" );                 \
   strcat( buff, TRAILER );                                         \
   strcat( buff, "\n</lifo>" );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   VDS_HANDLE sessionHandle, folderHandle;
   int errcode;
   char buff[1000];
   char src_path[PATH_MAX] = {"../../../XML/vdsf_md10.xsd"};
   vdsObjectDefinition def = { 
      VDS_FOLDER, 
      0, 
      { 0, 0, 0, 0}, 
      { { "", 0, 0, 0, 0, 0, 0} } 
   };
   
   if ( argc > 1 ) {
      errcode = vdsInit( argv[1], 0 );
   }
   else {
      errcode = vdsInit( "10701", 0 );
   }
   if ( argc > 2 ) {
      strcpy( src_path, argv[2] );
      strcat( src_path, "/src/XML/vdsf_md10.xsd" );
   }
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdsInitSession( &sessionHandle );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsCreateObject( sessionHandle,
                              "/api_lifo_cx",
                              strlen("/api_lifo_cx"),
                              &def );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsFolderOpen( sessionHandle,
                            "/api_lifo_cx",
                            strlen("/api_lifo_cx"),
                            &folderHandle );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Invalid arguments to tested function. */

   BUILD_XML(src_path, 
      "<field name=\"junk1\"><boolean /></field>\n"
      "<field name=\"junk3\"><string length=\"100\"/></field>\n"
      "<lastField name=\"junk2\"><integer size=\"4\" /></lastField>" );

   errcode = vdsFolderCreateObjectXML( folderHandle,
                                       NULL,
                                       strlen(buff) );
   if ( errcode != VDS_NULL_POINTER ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsFolderCreateObjectXML( folderHandle,
                                       buff,
                                       0 );
   if ( errcode != VDS_INVALID_LENGTH ) { 
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Empty definition, obviously. */
   BUILD_XML(src_path, "" );
   errcode = vdsFolderCreateObjectXML( folderHandle,
                                       buff,
                                       strlen(buff) );
   if ( errcode != VDS_XML_VALIDATION_FAILED ) { 
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* lastfield not the last one */
   BUILD_XML(src_path, 
      "<field name=\"junk1\"><boolean /></field>\n"
      "<lastField name=\"junk2\"><integer size=\"4\" /></lastField>\n"
      "<field name=\"junk3\"><string length=\"100\"/></field>" );
   errcode = vdsFolderCreateObjectXML( folderHandle,
                                       buff,
                                       strlen(buff) );
   if ( errcode != VDS_XML_VALIDATION_FAILED ) { 
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* same names */
   BUILD_XML(src_path, 
      "<field name=\"junk1\"><boolean /></field>\n"
      "<field name=\"junk2\"><string length=\"100\"/></field>\n"
      "<lastField name=\"junk2\"><integer size=\"4\" /></lastField>" );
   errcode = vdsFolderCreateObjectXML( folderHandle,
                                       buff,
                                       strlen(buff) );
   if ( errcode != VDS_XML_VALIDATION_FAILED ) { 
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* var string not in lastField */
   BUILD_XML(src_path, 
      "<field name=\"junk1\"><varString minLength=\"0\" maxLength=\"100\" /></field>\n"
      "<field name=\"junk2\"><string length=\"100\"/></field>\n"
      "<lastField name=\"junk2\"><integer size=\"4\" /></lastField>" );
   errcode = vdsFolderCreateObjectXML( folderHandle,
                                       buff,
                                       strlen(buff) );
   if ( errcode != VDS_XML_VALIDATION_FAILED ) { 
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* decimal with missing attributes */
   BUILD_XML(src_path, 
      "<field name=\"junk1\"><decimal /></field>\n"
      "<field name=\"junk2\"><string length=\"100\"/></field>\n"
      "<lastField name=\"junk3\"><integer size=\"4\" /></lastField>" );
   errcode = vdsFolderCreateObjectXML( folderHandle,
                                       buff,
                                       strlen(buff) );
   if ( errcode != VDS_XML_VALIDATION_FAILED ) { 
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* decimal with precision > max permitted */
   BUILD_XML(src_path, 
      "<field name=\"junk1\"><decimal precision=\"100\" scale=\"0\" /></field>\n"
      "<field name=\"junk2\"><string length=\"100\"/></field>\n"
      "<lastField name=\"junk3\"><integer size=\"4\" /></lastField>" );
   errcode = vdsFolderCreateObjectXML( folderHandle,
                                       buff,
                                       strlen(buff) );
   if ( errcode != VDS_XML_VALIDATION_FAILED ) { 
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* decimal with scale > precision */
   BUILD_XML(src_path, 
      "<field name=\"junk1\"><decimal precision=\"10\" scale=\"11\" /></field>\n"
      "<field name=\"junk2\"><string length=\"100\"/></field>\n"
      "<lastField name=\"junk3\"><integer size=\"4\" /></lastField>" );
   errcode = vdsFolderCreateObjectXML( folderHandle,
                                       buff,
                                       strlen(buff) );
   if ( errcode != VDS_INVALID_SCALE ) { 
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* integer with invalid size */
   BUILD_XML(src_path, 
      "<field name=\"junk1\"><integer size=\"11\" /></field>\n"
      "<field name=\"junk2\"><string length=\"100\"/></field>\n"
      "<lastField name=\"junk3\"><integer size=\"4\" /></lastField>" );
   errcode = vdsFolderCreateObjectXML( folderHandle,
                                       buff,
                                       strlen(buff) );
   if ( errcode != VDS_XML_VALIDATION_FAILED ) { 
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* integer with invalid attribute name */
   BUILD_XML(src_path, 
      "<field name=\"junk1\"><integer length=\"4\" /></field>\n"
      "<field name=\"junk2\"><string length=\"100\"/></field>\n"
      "<lastField name=\"junk3\"><integer size=\"4\" /></lastField>" );
   errcode = vdsFolderCreateObjectXML( folderHandle,
                                       buff,
                                       strlen(buff) );
   if ( errcode != VDS_XML_VALIDATION_FAILED ) { 
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   BUILD_XML(src_path, 
      "<field name=\"junk1\"><integer size=\"4\" /></field>\n"
      "<field name=\"junk2\"><string length=\"100\"/></field>\n"
      "<field name=\"junk3\"><decimal precision=\"10\" scale=\"2\" /></field>\n"
      "<field name=\"junk4\"><integer size=\"1\" /></field>\n"
      "<field name=\"junk5\"><boolean /></field>\n"
      "<field name=\"junk6\"><binary length=\"200\" /></field>\n"
      "<lastField name=\"junk7\"><varBinary minLength=\"10\" maxLength=\"0\" /></lastField>" );
   errcode = vdsFolderCreateObjectXML( folderHandle,
                                       buff,
                                       strlen(buff) );
   if ( errcode != VDS_OK ) { 
      fprintf(stderr, "buff = \n%s\n", buff );
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   vdsExit();

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
