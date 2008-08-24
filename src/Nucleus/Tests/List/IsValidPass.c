/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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
   psnLinkedList list;
   psnLinkNode node, node2;
   psnSessionContext context;
   int valid;
   
   initTest( expectedToPass, &context );
   InitMem();

   psnLinkNodeInit( &node );
   psnLinkNodeInit( &node2 );
   psnLinkedListInit( &list );
   
   psnLinkedListPutLast( &list, &node );
   
   valid = psnLinkedListIsValid( &list, &node );
   if ( ! valid ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   valid = psnLinkedListIsValid( &list, &node2 );
   if ( valid ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psnLinkedListFini( &list );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

