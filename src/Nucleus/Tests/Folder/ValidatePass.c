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

#include "folderTest.h"
#if defined WIN32
#  pragma warning(disable:4273)
#endif
#include "Nucleus/Folder.c"
#if defined WIN32
#  pragma warning(default:4273)
#endif

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int errcode, i;
   char name [PSO_MAX_NAME_LENGTH+10];
   size_t partial;
   bool last;
   
   errcode = psnValidateString( "Test2",
                                 5,
                                 &partial,
                                 &last );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( partial != 5 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if (! last) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = psnValidateString( "Test2/Test555",
                                 13,
                                 &partial,
                                 &last );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( partial != 5 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if (last) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   for ( i = 0; i < PSO_MAX_NAME_LENGTH+9; ++i ) {
      name[i] = 't';
   }
   name[PSO_MAX_NAME_LENGTH+9] = 0;

   errcode = psnValidateString( name,
                                 PSO_MAX_NAME_LENGTH+9, /* not 10 ! */
                                 &partial,
                                 &last );
   if ( errcode != PSO_OBJECT_NAME_TOO_LONG ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = psnValidateString( "Test2/",
                                 6,
                                 &partial,
                                 &last );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( partial != 5 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if (! last) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   name[10] = 0;
   name[4] = '\t';
   errcode = psnValidateString( name,
                                 10,
                                 &partial,
                                 &last );
   if ( errcode != PSO_INVALID_OBJECT_NAME ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   name[4] = '=';
   errcode = psnValidateString( name,
                                 10,
                                 &partial,
                                 &last );
   if ( errcode != PSO_INVALID_OBJECT_NAME ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   name[0] = '3';
   name[4] = 't';
   errcode = psnValidateString( name,
                                 10,
                                 &partial,
                                 &last );
   if ( errcode != PSO_INVALID_OBJECT_NAME ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   name[0] = '_';
   errcode = psnValidateString( name,
                                 10,
                                 &partial,
                                 &last );
   if ( errcode != PSO_INVALID_OBJECT_NAME ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   name[0] = 't';
   name[4] = '_';
   errcode = psnValidateString( name,
                                 10,
                                 &partial,
                                 &last );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   name[4] = '3';
   errcode = psnValidateString( name,
                                 10,
                                 &partial,
                                 &last );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
