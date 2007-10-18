/*
 * Copyright (C) 2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

#include "folderTest.h"
#include "Engine/Folder.c"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int errcode, i;
   char name [VDS_MAX_NAME_LENGTH+10];
   size_t partial;
   bool last;
   
   errcode = vdseValidateString( strCheck("Test2"),
                                 5,
                                 &partial,
                                 &last );
   if ( errcode != 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( partial != 5 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   if (! last) 
      ERROR_EXIT( expectedToPass, NULL, ; );

   errcode = vdseValidateString( strCheck("Test2/Test555"),
                                 13,
                                 &partial,
                                 &last );
   if ( errcode != 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( partial != 5 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   if (last) 
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   /* Must be null-terminated for mbsrtowcs */
   for ( i = 0; i < VDS_MAX_NAME_LENGTH+9; ++i )
      name[i] = 't';
   name[VDS_MAX_NAME_LENGTH+9] = 0;
   errcode = vdseValidateString( strCheck(name),
                                 VDS_MAX_NAME_LENGTH+9, /* not 10 ! */
                                 &partial,
                                 &last );
   if ( errcode != VDS_OBJECT_NAME_TOO_LONG )
      ERROR_EXIT( expectedToPass, NULL, ; );

   errcode = vdseValidateString( strCheck("Test2/"),
                                 6,
                                 &partial,
                                 &last );
   if ( errcode != 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( partial != 5 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   if (! last) 
      ERROR_EXIT( expectedToPass, NULL, ; );

   name[10] = 0;
   name[4] = '\t';
   errcode = vdseValidateString( strCheck(name),
                                 10,
                                 &partial,
                                 &last );
   if ( errcode != VDS_INVALID_OBJECT_NAME )
      ERROR_EXIT( expectedToPass, NULL, ; );

   name[4] = '=';
   errcode = vdseValidateString( strCheck(name),
                                 10,
                                 &partial,
                                 &last );
   if ( errcode != VDS_INVALID_OBJECT_NAME )
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   name[4] = ' ';
   errcode = vdseValidateString( strCheck(name),
                                 10,
                                 &partial,
                                 &last );
   if ( errcode != 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
