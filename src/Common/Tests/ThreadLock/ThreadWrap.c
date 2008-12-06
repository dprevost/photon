/*
 * Copyright (C) 2006, 2007-2008 Daniel Prevost <dprevost@photonsoftware.org>
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

#include "Common/Tests/ThreadLock/ThreadWrap.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined (WIN32)
unsigned __stdcall psotStartRoutine( void* arg )
#else
void * psotStartRoutine( void* arg )
#endif
{
   psotThreadWrap* pThread = (psotThreadWrap*) arg;

   pThread->returnCode = pThread->startRoutine( pThread->arg );
   
#if defined(WIN32)
   return 0;
#else
   return NULL;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psotCreateThread( psotThreadWrap* pThread, 
                      PSOT_THREAD_FUNCTION startRoutine,
                      void* arg,
                      psocErrorHandler* pError )
{
#if defined (WIN32)
   HANDLE handle;
   unsigned threadId;
#else
   int errcode = -1;
#endif

   PSO_PRE_CONDITION( pThread != NULL );
   PSO_PRE_CONDITION( startRoutine != NULL );

   pThread->arg = arg;
   pThread->returnCode = 0;
   pThread->startRoutine = startRoutine;
   
#if defined (WIN32)
   handle = (HANDLE) _beginthreadex( NULL, /* Default sec. attributes */
                                     0,    /* Default stack size */
                                     &psotStartRoutine,
                                     (void*)pThread,
                                     0, /* Thread does not start suspended */
                                     &threadId );
   if ( handle == NULL ) {
      /* _beginthreadex does indeed set errno. */
      psocSetError( pError, PSOC_ERRNO_HANDLE, errno );
      return -1;
   }
   pThread->hThread = handle;
   
#else
   errcode = pthread_create ( &pThread->threadId, 
                              NULL, psotStartRoutine, (void*)pThread );

   if ( errcode != 0 ) {
      if ( errcode > 0 ) {
         psocSetError( pError, PSOC_ERRNO_HANDLE, errcode );
      }
      else {
         psocSetError( pError, PSOC_ERRNO_HANDLE, errno );
      }
      return -1;
   }
#endif

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psotJoinThread( psotThreadWrap* pThread, 
//                    void* retValue,
                    psocErrorHandler* pError )
{
#if defined (WIN32)
   DWORD err;

   err = WaitForSingleObject( pThread->hThread, INFINITE );
   if ( err == WAIT_OBJECT_0 ) {
//      if ( GetExitCodeThread( pThread->hThread, &value ) == TRUE ) {
         CloseHandle( pThread->hThread );
//         if ( retValue != NULL )
//            *(void *)retValue = value;
         return 0;
//      }
   }

   psocSetError( pError, PSOC_WINERR_HANDLE, GetLastError() );
   return -1;

#else
   int errcode;
   
   errcode = pthread_join( pThread->threadId, NULL ); // &retValue
   if ( errcode != 0 ) {
      if ( errcode > 0 ) {
         psocSetError( pError, PSOC_ERRNO_HANDLE, errcode );
      }
      else {
         psocSetError( pError, PSOC_ERRNO_HANDLE, errno );
      }
      return -1;
   }
   return 0;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

