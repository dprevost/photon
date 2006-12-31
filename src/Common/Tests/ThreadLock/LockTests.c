/*
 * Copyright (C) 2006 Daniel Prevost <dprevost@users.sourceforge.net>
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

#include "Common.h"
#include "ThreadLock.h"
#include "PrintError.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int errcode;
   vdscThreadLock lock;
   
   errcode = vdscInitThreadLock( &lock );
   if ( errcode != 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   errcode = vdscTryAcquireThreadLock( &lock, 0 );
   if ( errcode != 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );

   /* 
    * On Windows, Critical Sections are recursives (for the calling thread,
    * obviously...) while the simplest Posix locks are not.
    *
    * Quite frankly, this test seems useless but it does not hurt so...
    */
   errcode = vdscTryAcquireThreadLock( &lock, 1000 );
#if defined (WIN32)
   if ( errcode != 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   vdscReleaseThreadLock( &lock );
#else
   if ( errcode == 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );
#endif

   vdscReleaseThreadLock( &lock );

   errcode = vdscTryAcquireThreadLock( &lock, 1000 );
   if ( errcode != 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );

   vdscReleaseThreadLock( &lock );

   vdscFiniThreadLock( &lock );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

