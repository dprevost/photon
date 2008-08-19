/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of photon (photonsoftware.org).
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

int main()
{
#if defined(USE_DBC)
   psnLinkedList list;
   psnLinkNode node1, node2, *pNode;
   psnSessionContext context;
   bool ok;
   
   initTest( expectedToPass, &context );
   InitMem();
   
   psnLinkNodeInit( &node1 );
   psnLinkNodeInit( &node2 );
   psnLinkedListInit( &list );
   
   psnLinkedListPutLast( &list, &node1 );
   psnLinkedListPutLast( &list, &node2 );

   ok = psnLinkedListPeakLast( &list, &pNode );
   if ( ! ok ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   pNode->nextOffset = PSN_NULL_OFFSET;
   
   ok = psnLinkedListPeakPrevious( &list, pNode, &pNode );

   ERROR_EXIT( expectedToPass, NULL, ; );
#else
   return 1;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

