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
#include "ProcessLock.h"

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int main( int argc, char* argv[] )
{
   int errcode;
   vdscProcessLock lock;

   errcode = vdscInitLock( &lock, 1 );
   if ( errcode != 0 )
   {
      fprintf( stderr, "Unexpected error in vdscInitLock!\n" );
      return 1;
   }
   
   vdscInitGuard( &lock, 0xff );

   if ( ! vdscTestLockPidValue( &lock, 0xff ) )
   {
      fprintf( stderr, "Unexpected error in vdscTestLockPidValue!\n" );
      return -1;
   }

   vdscFiniGuard( &lock );

   vdscFiniLock( &lock );

   return 0;
}
