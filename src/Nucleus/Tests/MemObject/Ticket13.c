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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   psoErrors errcode;
   psotObjDummy *pDummy;
   psonSessionContext context;
   bool ok;
   /** our current list of locks */
   ptrdiff_t lockOffsets[PSON_MAX_LOCK_DEPTH];
   /** number of locks we are holding */
   int numLocks = 0;
   
   pDummy = initMemObjTest( &context );
   context.lockOffsets = lockOffsets;
   context.numLocks = &numLocks;
   
   errcode = psonMemObjectInit( &pDummy->memObject, 
                                PSON_IDENT_FOLDER,
                                &pDummy->blockGroup,
                                1 );
   assert_true( errcode == PSO_OK );
   
   /*
    * We acquire the lock without going through the sessioncontext
    * (without using psonLock()).
    */
   ok = psocTryAcquireProcessLock ( &pDummy->memObject.lock,
                                    context.pidLocker,
                                    PSON_LOCK_TIMEOUT );
   assert_true( ok );

   pDummy->memObject.totalBlocks = 3;
   context.pidLocker++;
   ok = psonLock( &pDummy->memObject, &context );
   assert_false( ok );
   ok = psonLock( &pDummy->memObject, &context );
   assert_false( ok );
   ok = psonLock( &pDummy->memObject, &context );
   assert_false( ok );
   ok = psonLock( &pDummy->memObject, &context );
   assert_false( ok );
   
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test( test_pass )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

