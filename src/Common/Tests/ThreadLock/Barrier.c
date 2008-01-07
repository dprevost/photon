/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#include "Common/Tests/ThreadLock/Barrier.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*!
 * 
 * \param[in] pBarrier A pointer to the vdstBarrier struct itself.
 * \param[in] numThreads The number of threads to synchronize.
 * \param[in] pError     A pointer to a vdscErrorHandler struct
 *
 * \retval  0 on success
 * \retval -1 on error (use pError to retrieve the error)
 *
 * \pre \em pBarrier cannot be NULL.
 * \pre \em numThreads must be greater than zero.
 * \pre \em pError cannot be NULL.
 *
 */
int vdstInitBarrier( vdstBarrier *     pBarrier, 
                     int               numThreads,
                     vdscErrorHandler* pError )
{
#if defined (WIN32)
   BOOL status;
#else
   int status;      
#endif

   VDS_PRE_CONDITION( pBarrier != NULL );
   VDS_PRE_CONDITION( numThreads > 0 );
   VDS_PRE_CONDITION( pError != NULL );
   
#if defined ( WIN32 )
   /*
    * InitializeCriticalSection does not return an error but might throw
    * an exception instead. For simplicity, using the other method was
    * chosen. Not sure about the spin count however.
    */
   status = InitializeCriticalSectionAndSpinCount( 
      &pBarrier->subBarrier[0].waitLock, 100 );
   if ( status == FALSE ) goto end_on_error;
   status = InitializeCriticalSectionAndSpinCount( 
      &pBarrier->subBarrier[1].waitLock, 100 );
   if ( status == FALSE ) goto end_on_error;
   
   /*
    * HANDLE WINAPI CreateEvent(
    *    LPSECURITY_ATTRIBUTES lpEventAttributes,
    *    BOOL bManualReset,
    *    BOOL bInitialState,
    *    LPCTSTR lpName );
    *
    * The manual reset must be set to TRUE and the initial state
    * to "not signaled" (FALSE)
    */
   pBarrier->subBarrier[0].waitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
   pBarrier->subBarrier[1].waitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

   if ( pBarrier->subBarrier[0].waitEvent == NULL ) goto end_on_error;
   if ( pBarrier->subBarrier[1].waitEvent == NULL ) goto end_on_error;
#else
   status = pthread_mutex_init( &pBarrier->subBarrier[0].waitLock, NULL );
   if ( status != 0 ) goto end_on_error;
   status = pthread_mutex_init( &pBarrier->subBarrier[1].waitLock, NULL );
   if ( status != 0 ) goto end_on_error;
   status = pthread_cond_init( &pBarrier->subBarrier[0].waitVar, NULL );
   if ( status != 0 ) goto end_on_error;
   status = pthread_cond_init( &pBarrier->subBarrier[1].waitVar, NULL );
   if ( status != 0 ) goto end_on_error;
   
#endif

//   pBarrier->pCurrentSub = pBarrier->subBarrier[0];
   pBarrier->numThreads = numThreads;
   pBarrier->subBarrier[0].numRunners = numThreads;
   pBarrier->subBarrier[1].numRunners = numThreads;
   pBarrier->currentSubBarrier = 0;

   return 0;
   
end_on_error:

#if defined (WIN32)
   vdscSetError( pError, VDSC_WINERR_HANDLE, GetLastError() );
#else
   /* Some old versions of pthread used to returned -1 instead of
    * returning the error code. We check for this just in case.
    */
   if ( status > 0 )
      vdscSetError( pError, VDSC_ERRNO_HANDLE, status );
   else
      vdscSetError( pError, VDSC_ERRNO_HANDLE, errno );
#endif
   return -1;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*!
 * 
 * \param[in] pBarrier A pointer to the vdstBarrier struct itself.
 *
 * \pre \em pBarrier cannot be NULL.
 *
 * \post All the calls to destroy mutexes and similar must be successful.
 *
 */
 
void vdstFiniBarrier( vdstBarrier * pBarrier )
{
#if defined (WIN32)
   BOOL status;
#else
   int status;      
#endif

   VDS_PRE_CONDITION( pBarrier != NULL );

#if defined (WIN32)
   status = CloseHandle( pBarrier->subBarrier[0].waitEvent );
   VDS_POST_CONDITION( status != 0 );
   status = CloseHandle( pBarrier->subBarrier[1].waitEvent );
   VDS_POST_CONDITION( status != 0 );

   DeleteCriticalSection( &pBarrier->subBarrier[0].waitLock );
   DeleteCriticalSection( &pBarrier->subBarrier[1].waitLock );
#else
   status = pthread_cond_destroy( &pBarrier->subBarrier[0].waitVar );
   VDS_POST_CONDITION( status == 0 );
   status = pthread_cond_destroy( &pBarrier->subBarrier[1].waitVar );
   VDS_POST_CONDITION( status == 0 );
   status = pthread_mutex_destroy( &pBarrier->subBarrier[0].waitLock );
   VDS_POST_CONDITION( status == 0 );
   status = pthread_mutex_destroy( &pBarrier->subBarrier[1].waitLock );
   VDS_POST_CONDITION( status == 0 );
#endif

   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*!
 * 
 * \param[in] pBarrier A pointer to the vdstBarrier struct itself.
 *
 * \pre \em pBarrier cannot be NULL.
 *
 */

void vdstBarrierWait( vdstBarrier * pBarrier )
{
#if ! defined (WIN32)
   int status;      
#endif
   struct vdstSubBarrier* pCurrentSub;

   VDS_PRE_CONDITION( pBarrier != NULL );

   pCurrentSub = &pBarrier->subBarrier[pBarrier->currentSubBarrier];
#if defined (WIN32)
   EnterCriticalSection( &pCurrentSub->waitLock );
#else
   status = pthread_mutex_lock( &pCurrentSub->waitLock );
   VDS_POST_CONDITION( status == 0 );
#endif

   pCurrentSub->numRunners--;
#if defined (WIN32)
   LeaveCriticalSection( &pCurrentSub->waitLock );
#endif

   if ( pCurrentSub->numRunners == 0 )
   {
      /* All done, we're the last thread to reach the barrier */
      
      /* Reset everything */
      
      pCurrentSub->numRunners = pBarrier->numThreads;
      pBarrier->currentSubBarrier = 1 - pBarrier->currentSubBarrier;
      
      /* And go! */
#if defined (WIN32)
      /*
       * First make sure that the second subbarrier is in blocking
       * mode (by making sure that the event is not signaled). 
       * Then signal the current event (subbarrier).
       * Note: currentSubBarrier was modified - it does not point
       * to the current barrier anymore).
       */
      ResetEvent(pBarrier->subBarrier[pBarrier->currentSubBarrier].waitEvent);
      SetEvent( pBarrier->subBarrier[1-pBarrier->currentSubBarrier].waitEvent ); 
#else
      status = pthread_cond_broadcast( &pCurrentSub->waitVar );
      VDS_POST_CONDITION( status == 0 );
#endif
   }
   else
   {
#if defined (WIN32)
      WaitForSingleObject(pBarrier->subBarrier[pBarrier->currentSubBarrier].waitEvent, INFINITE);
#else
      while ( pCurrentSub->numRunners != pBarrier->numThreads )
         pthread_cond_wait( &pCurrentSub->waitVar, &pCurrentSub->waitLock );
#endif
   }

#if ! defined (WIN32)
   pthread_mutex_unlock( &pCurrentSub->waitLock );
#endif

   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

