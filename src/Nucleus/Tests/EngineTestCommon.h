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

#ifndef ENGINE_TEST_COMMON_H
#define ENGINE_TEST_COMMON_H

#include "Nucleus/Engine.h"
#include "Nucleus/InitEngine.h"
#include "Tests/PrintError.h"
#include "Nucleus/SessionContext.h"

VDSF_ENGINE_EXPORT
pscErrMsgHandle g_vdsErrorHandle;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This function should be called by every function that requires the
 * errorhandle for the VDS errors tobe initialized. 
 */
 
void initTest( bool testIsExpectedToSucceed, psnSessionContext* pContext )
{
   bool ok;
   
   memset( pContext, 0, sizeof(psnSessionContext) );
   pContext->pidLocker = getpid();
   
   ok = psnInitEngine();
   if ( ok != true ) {
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   
   pscInitErrorHandler( &pContext->errorHandler );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* ENGINE_TEST_COMMON_H */

