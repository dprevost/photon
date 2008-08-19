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

const bool expectedToPass = false;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * The following test is pretty weak. Removing an item from the wrong list
 * has one consequence - the list counter becomes wrong. Otherwise all is
 * well (the call psnLinkedListRemoveItem() does remove the link from its
 * linked list since it uses the previous and next members of the removed
 * node.
 */
int main()
{
#if defined(USE_DBC)
   psnLinkedList list1;
   psnLinkedList list2;
   psnLinkNode node1, node2;
   psnSessionContext context;
   
   initTest( expectedToPass, &context );
   InitMem();
   
   psnLinkNodeInit( &node1 );
   psnLinkNodeInit( &node2 );
   psnLinkedListInit( &list1 );
   psnLinkedListInit( &list2 );
   
   psnLinkedListPutLast( &list1, &node1 );
   psnLinkedListPutLast( &list2, &node2 );

   /* Remove it from the wrong list - should work but... */
   psnLinkedListRemoveItem( &list2, &node1 );

   /* Should crash since the number of items in list2 is zero */
   psnLinkedListRemoveItem( &list2, &node2 );

   ERROR_EXIT( expectedToPass, NULL, ; );
#else
   return 1;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

