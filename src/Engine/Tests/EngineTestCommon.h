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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifndef ENGINE_TEST_COMMON_H
#define ENGINE_TEST_COMMON_H

#include "Engine.h"
#include "ErrorHandler.h"
#include "VdsErrorHandler.h"
#include "PrintError.h"

vdscErrMsgHandle g_vdsErrorHandle;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This function should be called by every function that requires the
 * errorhandle for the VDS errors tobe initialized. 
 */
 
void initTest( bool testIsExpectedToSucceed )
{
   vdscInitErrorDefs();

   g_vdsErrorHandle = vdscAddErrorMsgHandler( "VDSF", vdseErrGetErrMessage );

   if ( g_vdsErrorHandle == VDSC_NO_ERRHANDLER )
   {
      fprintf( stderr, "Error registring the error handler for VDS errors\n" );
      fprintf( stderr, "The problem might be a lack of memory\n" );
      /* We do the opposite of what a successful test should return to
       * signal the issue. */
      if ( testIsExpectedToSucceed )
         exit(1);
      exit(0);
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* ENGINE_TEST_COMMON_H */

