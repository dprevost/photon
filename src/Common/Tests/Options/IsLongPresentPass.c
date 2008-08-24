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

#include "Common/Options.h"
#include "Tests/PrintError.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int errcode = 0;
   char dummyArgs[100];
   char *dummyPtrs[10];
   pscOptionHandle handle;
   bool gotIt, ok;
   
   struct pscOptStruct opts[5] = {
      { '3', "three",   1, "", "repeat the loop three times" },
      { 'a', "address", 0, "WATCHDOG_ADDRESS", "tcp/ip port number of the watchdog" },
      { 'x', "",        1, "DISPLAY", "X display to use" },
      { 'v', "verbose", 1, "", "try to explain what is going on" },
      { 'z', "zzz",     1, "", "go to sleep..." }
   };
   
   ok = pscSetSupportedOptions( 5, opts, &handle );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   strcpy( dummyArgs, "OptionTest2 --address 12345 -v --zzz" );
   /*                  012345678901234567890123456789012345 */
   dummyPtrs[0] = dummyArgs;
   dummyPtrs[1] = &dummyArgs[12];
   dummyPtrs[2] = &dummyArgs[22];
   dummyPtrs[3] = &dummyArgs[28];
   dummyPtrs[4] = &dummyArgs[31];

   dummyArgs[11] = 0;
   dummyArgs[21] = 0;
   dummyArgs[27] = 0;
   dummyArgs[30] = 0;

   errcode = pscValidateUserOptions( handle, 5, dummyPtrs, 1 );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Option is present */
   gotIt = pscIsLongOptPresent( handle, "address" );
   if ( ! gotIt ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Option is absent */
   gotIt = pscIsLongOptPresent( handle, "three" );
   if ( gotIt ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Unknown option */
   gotIt = pscIsLongOptPresent( handle, "zzaddress" );
   if ( gotIt ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   pscUnsetSupportedOptions( handle );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

