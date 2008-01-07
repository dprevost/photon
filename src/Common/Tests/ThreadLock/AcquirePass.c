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

#include "Common/Common.h"
#include "Common/ThreadLock.h"
#include "Tests/PrintError.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int errcode;
   vdscThreadLock lock;

   errcode = vdscInitThreadLock( &lock );
   if ( errcode != 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   vdscAcquireThreadLock( &lock );

   /*
    * Normally we should test if the lock is really in place. Because
    * CriticalSections are recursive and the most simple pthread mutex
    * is not, there is no easy way to test this unless you try to 
    * acquire the mutex from a different thread.
    *
    * We might want to add this test later.
    */
   
   vdscReleaseThreadLock( &lock );

   vdscFiniThreadLock( &lock );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

