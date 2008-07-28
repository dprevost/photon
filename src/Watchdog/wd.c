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

#include "API/WatchdogCommon.h"
#include "Watchdog/Watchdog.h"
#include "Common/Options.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char *argv[] )
{
   vdswWatchdog wDog;
   int errcode = 0;
   
   vdscOptionHandle optHandle;
   char *optArgument;
#if defined (WIN32)
   struct vdscOptStruct opts[5] = {
      { 'c', "config",    0, "filename", "Filename for the configuration options" },
      { 'i', "install",   1, "",         "Install the program as a NT service (Windows only)" },
      { 't', "test",      1, "",         "Test the config file and exit" },
      { 'u', "uninstall", 1, "",         "Uninstall the program as a NT service (Windows only)" },
      { 'v', "verify",    1, "",         "Verify the VDS and exit" }
   };
   errcode = vdscSetSupportedOptions( 5, opts, &optHandle );
#else
   struct vdscOptStruct opts[4] = {
      { 'c', "config", 0, "filename", "Filename for the configuration options" },
      { 'd', "daemon", 1, "",         "Run the program as a Unix daemon (Unix/linux only)" },
      { 't', "test",   1, "",         "Test the config file and exit" },
      { 'v', "verify", 1, "",         "Verify the VDS and exit" }
   };

   g_pWD = &wDog;
   vdswWatchdogInit( g_pWD );

   errcode = vdscSetSupportedOptions( 4, opts, &optHandle );
#endif
   if ( errcode != 0 ) {
      fprintf( stderr, 
         "Internal error in vdscSetSupportedOptions, error code = %d\n",
         errcode );
      return 1;
   }
   
   errcode = vdscValidateUserOptions( optHandle, argc, argv, 1 );
   if ( errcode < 0 ) {
      vdscShowUsage( optHandle, argv[0], "" );
      return 1;
   }
   
   if ( errcode > 0 ) {
      vdscShowUsage( optHandle, argv[0], "" );
      return 0;
   }

   if ( vdscGetShortOptArgument( optHandle, 'c', &optArgument ) ) {
      errcode = vdswWatchdogReadConfig( &wDog, optArgument );
      if ( errcode != 0 ) {
         fprintf( stderr, "%s\n", g_pWD->errorMsg );
         return errcode;
      }
   }

   // In test mode, we test the config file and exit.
   if ( vdscIsShortOptPresent( optHandle, 't' ) ) return 0;
   
#if defined ( WIN32 )
   if ( vdscIsShortOptPresent( optHandle, 'i' ) ) {
      errcode = vdswInstall( &wDog );
      if ( errcode != 0 ) return errcode;
      return 0;
   }
   
   if ( vdscIsShortOptPresent( optHandle, 'u' ) ) {
      vdswUninstall( &wDog );
      return 0;
   }
#endif
   if ( vdscIsShortOptPresent( optHandle, 'v' ) ) {
      wDog.verifyVDSOnly = true;
   }

   errcode = vdswInitializeVDS( &wDog );
   if ( errcode != 0 ) {
      vdswSendMessage( &wDog.log, 
                       WD_ERROR, 
                       "VDS initialization error  - aborting..." );
      return errcode;
   }

   if ( wDog.verifyVDSOnly ) return 0;
   
#if ! defined ( WIN32 )
   if ( vdscIsShortOptPresent( optHandle, 'd' ) ) {
      errcode = vdswDaemon( &wDog );
      if ( errcode != 0 ) return errcode;
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

