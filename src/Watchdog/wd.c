/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of photon (photonsoftware.org).
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

#include "API/WatchdogCommon.h"
#include "Watchdog/Watchdog.h"
#include "Common/Options.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char *argv[] )
{
   vdswWatchdog wDog;
   int errcode = 0;
   bool ok;
   
   pscOptionHandle optHandle;
   char *optArgument;
#if defined (WIN32)
   struct pscOptStruct opts[5] = {
      { 'c', "config",    0, "filename", "Filename for the configuration options" },
      { 'i', "install",   1, "",         "Install the program as a NT service (Windows only)" },
      { 't', "test",      1, "",         "Test the config file and exit" },
      { 'u', "uninstall", 1, "",         "Uninstall the program as a NT service (Windows only)" },
      { 'v', "verify",    1, "",         "Verify the VDS and exit" }
   };
#else
   struct pscOptStruct opts[4] = {
      { 'c', "config", 0, "filename", "Filename for the configuration options" },
      { 'd', "daemon", 1, "",         "Run the program as a Unix daemon (Unix/linux only)" },
      { 't', "test",   1, "",         "Test the config file and exit" },
      { 'v', "verify", 1, "",         "Verify the VDS and exit" }
   };
#endif

   g_pWD = &wDog;
   vdswWatchdogInit( g_pWD );

#if defined (WIN32)
   ok = pscSetSupportedOptions( 5, opts, &optHandle );
#else
   ok = pscSetSupportedOptions( 4, opts, &optHandle );
#endif
   PSO_POST_CONDITION( ok == true || ok == false );

   if ( ! ok ) {
      fprintf( stderr, "Internal error in pscSetSupportedOptions\n" );
      return 1;
   }
   
   errcode = pscValidateUserOptions( optHandle, argc, argv, 1 );
   if ( errcode < 0 ) {
      pscShowUsage( optHandle, argv[0], "" );
      return 1;
   }
   
   if ( errcode > 0 ) {
      pscShowUsage( optHandle, argv[0], "" );
      return 0;
   }

   if ( pscGetShortOptArgument( optHandle, 'c', &optArgument ) ) {
      ok = vdswWatchdogReadConfig( &wDog, optArgument );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) {
         fprintf( stderr, "%s\n", g_pWD->errorMsg );
         return -1;
      }
   }

   // In test mode, we test the config file and exit.
   if ( pscIsShortOptPresent( optHandle, 't' ) ) return 0;
   
#if defined ( WIN32 )
   if ( pscIsShortOptPresent( optHandle, 'i' ) ) {
      ok = vdswInstall( &wDog );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) return -1;
      return 0;
   }
   
   if ( pscIsShortOptPresent( optHandle, 'u' ) ) {
      vdswUninstall( &wDog );
      return 0;
   }
#endif
   if ( pscIsShortOptPresent( optHandle, 'v' ) ) {
      wDog.verifyVDSOnly = true;
   }

   ok = vdswInitializeVDS( &wDog );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) {
      vdswSendMessage( &wDog.log, 
                       WD_ERROR, 
                       "VDS initialization error  - aborting..." );
      return -1;
   }

   if ( wDog.verifyVDSOnly ) return 0;
   
#if ! defined ( WIN32 )
   if ( pscIsShortOptPresent( optHandle, 'd' ) ) {
      ok = vdswDaemon( &wDog );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) return -1;
   }
#endif

   /*
    * This is the main loop. If using Windows NT services, this loop
    * is called by the Service Control Manager (SCM) directly.
    */
   vdswRun();
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

