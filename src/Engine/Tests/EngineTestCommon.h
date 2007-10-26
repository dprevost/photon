/*
 * Copyright (C) 2006-2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

#include "Engine/Engine.h"
#include "Engine/InitEngine.h"
#include "Tests/PrintError.h"
#include "Engine/SessionContext.h"

VDSF_ENGINE_EXPORT
vdscErrMsgHandle g_vdsErrorHandle;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This function should be called by every function that requires the
 * errorhandle for the VDS errors tobe initialized. 
 */
 
void initTest( bool testIsExpectedToSucceed, vdseSessionContext* pContext )
{
   int errcode;
   
   memset( pContext, 0, sizeof(vdseSessionContext) );
   pContext->pidLocker = getpid();
   
   errcode = vdseInitEngine();
   if ( errcode != 0 )
   {
      if ( testIsExpectedToSucceed )
         exit(1);
      exit(0);
   }
   
   vdscInitErrorHandler( &pContext->errorHandler );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* ENGINE_TEST_COMMON_H */

