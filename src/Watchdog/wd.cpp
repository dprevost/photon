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

/* 
 * "design":
 *
 * 1 - read configuration parameters - 
 *
 */


// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#include "WatchdogCommon.h"
#include "Watchdog.h"

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int main( int argc, char *argv[] )
{
   VdsWatchdog wDog;
   VdsWatchdog::g_pWD = &wDog;
   int errcode = 0;
   
   if ( argc < 2 )
   {
      wDog.Help( argv[0] );
      return -1;
   }
   
   if ( strcmp( argv[1], "--help" ) == 0 || strcmp( argv[1], "-h" ) == 0 )
   {
      wDog.Help( argv[0] );
      return 0;
   }

   if (strcmp( argv[1], "--uninstall" ) == 0 || strcmp( argv[1], "-u" ) == 0)
   {
#if defined ( WIN32 )
      errcode = wDog.Uninstall();
      if ( errcode != 0 )
         return errcode;
      return 0;
#else
      fprintf( stderr, "This option is only valid on Windows\n" );
      return -1;
   }
#endif

   // From now on, a config file is needed - we test the last argv for a
   // leading '-' just in case.
   if ( argv[argc-1][0] == '-' )
   {
      wDog.Help( argv[0] );
      return -1;
   }

   errcode = wDog.ReadConfig( argv[argc-1] );
   if ( errcode != 0 )
   {
      fprintf( stderr, "%s\n", wDog.GetErrorMsg() );

      return errcode;
   }
   
   if ( strcmp( argv[1], "--install" ) == 0 || strcmp( argv[1], "-i" ) == 0 )
   {
#if defined ( WIN32 )
      errcode = wDog.Install();
      if ( errcode != 0 )
         return errcode;
      return 0;
#else
      fprintf( stderr, "This option is only valid on Windows\n" );
      return -1;
   }
#endif

   if ( strcmp( argv[1], "--daemon" ) == 0 || strcmp( argv[1], "-d" ) == 0 )
   {
#if defined ( WIN32 )
      fprintf( stderr, "This option is only valid on Unix/linux\n" );
      return -1;
#else
      errcode = wDog.Daemon();
      if ( errcode != 0 )
         return errcode;
   }
#endif

   // In test mode, we test the config file and exit.
   if ( strcmp( argv[1], "--test" ) != 0 )
   {
      errcode = wDog.InitializeVDS();
      if ( errcode != 0 )
      {
         return errcode;
      }

      /*
       * This is the main loop. If using Windows NT services, this loop
       * is called by the Service Control Manager (SCM) directly.
       */
      VdsWatchdog::Run();
   }
   
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
