/*
 * Copyright (C) 2006-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#include "ListTestCommon.h"
#include "Nucleus/Tests/EngineTestCommon.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * This test will do the following:
 *
 * - create and populate a list with X elements (nodes)
 * - loop for Y times:
 *    - randomly put/get/remove elements from the list
 *    - periodically iterate over the whole list (first, next).
 * - examine the list at each step to make sure it is consistent
 *
 * The elements will be defined in a dummy struct and the offsetof
 * macro will be used to get the real pointer, just like we do in
 * the engine code.
 *
 * This test is the smaller version of this test. There are more checks
 * to insure consistency of the list at every step.
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
gf after fini -> assert
*/
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define MAX_ELEMENTS      20000
#define INITIAL_LIST_SIZE (MAX_ELEMENTS/2)
#define MAX_LOOP          10000
#define GET_NEXT_LOOP     (MAX_LOOP/1)

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   psonLinkedList list;
   dummyStruct dummy[MAX_ELEMENTS];
   int i, j, k;
   int randElement;
   unsigned int randAction;
   int errcode, countNext;
   psonSessionContext context;
   bool ok;
   
   int numInList;
   psonLinkNode* pNode = NULL;
   dummyStruct*  pDummy;
   
   initTest( &context );
   InitMem();

   /* Initialize the array of dummy structs */
   memset( dummy, 0, sizeof(dummyStruct)*MAX_ELEMENTS );
   
   for ( i = 0; i < MAX_ELEMENTS; ++i ) {
      dummy[i].dummy1 = i;
      psonLinkNodeInit( &dummy[i].node );
   }

   /* Initialize the list */
   psonLinkedListInit( &list );

   for ( i = 0; i < INITIAL_LIST_SIZE; ++i ) {
      psonLinkNodeInit( &dummy[i].node );
      psonLinkedListPutLast( &list, &dummy[i].node );
      dummy[i].isInUse = 1;
   }
   numInList = INITIAL_LIST_SIZE;

   assert_true( TestList( &list ) == 0 );
   
   /* Initialize the random generator */
   mysrand( 0x37bb05 );
   
   /* Start the real work */
   for ( i = 0; i < MAX_LOOP; ++i ) {
      randAction =  (int) (6.0*myrand()/(MY_RAND_MAX+1.0));

      randElement = (int) ((MAX_ELEMENTS*myrand()*1.0)/(MY_RAND_MAX+1.0));
      
      k = randElement;
      
      switch ( randAction ) {
      case 0:
         /* Find a random element to insert */
         for ( j = randElement+1; j < MAX_ELEMENTS; ++j ) {
            if ( dummy[j].isInUse == 0 ) {
               k = j;
               break;
            }
         }
         if ( k == randElement ) {
            for ( j = 0; j < randElement; ++j ) {
               if ( dummy[j].isInUse == 0 ) {
                  k = j;
                  break;
               }
            }
         }
         assert_false( k == randElement );
         
         assert_true( dummy[k].isInUse == 0 );
         
         psonLinkNodeInit( &dummy[k].node );
         psonLinkedListPutFirst( &list, &dummy[k].node );

         dummy[k].isInUse = 1;
         numInList++;
         break;
         
         /* Do it twice often to equilibrate the # of insert and remove */
      case 1:
      case 5:         
         /* Find a random element to insert */
         for ( j = randElement+1; j < MAX_ELEMENTS; ++j ) {
            if ( dummy[j].isInUse == 0 ) {
               k = j;
               break;
            }
         }
         if ( k == randElement ) {
            for ( j = 0; j < randElement; ++j ) {
               if ( dummy[j].isInUse == 0 ) {
                  k = j;
                  break;
               }
            }
         }
         assert_false( k == randElement );
         
         assert_true( dummy[k].isInUse == 0 );

         psonLinkNodeInit( &dummy[k].node );
         psonLinkedListPutLast( &list, &dummy[k].node );

         dummy[k].isInUse = 1;
         numInList++;
         break;
         
      case 2:
         ok = psonLinkedListGetFirst( &list, &pNode );
         assert_true( ok );
         
         pDummy = (dummyStruct* )
            ((char*)pNode - offsetof(dummyStruct, node ));

         for ( j = 0; j < MAX_ELEMENTS; ++j ) {
            if ( pDummy == &dummy[j] ) break;
         }
         assert_false( j == MAX_ELEMENTS );
         
         assert_true( pDummy->isInUse == 1 );
         
         pDummy->isInUse = 0;
         numInList--;
         break;
         
      case 3:
         ok = psonLinkedListGetLast( &list, &pNode );
         assert_true( ok );
         
         pDummy = (dummyStruct* )
            ((char*)pNode - offsetof(dummyStruct, node ));

         for ( j = 0; j < MAX_ELEMENTS; ++j ) {
            if ( pDummy == &dummy[j] ) break;
         }
         assert_false( j == MAX_ELEMENTS );
         
         assert_true( pDummy->isInUse == 1 );

         pDummy->isInUse = 0;
         numInList--;
         break;
         
      case 4:
         /* Find a random element to remove */
         for ( j = randElement+1; j < MAX_ELEMENTS; ++j ) {
            if ( dummy[j].isInUse == 1 ) {
               k = j;
               break;
            }
         }
         if ( k == randElement ) {
            for ( j = 0; j < randElement; ++j ) {
               if ( dummy[j].isInUse == 1 ) {
                  k = j;
                  break;
               }
            }
         }
         assert_false( k == randElement );
         
         assert_true( dummy[k].isInUse == 1 );

         psonLinkedListRemoveItem( &list, &dummy[k].node );

         dummy[k].isInUse = 0;         
         numInList--;
         break;
         
      } /* End switch statement */

      assert_true( (size_t)numInList == list.currentSize );
      
      for ( j = 0, k = 0; j < MAX_ELEMENTS; ++j ) {
         assert_true( dummy[j].isInUse == 0 || dummy[j].isInUse == 1 );
         
         if ( dummy[j].isInUse ) k++;
      }
      assert_true( k == numInList );
      
      errcode = TestList( &list );
      assert_true( errcode == 0 );
      
      /* Test the iterators */
      if ( ((i+1)%GET_NEXT_LOOP ) == 0 ) {
         ok = psonLinkedListPeakFirst( &list, &pNode );
            
         countNext = 1;
         while ( ok ) {
            ok = psonLinkedListPeakNext( &list, pNode, &pNode );
            if ( ok ) countNext++;
         }

         assert_true( countNext == numInList );
         
         ok = psonLinkedListPeakLast( &list, &pNode );
         
         countNext = 1;
         while ( ok ) {
            ok = psonLinkedListPeakPrevious( &list, 
                                             pNode, 
                                             &pNode );
            if ( ok ) countNext++;
         }

         assert_true( countNext == numInList );

      } /* End of if ((i+1)%GET_NEXT_LOOP ) == 0 */

   } /* End of for loop */

   psonLinkedListFini( &list );

#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_pass, setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

