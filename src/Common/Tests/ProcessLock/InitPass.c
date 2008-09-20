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


#include "Common/Common.h"
#include "Common/ProcessLock.h"
#include "Tests/PrintError.h"

const bool expectedToPass = true;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int main()
{
   bool ok;
   psocProcessLock lock;

   ok = psocInitProcessLock( &lock );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   if ( lock.initialized != PSOC_LOCK_SIGNATURE ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* A second init call is allowed */
   ok = psocInitProcessLock( &lock );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   psocFiniProcessLock( &lock );

   return 0;
}
