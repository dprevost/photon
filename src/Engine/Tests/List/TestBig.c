/*
 * Copyright (C) 2006, 2007 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of vdsf (Virtual Data Space Framework).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation and appearing in the file COPYING included in the
 * packaging of this library.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "ListTestCommon.h"
#include "EngineTestCommon.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * This test will do the following:
 *
 * - create and populate a list with X elements (nodes)
 * - loop for Y times:
 *    - randomly put/get/remove elements from the list
 *    - periodically iterate over the whole list (first, next).
 * - examine the list at the end to make sure it is consistent
 *
 * The elements will be defined in a dummy struct and the offsetof
 * macro will be used to get the real pointer, just like we do in
 * the engine code.
 *
 * This test is the bigger version of this test. There are less checks
 * to insure consistency of the list. This enable us to make the
 * loop much larger.
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
gf after reset -> list empty
gf after fini -> assert
*/
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define MAX_ELEMENTS      20000
#define INITIAL_LIST_SIZE (MAX_ELEMENTS/2)
#define MAX_LOOP          10000000
#define GET_NEXT_LOOP     (MAX_LOOP/10)

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   vdseLinkedList list;
   dummyStruct dummy[MAX_ELEMENTS];
   int i, j, k, errcode;
   int randElement;
   unsigned int randAction;
   enum ListErrors error;
//   int jj, kk;
   
   int numInList;
   vdseLinkNode* pNode;
   dummyStruct*  pDummy;
   
   initTest( expectedToPass );
   InitMem();

   /* Initialize the array of dummy structs */
   memset( dummy, 0, sizeof(dummyStruct)*MAX_ELEMENTS );
   
   for ( i = 0; i < MAX_ELEMENTS; ++i )
   {
      dummy[i].dummy1 = i;
      vdseLinkNodeInit( &dummy[i].node );
   }

   /* Initialize the list */
   vdseLinkedListInit( &list );

   for ( i = 0; i < INITIAL_LIST_SIZE; ++i )
   {
      vdseLinkedListPutLast( &list, &dummy[i].node );
      dummy[i].isInUse = 1;
   }
   numInList = INITIAL_LIST_SIZE;

   if ( TestList( &list ) != 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   /* Initialize the random generator */
   mysrand( 0x130bc9 );
   
   /* Start the real work */
   for ( i = 0; i < MAX_LOOP; ++i )
   {
      randAction =  (int) (6.0*myrand()/(MY_RAND_MAX+1.0));

      randElement = (int) ((MAX_ELEMENTS*myrand()*1.0)/(MY_RAND_MAX+1.0));
      
      k = randElement;
      
      switch ( randAction )
      {
      case 0:
         
         /* Find a random element to insert */
         for ( j = randElement+1; j < MAX_ELEMENTS; ++j )
         {
            if ( dummy[j].isInUse == 0 )
            {
               k = j;
               break;
            }
         }
         if ( k == randElement )
         {
            for ( j = 0; j < randElement; ++j )
            {
               if ( dummy[j].isInUse == 0 )
               {
                  k = j;
                  break;
               }
            }
         }
         if ( k == randElement )
         {
            fprintf( stderr, "Case 0, did not found a free element\n" );
            ERROR_EXIT( expectedToPass, NULL, ; );
         }
         
         vdseLinkedListPutFirst( &list, &dummy[k].node );

         dummy[k].isInUse = 1;
         numInList++;
         break;
         
         /* Do it twice often to equilibrate the # of insert and remove */
      case 1:
      case 5:         
         /* Find a random element to insert */
         for ( j = randElement+1; j < MAX_ELEMENTS; ++j )
         {
            if ( dummy[j].isInUse == 0 )
            {
               k = j;
               break;
            }
         }         
         if ( k == randElement )
         {
            for ( j = 0; j < randElement; ++j )
            {
               if ( dummy[j].isInUse == 0 )
               {
                  k = j;
                  break;
               }
            }
         }
         if ( k == randElement )
         {
            fprintf( stderr, "Case 1 or 5, did not found a free element\n" );
            ERROR_EXIT( expectedToPass, NULL, ; );
         }

         vdseLinkedListPutLast( &list, &dummy[k].node );

         dummy[k].isInUse = 1;
         numInList++;
         break;
         
      case 2:
         error = vdseLinkedListGetFirst( &list, &pNode );
         if ( error == LIST_INTERNAL_ERROR ) 
            ERROR_EXIT( expectedToPass, NULL, ; );

         pDummy = (dummyStruct* )
            ((char*)pNode - offsetof(dummyStruct, node ));
         
         pDummy->isInUse = 0;
         numInList--;
         break;
         
      case 3:
         error = vdseLinkedListGetLast( &list, &pNode );
         if ( error == LIST_INTERNAL_ERROR ) 
            ERROR_EXIT( expectedToPass, NULL, ; );

         pDummy = (dummyStruct* )
            ((char*)pNode - offsetof(dummyStruct, node ));

         pDummy->isInUse = 0;
         numInList--;
         break;
         
      case 4:
         /* Find a random element to remove */
         for ( j = randElement+1; j < MAX_ELEMENTS; ++j )
         {
            if ( dummy[j].isInUse == 1 )
            {
               k = j;
               break;
            }
         }
         if ( k == randElement )
         {
            for ( j = 0; j < randElement; ++j )
            {
               if ( dummy[j].isInUse == 1 )
               {
                  k = j;
                  break;
               }
            }
         }
         if ( k == randElement )
         {
            fprintf( stderr, "Case 4, did not found a used element\n" );
            ERROR_EXIT( expectedToPass, NULL, ; );
         }

         vdseLinkedListRemoveItem( &list, &dummy[k].node );

         dummy[k].isInUse = 0;         
         numInList--;
         break;
         
      } /* End switch statement */

      if ( (size_t)numInList != list.currentSize )
      {
         fprintf( stderr, "Discrepency in list size (%d %d), action = %d\n", 
                  numInList, list.currentSize, randAction%6 );
         ERROR_EXIT( expectedToPass, NULL, ; );
      }

      /* Test the iterators */
      if ( ((i+1)%GET_NEXT_LOOP ) == 0 )
      {
         error = vdseLinkedListPeakFirst( &list, &pNode );
         if ( error == LIST_INTERNAL_ERROR ) 
            ERROR_EXIT( expectedToPass, NULL, ; );
         
         while ( error == LIST_OK )
         {
            error = vdseLinkedListPeakNext( &list, pNode, &pNode );
            if ( error == LIST_INTERNAL_ERROR ) 
               ERROR_EXIT( expectedToPass, NULL, ; );
         }

         error = vdseLinkedListPeakLast( &list, &pNode );
         if ( error == LIST_INTERNAL_ERROR ) 
            ERROR_EXIT( expectedToPass, NULL, ; );
         
         while ( error == LIST_OK )
         {
            error = vdseLinkedListPeakPrevious( &list, 
                                                pNode, 
                                                &pNode );
            if ( error == LIST_INTERNAL_ERROR ) 
               ERROR_EXIT( expectedToPass, NULL, ; );
         }
      } /* End of if ((i+1)%GET_NEXT_LOOP ) == 0 */

   } /* End of for loop */

   errcode = TestList( &list );
   if ( errcode != 0 )
   {
      fprintf( stderr, "TestList failed, error = %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   vdseLinkedListFini( &list );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

