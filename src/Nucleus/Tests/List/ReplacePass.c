/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@photonsoftware.org>
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

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   psonLinkedList list;
   psonLinkNode oldNode, newNode, * pDummy = NULL;
   psonLinkNode node1, node2;
   psonSessionContext context;
   
   initTest( expectedToPass, &context );
   InitMem();
   
   psonLinkNodeInit( &oldNode );
   psonLinkNodeInit( &newNode );
   psonLinkNodeInit( &node1 );
   psonLinkNodeInit( &node2 );

   psonLinkedListInit( &list );

   /* Test 1 - replace alone */
   psonLinkedListPutFirst( &list, &oldNode );

   psonLinkedListReplaceItem( &list, &oldNode, &newNode );

   if ( list.currentSize != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psonLinkedListPeakFirst( &list, &pDummy );
   if ( pDummy != &newNode ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   if ( TestList( &list ) != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Test 2 - replace at tail */
   psonLinkedListPutFirst( &list, &node1 );
   psonLinkNodeInit( &oldNode );
   psonLinkedListReplaceItem( &list, &newNode, &oldNode );

   if ( list.currentSize != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psonLinkedListPeakLast( &list, &pDummy );
   if ( pDummy != &oldNode ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( TestList( &list ) != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Test 3 - replace in the middle */
   psonLinkedListPutLast( &list, &node2 );
   psonLinkNodeInit( &newNode );
   psonLinkedListReplaceItem( &list, &oldNode, &newNode );

   if ( list.currentSize != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psonLinkedListPeakNext( &list, &node1, &pDummy );
   if ( pDummy != &newNode ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( TestList( &list ) != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Test 4 - replace at head */
   psonLinkNodeInit( &oldNode );
   psonLinkedListReplaceItem( &list, &node1, &oldNode );

   if ( list.currentSize != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psonLinkedListPeakFirst( &list, &pDummy );
   if ( pDummy != &oldNode ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( TestList( &list ) != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psonLinkedListFini( &list );
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

