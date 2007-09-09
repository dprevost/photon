/*
 * Copyright (C) 2006-2007 Daniel Prevost <dprevost@users.sourceforge.net>
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
#include "Engine/Tests/EngineTestCommon.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   vdseLinkedList list;
   vdseLinkNode oldNode, newNode, * pDummy = NULL;
   vdseLinkNode node1, node2;
   
   initTest( expectedToPass );
   InitMem();
   
   vdseLinkNodeInit( &oldNode );
   vdseLinkNodeInit( &newNode );
   vdseLinkNodeInit( &node1 );
   vdseLinkNodeInit( &node2 );

   vdseLinkedListInit( &list );

   /* Test 1 - replace alone */
   vdseLinkedListPutFirst( &list, &oldNode );

   vdseLinkedListReplaceItem( &list, &oldNode, &newNode );

   if ( list.currentSize != 1 ) 
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   vdseLinkedListPeakFirst( &list, &pDummy );
   if ( pDummy != &newNode ) 
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   if ( TestList( &list ) != 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );

   /* Test 2 - replace at tail */
   vdseLinkedListPutFirst( &list, &node1 );
   vdseLinkedListReplaceItem( &list, &newNode, &oldNode );

   if ( list.currentSize != 2 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   vdseLinkedListPeakLast( &list, &pDummy );
   if ( pDummy != &oldNode )
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   if ( TestList( &list ) != 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   /* Test 3 - replace in the middle */
   vdseLinkedListPutLast( &list, &node2 );
   vdseLinkedListReplaceItem( &list, &oldNode, &newNode );

   if ( list.currentSize != 3 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   vdseLinkedListPeakNext( &list, &node1, &pDummy );
   if ( pDummy != &newNode )
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   if ( TestList( &list ) != 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );

   /* Test 4 - replace at head */
   vdseLinkedListReplaceItem( &list, &node1, &oldNode );

   if ( list.currentSize != 3 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   vdseLinkedListPeakFirst( &list, &pDummy );
   if ( pDummy != &oldNode )
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   if ( TestList( &list ) != 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   vdseLinkedListFini( &list );
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

