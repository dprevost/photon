/*
 * Copyright (C) 2006-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#include "Common/Common.h"
#include "Common/ProcessLock.h"
#include "Tests/PrintError.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   psocProcessLock lock;
   pid_t pid = getpid();
   bool ok;
   
   ok = psocInitProcessLock( &lock );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   ok = psocTryAcquireProcessLock( &lock, pid, 0 );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   ok = psocTryAcquireProcessLock( &lock, pid, 1000 );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   ok = psocIsItLocked( &lock );   
   if ( ok == false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   ok = psocTestLockPidValue( &lock, pid );
   if ( ok == false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psocReleaseProcessLock( &lock );

   /* Calling it a second time should do nothing */
   psocReleaseProcessLock( &lock );

   ok = psocTryAcquireProcessLock( &lock, pid, 1000 );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   psocReleaseProcessLock( &lock );

   psocFiniProcessLock( &lock );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

