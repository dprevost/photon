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
   uint32_t partial;
   bool last;
   
   errcode = psonValidateString( "Test2",
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
   
   errcode = psonValidateString( "Test2/Test555",
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

   errcode = psonValidateString( name,
                                 PSO_MAX_NAME_LENGTH+9, /* not 10 ! */
                                 &partial,
                                 &last );
   if ( errcode != PSO_OBJECT_NAME_TOO_LONG ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = psonValidateString( "Test2/",
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
   errcode = psonValidateString( name,
                                 10,
                                 &partial,
                                 &last );
   if ( errcode != PSO_INVALID_OBJECT_NAME ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   name[4] = '=';
   errcode = psonValidateString( name,
                                 10,
                                 &partial,
                                 &last );
   if ( errcode != PSO_INVALID_OBJECT_NAME ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   name[0] = '3';
   name[4] = 't';
   errcode = psonValidateString( name,
                                 10,
                                 &partial,
                                 &last );
   if ( errcode != PSO_INVALID_OBJECT_NAME ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   name[0] = '_';
   errcode = psonValidateString( name,
                                 10,
                                 &partial,
                                 &last );
   if ( errcode != PSO_INVALID_OBJECT_NAME ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   name[0] = 't';
   name[4] = '_';
   errcode = psonValidateString( name,
                                 10,
                                 &partial,
                                 &last );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   name[4] = '3';
   errcode = psonValidateString( name,
                                 10,
                                 &partial,
                                 &last );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
