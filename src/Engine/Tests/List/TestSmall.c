/*
 * Copyright (C) 2006 Daniel Prevost <dprevost@users.sourceforge.net>
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

int main( int argc, char* argv[] )
{
   vdseLinkedList list;
   dummyStruct dummy[MAX_ELEMENTS];
   int i, j, k;
   int randElement;
   unsigned int randAction;
   enum ListErrors error;
   int errcode, countNext;
   
   int numInList;
   vdseLinkNode* pNode;
   dummyStruct*  pDummy;
   
   initTest( true );
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
      return -1;
   
   /* Initialize the random generator */
   mysrand( 0x37bb05 );
   
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
            return -1;
         }
         
         if ( dummy[k].isInUse != 0 )
         {
            fprintf( stderr, "Case 0, wrong isInUse value )\n" );
            return -1;
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
            return -1;
         }
         
         if ( dummy[k].isInUse != 0 )
         {
            fprintf( stderr, "Case 1 or 5, wrong isInUse value )\n" );
            return -1;
         }

         vdseLinkedListPutLast( &list, &dummy[k].node );

         dummy[k].isInUse = 1;
         numInList++;
         break;
         
      case 2:
         error = vdseLinkedListGetFirst( &list, &pNode );
         if ( error == LIST_INTERNAL_ERROR ) 
            return -1;

         pDummy = (dummyStruct* )
            ((char*)pNode - offsetof(dummyStruct, node ));

         for ( j = 0; j < MAX_ELEMENTS; ++j )
            if ( pDummy == &dummy[j] ) 
               break;
         if ( j == MAX_ELEMENTS )
         {
            fprintf( stderr, "Case 2, retrieve node ptr is invalid\n" );
            return -1;
         }
         
         if ( pDummy->isInUse != 1 )
         {
            fprintf( stderr, "Case 2, wrong isInUse value )\n" );
            return -1;
         }
         
         pDummy->isInUse = 0;
         numInList--;
         break;
         
      case 3:
         error = vdseLinkedListGetLast( &list, &pNode );
         if ( error == LIST_INTERNAL_ERROR ) 
            return -1;

         pDummy = (dummyStruct* )
            ((char*)pNode - offsetof(dummyStruct, node ));

         for ( j = 0; j < MAX_ELEMENTS; ++j )
            if ( pDummy == &dummy[j] ) 
               break;
         if ( j == MAX_ELEMENTS )
         {
            fprintf( stderr, "Case 3, retrieve node ptr is invalid\n" );
            return -1;
         }
         
         if ( pDummy->isInUse != 1 )
         {
            fprintf( stderr, "Case 3, wrong isInUse value )\n" );
            return -1;
         }

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
            return -1;
         }
         
         if ( dummy[k].isInUse != 1 )
         {
            fprintf( stderr, "Case 4, wrong isInUse value )\n" );
            return -1;
         }

         vdseLinkedListRemoveItem( &list, &dummy[k].node );

         dummy[k].isInUse = 0;         
         numInList--;
         break;
         
      } /* End switch statement */

      if ( numInList != list.currentSize )
      {
         fprintf( stderr, "Discrepency in list size (%d %d), action = %d\n", 
                  numInList, list.currentSize, randAction%6 );
         return -1;
      }
      
      for ( j = 0, k = 0; j < MAX_ELEMENTS; ++j )
      {
         if ( dummy[j].isInUse != 0 && dummy[j].isInUse != 1 )
         {
            fprintf( stderr, "Invalid isInUse value, action = %d\n", 
                     randAction%6 );
            return -1;
         }
         
         if ( dummy[j].isInUse ) k++;
      }
      if ( k != numInList )
      {
         fprintf( stderr, "Discrepency in sum of isInUse and list size\n" );
         fprintf( stderr, "isInUse sum = %d, list size = %d, action = %d\n",
                  k, numInList, randAction%6 );
         return -1;
      }
      
      errcode = TestList( &list );
      if ( errcode != 0 )
      {
         fprintf( stderr, "TestList failed, error = %d, action = %d\n", 
                  errcode, randAction%6 );
         return -1;
      }
      
      /* Test the iterators */
      if ( ((i+1)%GET_NEXT_LOOP ) == 0 ) 
      {
         error = vdseLinkedListPeakFirst( &list, &pNode );
         if ( error != LIST_OK && error != LIST_EMPTY )
            return -1;

         countNext = 1;
         while ( error == LIST_OK )
         {
            error = vdseLinkedListPeakNext( &list, pNode, &pNode );
            if ( error == LIST_INTERNAL_ERROR ) 
               return -1;
            if ( error == VDS_OK ) countNext++;
         }

         if ( countNext != numInList )
         {
            fprintf( stderr, "Countnext is wrong in First/Next loop\n" );
            return -1;
         }
         
         error = vdseLinkedListPeakLast( &list, &pNode );
         if ( error != LIST_OK && error != LIST_EMPTY )
            return -1;

         countNext = 1;
         while ( error == LIST_OK )
         {
            error = vdseLinkedListPeakPrevious( &list, 
                                                pNode, 
                                                &pNode );
            if ( error == LIST_INTERNAL_ERROR ) 
               return -1;
            if ( error == VDS_OK ) countNext++;
         }

         if ( countNext != numInList )
         {
            fprintf( stderr, "Countnext is wrong in Last/Previous loop\n" );
            return -1;
         }

      } /* End of if ((i+1)%GET_NEXT_LOOP ) == 0 */

   } /* End of for loop */

   vdseLinkedListFini( &list );

   return 0;
}

