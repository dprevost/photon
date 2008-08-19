/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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
#include "Nucleus/Tests/EngineTestCommon.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   psnLinkedList list;
   psnLinkNode oldNode, newNode, * pDummy = NULL;
   psnLinkNode node1, node2;
   psnSessionContext context;
   
   initTest( expectedToPass, &context );
   InitMem();
   
   psnLinkNodeInit( &oldNode );
   psnLinkNodeInit( &newNode );
   psnLinkNodeInit( &node1 );
   psnLinkNodeInit( &node2 );

   psnLinkedListInit( &list );

   /* Test 1 - replace alone */
   psnLinkedListPutFirst( &list, &oldNode );

   psnLinkedListReplaceItem( &list, &oldNode, &newNode );

   if ( list.currentSize != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psnLinkedListPeakFirst( &list, &pDummy );
   if ( pDummy != &newNode ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   if ( TestList( &list ) != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Test 2 - replace at tail */
   psnLinkedListPutFirst( &list, &node1 );
   psnLinkNodeInit( &oldNode );
   psnLinkedListReplaceItem( &list, &newNode, &oldNode );

   if ( list.currentSize != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psnLinkedListPeakLast( &list, &pDummy );
   if ( pDummy != &oldNode ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( TestList( &list ) != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Test 3 - replace in the middle */
   psnLinkedListPutLast( &list, &node2 );
   psnLinkNodeInit( &newNode );
   psnLinkedListReplaceItem( &list, &oldNode, &newNode );

   if ( list.currentSize != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psnLinkedListPeakNext( &list, &node1, &pDummy );
   if ( pDummy != &newNode ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( TestList( &list ) != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Test 4 - replace at head */
   psnLinkNodeInit( &oldNode );
   psnLinkedListReplaceItem( &list, &node1, &oldNode );

   if ( list.currentSize != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psnLinkedListPeakFirst( &list, &pDummy );
   if ( pDummy != &oldNode ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( TestList( &list ) != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psnLinkedListFini( &list );
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

