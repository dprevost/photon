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

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   psonLinkedList list;
   psonLinkNode node, node2;
   psonSessionContext context;
   int valid;
   
   initTest( expectedToPass, &context );
   InitMem();

   psonLinkNodeInit( &node );
   psonLinkNodeInit( &node2 );
   psonLinkedListInit( &list );
   
   psonLinkedListPutLast( &list, &node );
   
   valid = psonLinkedListIsValid( &list, &node );
   if ( ! valid ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   valid = psonLinkedListIsValid( &list, &node2 );
   if ( valid ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psonLinkedListFini( &list );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

