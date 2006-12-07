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

/* unreachable code */
#pragma warning(disable:4702) 

const bool expectedToPass = false;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int errcode;
   vdscProcessLock lock;

   errcode = vdscInitProcessLock( &lock );
   if ( errcode != 0 )
      ERROR_EXIT( expectedToPass, NULL, );
   
   vdscAcquireProcessLock( NULL, 0xff );

   vdscReleaseProcessLock( &lock );

   vdscFiniProcessLock( &lock );

   ERROR_EXIT( expectedToPass, NULL, );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

