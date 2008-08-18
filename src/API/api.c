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

#include "Common/Common.h"
#include <vdsf/vds.h>
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

int vdsInit( const char * wdAddress,
             int          programIsMultiThreaded )                  
{
   int errcode = VDS_OK;
   vdsaProcess * process;
   bool ok;
   
   if ( wdAddress == NULL ) return VDS_INVALID_WATCHDOG_ADDRESS;
   
   g_protectionIsNeeded = programIsMultiThreaded;
   
   if ( g_protectionIsNeeded ) {
      ok = pscInitThreadLock( &g_ProcessMutex );
      VDS_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) return VDS_NOT_ENOUGH_RESOURCES;
   }

   process = (vdsaProcess *) malloc( sizeof(vdsaProcess) );
   if ( process == NULL ) return VDS_NOT_ENOUGH_HEAP_MEMORY;
  
   memset( process, 0, sizeof(vdsaProcess) );
   errcode = vdsaProcessInit( process, wdAddress );

   if ( errcode != VDS_OK ) free( process );

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdsExit()
{
   if ( g_pProcessInstance != NULL ) {
      vdsaProcessFini();
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

