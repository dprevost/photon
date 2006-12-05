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

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#include "Common.h"
#include "ThreadLock.h"
#include "PrintError.h"

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int main( int argc, char* argv[] )
{
   int errcode;
   vdscThreadLock lock;
   pid_t pid = getpid();
   
   errcode = vdscInitThreadLock( &lock );
   if ( errcode != 0 )
      ERROR_EXIT( 1, NULL, );
   
   errcode = vdscTryAcquireThreadLock( &lock, 0 );
   if ( errcode != 0 )
      ERROR_EXIT( 1, NULL, );

   errcode = vdscTryAcquireThreadLock( &lock, 1000 );
   if ( errcode == 0 )
      ERROR_EXIT( 1, NULL, );
      
   vdscReleaseThreadLock( &lock );

   errcode = vdscTryAcquireThreadLock( &lock, 1000 );
   if ( errcode != 0 )
      ERROR_EXIT( 1, NULL, );

   vdscReleaseThreadLock( &lock );

   vdscFiniThreadLock( &lock );

   return 0;
}


