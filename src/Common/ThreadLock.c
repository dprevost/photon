/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of photon (photonsoftware.org).
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

#include "ThreadLock.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool pscInitThreadLock( pscThreadLock* pLock )
{
#if defined (WIN32)
   BOOL status;
#else
   int rc;
#endif
   bool ok = false;
   
   PSO_PRE_CONDITION( pLock != NULL );

#if defined (WIN32)
   /*
    * InitializeCriticalSection does not return an error but might throw
    * an exception instead. For simplicity, using the other method was
    * chosen. Not sure about the spin count however.
    */
   status = InitializeCriticalSectionAndSpinCount( &pLock->mutex, 100 );
   if ( status == TRUE ) {
      ok = true;
      pLock->initialized = PSC_THREADLOCK_SIGNATURE;
   }
#else
   /*
    * Default attributes (passing NULL as the second parameter) should give
    * us a very simple mutex (not recursive and with minimal overhead) -
    * which is what we want (I think...).
    */
   rc = pthread_mutex_init( &pLock->mutex, NULL );
   if ( rc == 0 ) {
      pLock->initialized = PSC_THREADLOCK_SIGNATURE;
      ok = true;
   }
#endif
   
   return ok;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void pscFiniThreadLock( pscThreadLock* pLock )
{
#if ! defined(WIN32)
   int err = 0;
#endif

   PSO_PRE_CONDITION( pLock != NULL );
   PSO_INV_CONDITION( pLock->initialized == PSC_THREADLOCK_SIGNATURE );

#if defined (WIN32)
   DeleteCriticalSection( &pLock->mutex );
#else
   err = pthread_mutex_destroy( &pLock->mutex );
   
   PSO_POST_CONDITION( err == 0 );
#endif
   pLock->initialized = 0;
   
   return;
}
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

