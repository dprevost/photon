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
   psocProcessLock lock;
   bool ok;
   
   ok = psocInitProcessLock( &lock );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   ok = psocTryAcquireProcessLock( &lock, 0xff, 100 );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psocReleaseProcessLock( &lock );

   psocFiniProcessLock( &lock );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

