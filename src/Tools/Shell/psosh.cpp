/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@photonsoftware.org>
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

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#include "Common/Common.h"
#include "Common/Options.h"
#include "Tools/Shell/shell.h"
#include <photon/photon>
#include <iostream>

using namespace std;
using namespace pso;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int main( int argc, char *argv[] )
{
   int errcode = 0;
   Process process;
   Session session;
   psoShell   sh(session);
   bool ok;
   
   psocOptionHandle optHandle;
   char *optArgument;
   struct psocOptStruct opts[1] = { 
      { 'a', "address", 0, "quasar_address", "The address of the Photon server" }
   };

   ok = psocSetSupportedOptions( 1, opts, &optHandle );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) return 1;

   errcode = psocValidateUserOptions( optHandle, argc, argv, 1 );
   if ( errcode < 0 ) {
      psocShowUsage( optHandle, argv[0], "" );
      return 1;
   }
   if ( errcode > 0 ) {
      psocShowUsage( optHandle, argv[0], "" );
      cout << endl;
      sh.Man();
      
      return 0;
   }

   psocGetShortOptArgument( optHandle, 'a', &optArgument );
   try {
      process.Init( optArgument );
      session.Init();
   }
   catch( Exception exc ) {
      cerr << "Init Photon failed, error = " << exc.Message() << endl;
      cerr << "Is Quasar running?" << endl;
      return 1;
   }

   sh.Run();
   
   cout << endl;

   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
