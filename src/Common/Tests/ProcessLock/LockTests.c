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
#include "ProcessLock.h"
#include "PrintError.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int errcode;
   vdscProcessLock lock;
   pid_t pid = getpid();
   
   errcode = vdscInitProcessLock( &lock );
   if ( errcode != 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   errcode = vdscTryAcquireProcessLock( &lock, pid, 0 );
   if ( errcode != 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );

   errcode = vdscTryAcquireProcessLock( &lock, pid, 1000 );
   if ( errcode == 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );

   errcode = vdscIsItLocked( &lock );   
   if ( errcode == 0 ) 
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   errcode = vdscTestLockPidValue( &lock, pid );
   if ( errcode == 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );
      
   vdscReleaseProcessLock( &lock );

   /* Calling it a second time should do nothing */
   vdscReleaseProcessLock( &lock );

   errcode = vdscTryAcquireProcessLock( &lock, pid, 1000 );
   if ( errcode != 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );

   vdscReleaseProcessLock( &lock );

   vdscFiniProcessLock( &lock );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

