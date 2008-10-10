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
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

#include "Nucleus/Engine.h"
#include "Nucleus/InitEngine.h"
#include "Common/ErrorHandler.h"
#include "Nucleus/psoErrorHandler.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

PHOTON_ENGINE_EXPORT psocErrMsgHandle g_psoErrorHandle = PSOC_NO_ERRHANDLER;

#if defined(WIN32)
PHOTON_ENGINE_EXPORT const char * MYCPU = "i386";
PHOTON_ENGINE_EXPORT const char * MYCC  = "cl.exe.";
PHOTON_ENGINE_EXPORT const char * MYCXX = "cl.exe";
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static int psonGetErrorMsg( int errnum, char *msg, unsigned int msgLength )
{
   const char * theMsg;

   PSO_PRE_CONDITION( msg != NULL);
   PSO_PRE_CONDITION( msgLength > 0 );

   theMsg = pson_ErrorMessage( errnum );
   if ( theMsg == NULL ) return -1;
   if ( strlen(theMsg) >= msgLength ) return -1;
   
   strcpy( msg, theMsg );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonInitEngine()
{
   if ( g_psoErrorHandle == PSOC_NO_ERRHANDLER ) {
      if ( ! psocInitErrorDefs() ) {
         fprintf( stderr, "Internal error in psocInitErrorDefs()\n" );
      }

      g_psoErrorHandle = psocAddErrorMsgHandler( "Photon", psonGetErrorMsg );

      if ( g_psoErrorHandle == PSOC_NO_ERRHANDLER ) {
         fprintf( stderr, "Error registring the error handler for VDS errors\n" );
         fprintf( stderr, "The problem might be a lack of memory\n" );
         return false;
      }
   }

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

