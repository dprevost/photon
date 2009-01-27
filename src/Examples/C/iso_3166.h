/*
 * Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
 * 
 * This code is in the public domain.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
 
#ifndef ISO_3166_H
#define ISO_3166_H

#if defined(WIN32)
#  include <Windows.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <photon/photon.h>

FILE * fp = NULL;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int openData( const char* filename )
{   
   fp = fopen( filename, "r" );
   if ( fp == NULL ) {
      fprintf(stderr, "Error opening the file %s, error = %s\n", 
         filename, strerror(errno) );
      return -1;
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int readData( char *countryCode, char* description )
{
   char line[80] = "";
   int len;
   
   if ( fgets( line, 80, fp ) == NULL ) {
      if ( ferror(fp) ) {
         fprintf( stderr, "Error reading the file, error = %s\n", 
            strerror(errno) );
         return -1;
      }
      return 0;
   }

   len = strlen(line);
   if ( line[len-1] == '\n' ) { line[len-1] = 0; }
   
   countryCode[0] = line[0];
   countryCode[1] = line[1];
   strcpy( description, &line[3] );

   return 1;
}
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* ISO_3166_H */

