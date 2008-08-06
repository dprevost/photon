/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#include "Common/Common.h"
#include "Common/Options.h"
#include "Tools/Shell/shell.h"
#include <vdsf/vds>
#include <iostream>

using namespace std;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int main( int argc, char *argv[] )
{
   int errcode = 0;
   vdsProcess process;
   vdsSession session;
   vdsShell   sh(session);
   bool ok;
   
   vdscOptionHandle optHandle;
   char *optArgument;
   struct vdscOptStruct opts[1] = { 
      { 'a', "address", 0, "watchdog_address", "The address of the VDSF watchdog" }
   };

   ok = vdscSetSupportedOptions( 1, opts, &optHandle );
   VDS_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) return 1;

   errcode = vdscValidateUserOptions( optHandle, argc, argv, 1 );
   if ( errcode < 0 ) {
      vdscShowUsage( optHandle, argv[0], "" );
      return 1;
   }
   if ( errcode > 0 ) {
      vdscShowUsage( optHandle, argv[0], "" );
      cout << endl;
      sh.Man();
      
      return 0;
   }

   vdscGetShortOptArgument( optHandle, 'a', &optArgument );
   try {
      process.Init( optArgument );
      session.Init();
   }
   catch( vdsException exc ) {
      cerr << "Init VDSF failed, error = " << exc.Message() << endl;
      cerr << "Is the watchdog running?" << endl;
      return 1;
   }

   sh.Run();
   
   cout << endl;

   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
