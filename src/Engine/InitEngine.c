/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of the vdsf (Virtual Data Space Framework) Library.
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

#include "Engine.h"
#include "InitEngine.h"
#include "Common/ErrorHandler.h"
#include "VdsErrorHandler.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

VDSF_ENGINE_EXPORT vdscErrMsgHandle g_vdsErrorHandle = VDSC_NO_ERRHANDLER;

#if defined(WIN32)
VDSF_ENGINE_EXPORT const char * MYCPU = "i386";
VDSF_ENGINE_EXPORT const char * MYCC  = "cl.exe.";
VDSF_ENGINE_EXPORT const char * MYCXX = "cl.exe";
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static int vdseGetErrorMsg( int errnum, char *msg, unsigned int msgLength )
{
   const char * theMsg;

   VDS_PRE_CONDITION( msg != NULL);
   VDS_PRE_CONDITION( msgLength > 0 );

   theMsg = vdse_ErrorMessage( errnum );
   if ( theMsg == NULL ) return -1;
   if ( strlen(theMsg) >= msgLength ) return -1;
   
   strcpy( msg, theMsg );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseInitEngine()
{
   if ( g_vdsErrorHandle == VDSC_NO_ERRHANDLER ) {
      vdscInitErrorDefs();

      g_vdsErrorHandle = vdscAddErrorMsgHandler( "VDSF", vdseGetErrorMsg );

      if ( g_vdsErrorHandle == VDSC_NO_ERRHANDLER ) {
         fprintf( stderr, "Error registring the error handler for VDS errors\n" );
         fprintf( stderr, "The problem might be a lack of memory\n" );
         return -1;
      }
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

