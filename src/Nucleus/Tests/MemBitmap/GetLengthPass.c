/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@photonsoftware.org>
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

#include "Nucleus/MemBitmap.h"
#include "Nucleus/Tests/EngineTestCommon.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   size_t calculatedSize;
   
   calculatedSize = psonGetBitmapLengthBytes( 1024, 8 );
   if ( calculatedSize != 1024/8/8 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   calculatedSize = psonGetBitmapLengthBytes( 1023, 8 );
   if ( calculatedSize != 1024/8/8 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   calculatedSize = psonGetBitmapLengthBytes( 1025, 8 );
   if ( calculatedSize != 1024/8/8 + 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   calculatedSize = psonGetBitmapLengthBytes( 1016, 8 );
   if ( calculatedSize != 1024/8/8 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   calculatedSize = psonGetBitmapLengthBytes( 1024+63, 8 );
   if ( calculatedSize != 1024/8/8 + 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   calculatedSize = psonGetBitmapLengthBytes( 1024+64, 8 );
   if ( calculatedSize != 1024/8/8 + 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   calculatedSize = psonGetBitmapLengthBytes( 1024+65, 8 );
   if ( calculatedSize != 1024/8/8 + 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
