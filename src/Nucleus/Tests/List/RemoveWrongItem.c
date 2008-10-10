/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@photonsoftware.org>
 *
 * This file is part of Photon (photonsoftware.org).
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
 * well (the call psonLinkedListRemoveItem() does remove the link from its
 * linked list since it uses the previous and next members of the removed
 * node.
 */
int main()
{
#if defined(USE_DBC)
   psonLinkedList list1;
   psonLinkedList list2;
   psonLinkNode node1, node2;
   psonSessionContext context;
   
   initTest( expectedToPass, &context );
   InitMem();
   
   psonLinkNodeInit( &node1 );
   psonLinkNodeInit( &node2 );
   psonLinkedListInit( &list1 );
   psonLinkedListInit( &list2 );
   
   psonLinkedListPutLast( &list1, &node1 );
   psonLinkedListPutLast( &list2, &node2 );

   /* Remove it from the wrong list - should work but... */
   psonLinkedListRemoveItem( &list2, &node1 );

   /* Should crash since the number of items in list2 is zero */
   psonLinkedListRemoveItem( &list2, &node2 );

   ERROR_EXIT( expectedToPass, NULL, ; );
#else
   return 1;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

