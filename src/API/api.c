/*
 * Copyright (C) 2007 Daniel Prevost <dprevost@users.sourceforge.net>
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
#include <vdsf/vds_c.h>
#include <vdsf/vdsCommon.h>
#include "Process.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
 * 
 * This file contains the source code for the C API, For a
 * description of the functions, please see the relevant header 
 * file.
 * 
 * --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
extern "C" {
#endif
  
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsInit( const char* wdAddress,
             int         programIsMultiThreaded,
             VDS_HANDLE* processHandle )                  
{
   int errcode = VDS_OK;
   vdsaProcess* pProcess;

   if ( processHandle == NULL )
      return VDS_INVALID_HANDLE;
   if ( wdAddress == NULL )
      return VDS_INVALID_WATCHDOG_ADDRESS;
   
   g_protectionIsNeeded = programIsMultiThreaded;
   
   if ( g_protectionIsNeeded )
   {
      errcode = vdscInitThreadLock( &g_ProcessMutex );
      if ( errcode != 0 )
         return VDS_NOT_ENOUGH_RESOURCES;
   }

   pProcess = (vdsaProcess*) malloc( sizeof(vdsaProcess) );
   if ( pProcess == NULL )
      return VDS_NOT_ENOUGH_HEAP_MEMORY;
   
   errcode = vdsaProcessInit( pProcess,
                              wdAddress );
   if ( errcode == VDS_OK )
   {
      *processHandle = (VDS_HANDLE) pProcess;
   }
   else
      free( pProcess );

   return errcode;
}
    
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdsExit( VDS_HANDLE handle )
{
   vdsaProcess* pProcess;

   pProcess = (vdsaProcess*) handle;
   if ( pProcess != NULL )
   {
      vdsaProcessFini( pProcess );
   }
} 
    
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
}
#endif

