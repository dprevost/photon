/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@photonsoftware.org>
 *
 * This file is part of Photon (photonsoftware.org).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 or version 3 as published by the 
 * Free Software Foundation and appearing in the file COPYING.GPL2 and 
 * COPYING.GPL3 included in the packaging of this software.
 *
 * Licensees holding a valid Photon Commercial license can use this file 
 * in accordance with the terms of their license.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "API/QuasarCommon.h"
#include "Quasar/Quasar.h"
#include "Common/Options.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char *argv[] )
{
   qsrQuasar wDog;
   int errcode = 0, i;
   bool ok, exitAfterOpen = false;
   char configName[PATH_MAX];
   char c;

   psocOptionHandle optHandle;
   char *optArgument;
#if defined (WIN32)
   struct psocOptStruct opts[6] = {
      { 'c', "config",    1, "filename", "Filename for the configuration options" },
      { 'i', "install",   1, "",         "Install the program as a NT service (Windows only)" },
      { 'e', "exit",      1, "",         "Create/open/verify a shared memory and exit" },
      { 't', "test",      1, "",         "Test the config file and exit" },
      { 'u', "uninstall", 1, "",         "Uninstall the program as a NT service (Windows only)" },
      { 'v', "verify",    1, "",         "Verify the shared memory and exit" }
   };
#else
   struct psocOptStruct opts[5] = {
      { 'c', "config", 1, "filename", "Filename for the configuration options" },
      { 'd', "daemon", 1, "",         "Run the program as a Unix daemon (Unix/linux only)" },
      { 'e', "exit",   1, "",         "Create/open/verify a shared memory and exit" },
      { 't', "test",   1, "",         "Test the config file and exit" },
      { 'v', "verify", 1, "",         "Verify the shared memory and exit" }
   };
#endif

   g_pQSR = &wDog;
   qsrQuasarInit( g_pQSR );

#if defined (WIN32)
   ok = psocSetSupportedOptions( 6, opts, &optHandle );
#else
   ok = psocSetSupportedOptions( 5, opts, &optHandle );
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

   if ( psocIsShortOptPresent( optHandle, 'c' ) ) {
      psocGetShortOptArgument( optHandle, 'c', &optArgument );
   }
   else {
      memset( configName, 0, PATH_MAX );
      printf( "Please enter the name of the XML configuration file: " );
      for ( i = 0; i < PATH_MAX-1 &&  (c = getchar()) != EOF && c != '\n'; i++ ) {
         configName[i] = c;
      }
      optArgument = configName;
   }
   
   ok = qsrQuasarReadConfig( &wDog, optArgument );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) {
      fprintf( stderr, "%s\n", g_pQSR->errorMsg );
      return -1;
   }

   // In test mode, we test the config file and exit.
   if ( psocIsShortOptPresent( optHandle, 't' ) ) return 0;

   // Do we create a new shared mem and exit?
   if ( psocIsShortOptPresent( optHandle, 'e' ) ) {
      exitAfterOpen = true;
   }
   
#if defined ( WIN32 )
   if ( psocIsShortOptPresent( optHandle, 'i' ) ) {
      ok = qsrInstall( &wDog );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) return -1;
      return 0;
   }
   
   if ( psocIsShortOptPresent( optHandle, 'u' ) ) {
      qsrUninstall( &wDog );
      return 0;
   }
#endif
   if ( psocIsShortOptPresent( optHandle, 'v' ) ) {
      wDog.verifyMemOnly = true;
   }

   ok = qsrInitializeMem( &wDog );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) {
      qsrSendMessage( &wDog.log, 
                       QSR_ERROR, 
                       "Shared memory initialization error  - aborting..." );
      return -1;
   }

   if ( wDog.verifyMemOnly || exitAfterOpen ) return 0;
   
#if ! defined ( WIN32 )
   if ( psocIsShortOptPresent( optHandle, 'd' ) ) {
      ok = qsrDaemon( &wDog );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) return -1;
   }
#endif

   /*
    * This is the main loop. If using Windows NT services, this loop
    * is called by the Service Control Manager (SCM) directly.
    */
   qsrRun();
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

