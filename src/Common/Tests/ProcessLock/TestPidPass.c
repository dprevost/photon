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

   errcode = vdscInitProcessLock( &lock );
   if ( errcode != 0 )
   {
      fprintf( stderr, "Unexpected error in vdscInitLock!\n" );
      return -1;
   }
   
   if ( vdscTestLockPidValue( &lock, 0xff ) )
   {
      fprintf( stderr, "TestLockPidValue invalid answer before Acquire\n" );
      return -1;
   }

   vdscAcquireProcessLock( &lock, 0xff );

   if ( !vdscTestLockPidValue( &lock, 0xff ) )
   {
      fprintf( stderr, "TestLockPidValue invalid answer after Acquire\n" );
      return -1;
   }

   vdscReleaseProcessLock( &lock );

   if ( vdscTestLockPidValue( &lock, 0xff ) )
   {
      fprintf( stderr, "TestLockPidValue invalid answer after Release\n" );
      return -1;
   }

   vdscFiniProcessLock( &lock );

   return 0;
}

