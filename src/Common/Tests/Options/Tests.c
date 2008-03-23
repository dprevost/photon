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

#include "Common/Options.h"
#include "Tests/PrintError.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int errcode = 0;
   vdscOptionHandle handle;
   char dummyArgs[100];
   char *dummyPtrs[10];

   struct vdscOptStruct opts[5] = {
      { '3', "three",   1, "", "repeat the loop three times" },
      { 'a', "address", 0, "WATCHDOG_ADDRESS", "tcp/ip port number of the watchdog" },
      { 'x', "",        1, "DISPLAY", "X display to use" },
      { 'v', "verbose", 1, "", "try to explain what is going on" },
      { 'z', "zzz",     1, "", "go to sleep..." }
   };

   dummyPtrs[0] = dummyArgs;
   
   errcode = vdscSetSupportedOptions( 5, opts, &handle );
   if ( errcode != 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   strcpy( dummyArgs, "OptionTest2 -?" );
   dummyPtrs[1] = &dummyArgs[12];
   dummyArgs[11] = 0;
   
   errcode = vdscValidateUserOptions( handle, 2, dummyPtrs, 1 );
   if ( errcode != 1 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   strcpy( dummyArgs, "OptionTest2 -a 12345" );
   dummyPtrs[1] = &dummyArgs[12];
   dummyPtrs[2] = &dummyArgs[15];
   dummyArgs[11] = 0;
   dummyArgs[14] = 0;   

   errcode = vdscValidateUserOptions( handle, 3, dummyPtrs, 1 );
   if ( errcode != 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

