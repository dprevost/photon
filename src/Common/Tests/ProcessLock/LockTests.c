/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of Photon (photonsoftware.org).
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

#include "Common/Common.h"
#include "Common/ProcessLock.h"
#include "Tests/PrintError.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   pscProcessLock lock;
   pid_t pid = getpid();
   bool ok;
   
   ok = pscInitProcessLock( &lock );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   ok = pscTryAcquireProcessLock( &lock, pid, 0 );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   ok = pscTryAcquireProcessLock( &lock, pid, 1000 );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   ok = pscIsItLocked( &lock );   
   if ( ok == false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   ok = pscTestLockPidValue( &lock, pid );
   if ( ok == false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   pscReleaseProcessLock( &lock );

   /* Calling it a second time should do nothing */
   pscReleaseProcessLock( &lock );

   ok = pscTryAcquireProcessLock( &lock, pid, 1000 );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pscReleaseProcessLock( &lock );

   pscFiniProcessLock( &lock );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

