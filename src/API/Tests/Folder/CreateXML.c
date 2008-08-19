/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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
#include <photon/vds.h>
#include "Tests/PrintError.h"

const bool expectedToPass = true;

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
                              "/afcx",
                              strlen("/afcx"),
                              &def );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsFolderOpen( sessionHandle,
                            "/afcx",
                            strlen("/afcx"),
                            &folderHandle );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Invalid arguments to tested function. */

   strcpy( buff, "<?xml version=\"1.0\"?>\n"
      "<folder xmlns=\"http://vdsf.sourceforge.net/vdsf_md\" "
      "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" "
      "xsi:schemaLocation=\"http://vdsf.sourceforge.net/vdsf_md ");
   strcat( buff, src_path);
   strcat( buff, "\" "
      "objName=\"My_name\" > "
      "</folder2>" );
//fprintf( stderr, "%s\n", buff );

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
   if ( errcode != VDS_INVALID_LENGTH ) { // 
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* The closing tag is </folder2> instead of folder. */
   errcode = vdsFolderCreateObjectXML( folderHandle,
                                       buff,
                                       strlen(buff) );
   if ( errcode != VDS_XML_READ_ERROR ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   strcpy( buff, "<?xml version=\"1.0\"?>\n"
      "<folder xmlns=\"http://vdsf.sourceforge.net/vdsf_md\" "
      "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" "
      "xsi:schemaLocation=\"http://vdsf.sourceforge.net/vdsf_md ");
   strcat( buff, src_path);
   strcat( buff, "\" "
      "objName=\"My_name\" >"
      "</folder>" );
   errcode = vdsFolderCreateObjectXML( folderHandle,
                                       buff,
                                       strlen(buff) );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   vdsExit();

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

