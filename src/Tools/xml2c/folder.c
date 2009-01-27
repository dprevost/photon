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

int doHashMap( xmlNode * hashmapNode, const char * name );
int doQueue(   xmlNode * queueNode,   const char * name );
int validateName( xmlChar * name );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int doFolder( xmlNode * folderNode, const char * name )
{
   int rc;
   xmlNode * node;
   xmlChar * prop;

   rc = mkdir( name, 0755 );
   if ( rc != 0 ) {
      if ( errno != EEXIST ) {
         fprintf( stderr, "Creating directory %s failed\n", name );
         return -1;
      }
   }
   rc = chdir( name );
   if ( rc != 0 ) {
      fprintf( stderr, "cd to directory %s failed\n", name );
      return -1;
   }

   node = folderNode->children;
   
   while ( node != NULL ) {
      if ( node->type == XML_ELEMENT_NODE ) {

         prop = xmlGetProp( node, BAD_CAST "objName" );
         if ( prop == NULL ) {
            fprintf( stderr, "Error getting the name of a node\n" );
            goto cleanup;
         }
         if ( validateName( prop ) != 0 ) {
            fprintf( stderr, "Invalid object name = %s\n", (char *) prop );
            goto cleanup;
         }

         if ( xmlStrcmp( node->name, BAD_CAST "folder") == 0 ) {
            rc = doFolder( node, (char *)prop );
         }
         else if ( xmlStrcmp( node->name, BAD_CAST "hashmap") == 0 ) {
            rc = doHashMap( node, (char *)prop );
         }
         else if ( xmlStrcmp( node->name, BAD_CAST "queue") == 0 ) {
            rc = doQueue( node, (char *)prop );
         }
         else {
            fprintf( stderr, "Error: node type is invalid (%s)\n", (char *)prop );
            rc = -1;
         }
         xmlFree( prop );
         prop = NULL;
         
         if ( rc != 0 ) goto cleanup;
      }
      node = node->next;
   }

cleanup:
   
   rc = chdir( ".." );
   if ( rc != 0 ) {
      fprintf( stderr, "cd to directory \"..\" failed\n" );
      return -1;
   }

   if ( prop ) xmlFree( prop );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
