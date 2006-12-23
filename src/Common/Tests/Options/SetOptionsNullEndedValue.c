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

#include "Options.h"
#include "PrintError.h"

const bool expectedToPass = false;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int errcode = 0, i;
   vdscOptionHandle handle;
   
   struct vdscOptStruct opts[5] = 
      { '3', "three",   1, "", "repeat the loop three times",
        'a', "address", 0, "WATCHDOG_ADDRESS", "tcp/ip port number of the watchdog",
        'x', "",        1, "DISPLAY", "X display to use",
        'v', "verbose", 1, "", "try to explain what is going on",
        'z', "zzz",     1, "", "go to sleep..."
      };   

   for ( i = 0; i < VDSC_OPT_ARGUMENT_MSG_LENGTH; ++i )
      opts[2].argumentMessage[i] = 'x';
   
   errcode = vdscSetSupportedOptions( 5, opts, &handle );

   ERROR_EXIT( expectedToPass, NULL, ; );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

