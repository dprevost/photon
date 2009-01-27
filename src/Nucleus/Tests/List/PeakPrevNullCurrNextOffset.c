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

const bool expectedToPass = false;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
#if defined(USE_DBC)
   psonLinkedList list;
   psonLinkNode node1, node2, *pNode;
   psonSessionContext context;
   bool ok;
   
   initTest( expectedToPass, &context );
   InitMem();
   
   psonLinkNodeInit( &node1 );
   psonLinkNodeInit( &node2 );
   psonLinkedListInit( &list );
   
   psonLinkedListPutLast( &list, &node1 );
   psonLinkedListPutLast( &list, &node2 );

   ok = psonLinkedListPeakLast( &list, &pNode );
   if ( ! ok ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   pNode->nextOffset = PSON_NULL_OFFSET;
   
   ok = psonLinkedListPeakPrevious( &list, pNode, &pNode );

   ERROR_EXIT( expectedToPass, NULL, ; );
#else
   return 1;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

