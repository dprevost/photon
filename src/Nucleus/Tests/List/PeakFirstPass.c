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

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   psonLinkedList list;
   psonLinkNode node, *pNode;
   psonSessionContext context;
   bool ok;
   
   initTest( expectedToPass, &context );
   InitMem();
   
   psonLinkNodeInit( &node );
   psonLinkedListInit( &list );
   
   psonLinkedListPutLast( &list, &node );
   
   ok = psonLinkedListPeakFirst( &list, &pNode );
   if ( ! ok ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pNode != &node ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( list.currentSize != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psonLinkedListFini( &list );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

