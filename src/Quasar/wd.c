/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@photonsoftware.org>
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

#include "API/WatchdogCommon.h"
#include "Quasar/Watchdog.h"
#include "Common/Options.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char *argv[] )
{
   psoqWatchdog wDog;
   int errcode = 0;
   bool ok;
   
   psocOptionHandle optHandle;
   char *optArgument;
#if defined (WIN32)
   struct psocOptStruct opts[5] = {
      { 'c', "config",    0, "filename", "Filename for the configuration options" },
      { 'i', "install",   1, "",         "Install the program as a NT service (Windows only)" },
      { 't', "test",      1, "",         "Test the config file and exit" },
      { 'u', "uninstall", 1, "",         "Uninstall the program as a NT service (Windows only)" },
      { 'v', "verify",    1, "",         "Verify the shared memory and exit" }
   };
#else
   struct psocOptStruct opts[4] = {
      { 'c', "config", 0, "filename", "Filename for the configuration options" },
      { 'd', "daemon", 1, "",         "Run the program as a Unix daemon (Unix/linux only)" },
      { 't', "test",   1, "",         "Test the config file and exit" },
      { 'v', "verify", 1, "",         "Verify the shared memory and exit" }
   };
#endif

   g_pWD = &wDog;
   psoqWatchdogInit( g_pWD );

#if defined (WIN32)
   ok = psocSetSupportedOptions( 5, opts, &optHandle );
#else
   ok = psocSetSupportedOptions( 4, opts, &optHandle );
#endif
   PSO_POST_CONDITION( ok == true || ok == false );

   if ( ! ok ) {
      fprintf( stderr, "Internal error in psocSetSupportedOptions\n" );
      return 1;
   }
   
   errcode = psocValidateUserOptions( optHandle, argc, argv, 1 );
   if ( errcode < 0 ) {
      psocShowUsage( optHandle, argv[0], "" );
      return 1;
   }
   
   if ( errcode > 0 ) {
      psocShowUsage( optHandle, argv[0], "" );
      return 0;
   }

   if ( psocGetShortOptArgument( optHandle, 'c', &optArgument ) ) {
      ok = psoqWatchdogReadConfig( &wDog, optArgument );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) {
         fprintf( stderr, "%s\n", g_pWD->errorMsg );
         return -1;
      }
   }

   // In test mode, we test the config file and exit.
   if ( psocIsShortOptPresent( optHandle, 't' ) ) return 0;
   
#if defined ( WIN32 )
   if ( psocIsShortOptPresent( optHandle, 'i' ) ) {
      ok = psoqInstall( &wDog );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) return -1;
      return 0;
   }
   
   if ( psocIsShortOptPresent( optHandle, 'u' ) ) {
      psoqUninstall( &wDog );
      return 0;
   }
#endif
   if ( psocIsShortOptPresent( optHandle, 'v' ) ) {
      wDog.verifyVDSOnly = true;
   }

   ok = psoqInitializeVDS( &wDog );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) {
      psoqSendMessage( &wDog.log, 
                       WD_ERROR, 
                       "VDS initialization error  - aborting..." );
      return -1;
   }

   if ( wDog.verifyVDSOnly ) return 0;
   
#if ! defined ( WIN32 )
   if ( psocIsShortOptPresent( optHandle, 'd' ) ) {
      ok = psoqDaemon( &wDog );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) return -1;
   }
#endif

   /*
    * This is the main loop. If using Windows NT services, this loop
    * is called by the Service Control Manager (SCM) directly.
    */
   psoqRun();
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

