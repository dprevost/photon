/*
 * Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#include "Common/Common.h"
#include <photon/photon.h>
#include "Process.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
 * 
 * This file contains the source code for some functions of the C API. 
 * For documentation for these functions, please see the distributed
 * header files.
 * 
 * --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
extern "C" {
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoInit( const char * qsrAddress, const char * processName )                  
{
   int errcode = PSO_OK;
   psoaProcess * process;
   bool ok;
   
   if ( qsrAddress == NULL ) return PSO_INVALID_QUASAR_ADDRESS;
   
   ok = psocInitThreadLock( &g_ProcessMutex );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) return PSO_NOT_ENOUGH_RESOURCES;

   process = (psoaProcess *) malloc( sizeof(psoaProcess) );
   if ( process == NULL ) return PSO_NOT_ENOUGH_HEAP_MEMORY;
  
   memset( process, 0, sizeof(psoaProcess) );
   errcode = psoaProcessInit( process, qsrAddress, processName );

   if ( errcode != PSO_OK ) free( process );

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psoExit()
{
   if ( g_pProcessInstance != NULL ) {
      psoaProcessFini();
   }
   g_pProcessInstance = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

