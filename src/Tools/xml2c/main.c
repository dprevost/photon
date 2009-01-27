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
#include "Common/Options.h"

#include <libxml/xmlschemas.h>
#include <libxml/xmlschemastypes.h>

int validateName( xmlChar * prop );
int doFolder(  xmlNode * folderNode,  const char * name );
int doHashMap( xmlNode * hashmapNode, const char * name );
int doQueue(   xmlNode * queueNode,   const char * name );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static void dummyErrorFunc( void * ctx, const char * msg, ...)
{
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   char * inputname = NULL;
   char * dirname = NULL, * dummy = NULL;
   int fd = -1, errcode = 0, rc = 0, debug = 0;
   size_t length, i, j;
   int separator = -1;
   psocOptionHandle optHandle;
   char * buff = NULL;
   bool ok;
   
#if HAVE_STAT || HAVE__STAT
   struct stat status;

   struct psocOptStruct opts[2] = {
      { 'i', "input",    0, "input_filename", "Filename for the input (XML)" },
      { 'o', "output",   0, "output_dirname", "Directory name for the output files" }
   };

#else
   struct psocOptStruct opts[3] = {
      { 'i', "input",    0, "input_filename", "Filename for the input (XML)" },
      { 'l', "length",   0, "input length",   "Length of input if stat() not supported" },
      { 'o', "output",   0, "output_dirname", "Directory name for the output files" }
   };

#endif

   xmlSchemaPtr schema = NULL;
   xmlSchemaValidCtxtPtr  validCtxt = NULL;
   xmlSchemaParserCtxtPtr parserCtxt = NULL;
   xmlNode * root = NULL;
   xmlDoc  * doc = NULL;
   xmlChar * prop = NULL;
   
#if HAVE_STAT || HAVE__STAT
   ok = psocSetSupportedOptions( 2, opts, &optHandle );
#else
   ok = psocSetSupportedOptions( 3, opts, &optHandle );
#endif
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) {
      fprintf( stderr, "Internal error in psocSetSupportedOptions\n" );
      return 1;
   }
   
   errcode = psocValidateUserOptions( optHandle, argc, argv, 1 );
   if ( errcode < 0 ) {
      psocShowUsage( optHandle, argv[0], "" );
      return 1;
   }
   
   if ( errcode > 0 ) {
      psocShowUsage( optHandle, argv[0], "" );
      return 0;
   }

   psocGetShortOptArgument( optHandle, 'i', &inputname );
   psocGetShortOptArgument( optHandle, 'o', &dirname );

#if HAVE_STAT || HAVE__STAT
   errcode = stat( inputname, &status );
   if ( errcode != 0) {
      fprintf( stderr, "Cannot access the size of the input file\n" );
      return 1;
   }
   length = status.st_size;
#else   
   psocGetShortOptArgument( optHandle, 'o', &dummy );
   sscanf( dummy, PSO_SIZE_T_FORMAT, &length );
#endif

   fd = open( inputname, O_RDONLY );
   if ( fd == -1 ) {
      fprintf( stderr, "Cannot open the input file\n" );
      return 1;
   }

   buff = (char *)malloc(length + 1 );
   if ( buff == NULL ) {
      fprintf( stderr, "Memory allocation error\n" );
      goto cleanup;
   }
   
   i = read( fd, buff, length );
   if ( i != length ) {
      fprintf( stderr, "Cannot read the input file\n" );
      goto cleanup;
   }
   buff[length] = 0;
   
   if ( debug ) {
      doc = xmlReadMemory( buff, i, NULL, NULL, 0 );
   }
   else {
      doc = xmlReadMemory( buff, i, NULL, NULL, XML_PARSE_NOERROR | XML_PARSE_NOWARNING );
   }
   if ( doc == NULL ) {
      fprintf( stderr, "Error reading xml in memory\n" );
      errcode = -1;
      goto cleanup;
   }
   
   root = xmlDocGetRootElement( doc );
   if ( root == NULL ) {
      fprintf( stderr, "Error: no root\n" );
      goto cleanup;
   }

//   if ( xmlStrcmp( root->name, BAD_CAST "quasar_config") != 0 ) {
//      errcode = PSOW_XML_INVALID_ROOT;
//      goto cleanup;
//   }
   
   prop = xmlGetProp( root, BAD_CAST "schemaLocation" );
   if ( prop == NULL ) {
      fprintf( stderr, "Error: no schemaLocation property (of root)\n" );
      goto cleanup;
   }
   
   for ( i = 0; i < xmlStrlen(prop)-1; ++i ) {
      if ( isspace(prop[i]) ) {
         for ( j = i+1; j < xmlStrlen(prop)-1; ++j ) {
            if ( isspace(prop[j]) == 0 ) {
               separator = j;
               break;
            }
         }
         break;
      }
   }
   if ( separator == -1 ) {
      fprintf( stderr, "Error: invalid schemaLocation property (of root)\n" );
      goto cleanup;
   }
   
   parserCtxt = xmlSchemaNewParserCtxt( (char*)&prop[separator] );
   if ( parserCtxt == NULL ) {
      fprintf( stderr, "Error: creating new parser context failed\n" );
      goto cleanup;
   }
   
   schema = xmlSchemaParse( parserCtxt );
   if ( schema == NULL ) {
      fprintf( stderr, "Error: parsing the schema failed\n" );
      goto cleanup;
   }
   
   xmlFree( prop );
   prop = NULL;

   validCtxt = xmlSchemaNewValidCtxt( schema );
   if ( validCtxt == NULL ) {
      fprintf( stderr, "Error: creating new validation context failed\n" );
      goto cleanup;
   }
   
   if ( debug ) {
      xmlSchemaSetValidErrors( validCtxt,
                               (xmlSchemaValidityErrorFunc) fprintf,
                               (xmlSchemaValidityWarningFunc) fprintf,
                               stderr );
   }
   else {
      xmlSchemaSetValidErrors( validCtxt,
                               (xmlSchemaValidityErrorFunc) dummyErrorFunc,
                               (xmlSchemaValidityWarningFunc) dummyErrorFunc,
                               stderr );
   }
   
   if ( xmlSchemaValidateDoc( validCtxt, doc ) != 0 ) {
      fprintf( stderr, "Error: document validation failed\n" );
      goto cleanup;
   }

   if ( xmlStrcmp( root->name, BAD_CAST "photon" ) == 0 ) {
      /* This is a topFolder and has no name */
      errcode = doFolder( root, dirname );
   }
   else {
      prop = xmlGetProp( root, BAD_CAST "objName" );
      if ( prop == NULL ) {
         fprintf( stderr, "Error getting the name of the root\n" );
         goto cleanup;
      }
      if ( validateName( prop ) != 0 ) {
         fprintf( stderr, "Invalid object name = %s\n", (char *) prop );
         goto cleanup;
      }

      rc = mkdir( dirname, 0755 );
      if ( rc != 0 ) {
         if ( errno != EEXIST ) {
            fprintf( stderr, "Creating directory %s failed\n", dirname );
            goto cleanup;
         }
      }
      rc = chdir( dirname );
      if ( rc != 0 ) {
         fprintf( stderr, "cd to directory %s failed\n", dirname );
         goto cleanup;
      }
      
      if ( xmlStrcmp( root->name, BAD_CAST "folder" ) == 0 ) {
         errcode = doFolder( root, (char *)prop );
      }
      else if ( xmlStrcmp( root->name, BAD_CAST "hashmap") == 0 ) {
         rc = doHashMap( root, (char *)prop );
      }
      else if ( xmlStrcmp( root->name, BAD_CAST "queue") == 0 ) {
         rc = doQueue( root, (char *)prop );
      }
      else {
         fprintf( stderr, "Error: root type is invalid\n" );
         errcode = -1;
      }

      rc = chdir( ".." );
      if ( rc != 0 ) {
         fprintf( stderr, "cd to directory \"..\" failed\n" );
      }
   }

cleanup:
   
   if ( buff != NULL ) free( buff );
   if ( fd != -1 ) close(fd);
   if ( parserCtxt ) xmlSchemaFreeParserCtxt( parserCtxt );
   if ( schema ) xmlSchemaFree( schema );
   if ( validCtxt ) xmlSchemaFreeValidCtxt( validCtxt );
   if ( prop ) xmlFree( prop );
   if ( doc ) xmlFreeDoc( doc );

   /* In case this program is include in a script */
   if ( errcode != 0  || rc != 0 ) return 1;

   return 0;   
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
