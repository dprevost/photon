/*
 * Copyright (C) 2008-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#include "Nucleus/MemoryObject.h"
#include "MemObjTest.h"
#include <photon/psoCommon.h>

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   psoErrors errcode;
   psotObjDummy *pDummy;
   psonSessionContext context;
   bool ok;
   /** our current list of locks */
   ptrdiff_t lockOffsets[PSON_MAX_LOCK_DEPTH];
   /** number of locks we are holding */
   int numLocks = 0;
   
   pDummy = initMemObjTest( expectedToPass, &context );
   context.lockOffsets = lockOffsets;
   context.numLocks = &numLocks;
   
   errcode = psonMemObjectInit( &pDummy->memObject, 
                                PSON_IDENT_FOLDER,
                                &pDummy->blockGroup,
                                1 );
   if ( errcode != PSO_OK ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /*
    * We acquire the lock without going through the sessioncontext
    * (without using psonLock()).
    */
   ok = psocTryAcquireProcessLock ( &pDummy->memObject.lock,
                                    context.pidLocker,
                                    PSON_LOCK_TIMEOUT );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDummy->memObject.totalBlocks = 3;
   context.pidLocker++;
   ok = psonLock( &pDummy->memObject, &context );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   ok = psonLock( &pDummy->memObject, &context );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   ok = psonLock( &pDummy->memObject, &context );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   ok = psonLock( &pDummy->memObject, &context );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
fprintf( stderr, "%d %d\n", PSON_MAX_LOCK_DEPTH, numLocks );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

