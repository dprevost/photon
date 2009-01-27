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

#include <libxml/xmlschemas.h>
#include <libxml/xmlschemastypes.h>

void writeHeader( FILE * fp, const char * name ) ;
void writeTrailer( FILE * fp, const char * name ) ;
int doDefinition( FILE * fp, xmlNode * node );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int doHashMap( xmlNode * queueNode, const char * name )
{
   FILE* fp = NULL;
   char * str;
   xmlNode * node;
   int rc;
   
   str = calloc( strlen(name) + strlen(".h") + 1, 1 );
   if ( str == NULL ) {
      fprintf( stderr, "Error allocing memory\n" );
      return -1;
   }
   strcpy( str, name );
   strcat( str, ".h" );
   
   fp = fopen( str, "w" );
   if ( fp == NULL ) {
      fprintf( stderr, "Error opening file %s\n", str );
      free( str );
      return -1;
   }
   free( str );
   
   writeHeader( fp, name );
   
   fprintf( fp, "struct %s {\n", name );
   
   node = queueNode->children;

   /* Jump past the key */
   while ( node != NULL ) {
      if ( node->type == XML_ELEMENT_NODE ) {
         node = node->next;
         break;
      }
      node = node->next;
   }
   
   rc = doDefinition( fp, node );
   
   if ( rc == 0 ) {
      fprintf( fp, "};\n\n" );
      writeTrailer( fp, name );
   }
   
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int doQueue( xmlNode * queueNode, const char * name )
{
   FILE* fp = NULL;
   char * str;
   xmlNode * node;
   int rc;
   
   str = calloc( strlen(name) + strlen(".h") + 1, 1 );
   if ( str == NULL ) {
      fprintf( stderr, "Error allocing memory\n" );
      return -1;
   }
   strcpy( str, name );
   strcat( str, ".h" );
   
   fp = fopen( str, "w" );
   if ( fp == NULL ) {
      fprintf( stderr, "Error opening file %s\n", str );
      free( str );
      return -1;
   }
   free( str );
   
   writeHeader( fp, name );
   
   fprintf( fp, "struct %s {\n", name );
   
   node = queueNode->children;
   
   rc = doDefinition( fp, node );
   
   if ( rc == 0 ) {
      fprintf( fp, "};\n\n" );
      writeTrailer( fp, name );
   }
   
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

