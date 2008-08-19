/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#include "Nucleus/MemBitmap.h"
#include "Nucleus/Tests/EngineTestCommon.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   size_t calculatedSize;
   
   calculatedSize = psnGetBitmapLengthBytes( 1024, 8 );
   if ( calculatedSize != 1024/8/8 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   calculatedSize = psnGetBitmapLengthBytes( 1023, 8 );
   if ( calculatedSize != 1024/8/8 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   calculatedSize = psnGetBitmapLengthBytes( 1025, 8 );
   if ( calculatedSize != 1024/8/8 + 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   calculatedSize = psnGetBitmapLengthBytes( 1016, 8 );
   if ( calculatedSize != 1024/8/8 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   calculatedSize = psnGetBitmapLengthBytes( 1024+63, 8 );
   if ( calculatedSize != 1024/8/8 + 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   calculatedSize = psnGetBitmapLengthBytes( 1024+64, 8 );
   if ( calculatedSize != 1024/8/8 + 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   calculatedSize = psnGetBitmapLengthBytes( 1024+65, 8 );
   if ( calculatedSize != 1024/8/8 + 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
