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

#include <libxml/xmlschemas.h>
#include <libxml/xmlschemastypes.h>

void writeHeader( FILE * fp, const char * name ) ;
void writeTrailer( FILE * fp, const char * name ) ;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int doQueue( xmlNode * queueNode, const char * name )
{
   FILE* fp = NULL;
   char * str;
   
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
   
   writeTrailer( fp, name );
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

