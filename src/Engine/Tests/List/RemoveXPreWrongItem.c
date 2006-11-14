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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char* argv[] )
{
   vdseLinkedList list1;
   vdseLinkedList list2;
   vdseLinkNode node1, node2;
   
   if ( InitMem() == -1 )
      return 0;
   
   vdseLinkNodeInit( &node1 );
   vdseLinkNodeInit( &node2 );
   vdseLinkedListInit( &list1, &g_alloc );
   vdseLinkedListInit( &list2, &g_alloc );
   
   vdseLinkedListPutLast( &list1, &node1, &g_alloc );
   vdseLinkedListPutLast( &list2, &node2, &g_alloc );

   /* Remove it from the wrong list */
   vdseLinkedListRemoveItem( &list1, &node2, &g_alloc );

   return 0;
}
