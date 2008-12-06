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

#ifndef ENGINE_TEST_COMMON_H
#define ENGINE_TEST_COMMON_H

#include "Nucleus/Engine.h"
#include "Nucleus/InitEngine.h"
#include "Tests/PrintError.h"
#include "Nucleus/SessionContext.h"

PHOTON_ENGINE_EXPORT
psocErrMsgHandle g_psoErrorHandle;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This function should be called by every program that requires the
 * errorhandle for the Photon errors to be initialized. 
 */
 
void initTest( bool testIsExpectedToSucceed, psonSessionContext* pContext )
{
   bool ok;
   
   memset( pContext, 0, sizeof(psonSessionContext) );
   pContext->pidLocker = getpid();
   
   ok = psonInitEngine();
   if ( ok != true ) {
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   
   psocInitErrorHandler( &pContext->errorHandler );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* ENGINE_TEST_COMMON_H */

