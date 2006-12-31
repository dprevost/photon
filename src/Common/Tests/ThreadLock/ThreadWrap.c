/*
 * Copyright (C) 2006 Daniel Prevost <dprevost@users.sourceforge.net>
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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "ThreadWrap.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdstCreateThread( vdstThreadWrap* pThread, 
                      void *(*startRoutine)(void*),
                      void* arg,
                      vdscErrorHandler* pError )
{
   int errcode = -1;
#if defined (WIN32)
   HANDLE handle;
   unsigned threadId;
#endif

   VDS_PRE_CONDITION( pThread != NULL );
   VDS_PRE_CONDITION( startRoutine != NULL );

   pThread->arg = arg;
   pThread->returnCode = 0;
   
#if defined (WIN32)
   handle = (HANDLE) _beginthreadex( NULL, /* Default sec. attributes */
                                     0,    /* Default stack size */
                                     &startRoutine,
                                     (void*)pThread,
                                     0, /* Thread does not start suspended */
                                     &threadId );
   if ( handle == NULL )
   {
      /* _beginthreadex does indeed set errno. */
      vdscSetError( pError, VDSC_ERRNO_HANDLE, errno );
      return -1;
   }
   pThread->hThread = handle;
   
#else
   errcode = pthread_create ( &pThread->threadId, 
                              NULL, startRoutine, (void*)pThread );

   if ( errcode != 0 )
   {
      if ( errcode > 0 )
         vdscSetError( pError, VDSC_ERRNO_HANDLE, errcode );
      else
         vdscSetError( pError, VDSC_ERRNO_HANDLE, errno );
      return -1;
   }
#endif

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdstJoinThread( vdstThreadWrap* pThread, 
//                    void* retValue,
                    vdscErrorHandler* pError )
{
#if defined (WIN32)
   DWORD err, value;

   err = WaitForSingleObject( pThread->hThread, INFINITE );
   if ( err == WAIT_OBJECT_0 )
   {
//      if ( GetExitCodeThread( pThread->hThread, &value ) == TRUE )
//      {
         CloseHandle( pThread->hThread );
//         if ( retValue != NULL )
//            *(void *)retValue = value;
         return 0;
//      }
   }

   vdscSetError( pError, VDSC_WINERR_HANDLE, GetLastError() );
   return -1;

#else
   int errcode;
   
   errcode = pthread_join( pThread->threadId, NULL ); // &retValue
   if ( errcode != 0 )
   {
      if ( errcode > 0 )
         vdscSetError( pError, VDSC_ERRNO_HANDLE, errcode );
      else
         vdscSetError( pError, VDSC_ERRNO_HANDLE, errno );
      return -1;
   }
   return 0;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

