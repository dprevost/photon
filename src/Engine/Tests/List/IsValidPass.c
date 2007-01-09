/*
 * Copyright (C) 2006, 2007 Daniel Prevost <dprevost@users.sourceforge.net>
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
#include "EngineTestCommon.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   vdseLinkedList list;
   vdseLinkNode node, node2;
   int valid;
   
   initTest( expectedToPass );
   InitMem();

   vdseLinkNodeInit( &node );
   vdseLinkNodeInit( &node2 );
   vdseLinkedListInit( &list );
   
   vdseLinkedListPutLast( &list, &node );
   
   valid = vdseLinkedListIsValid( &list, &node );
   if ( ! valid ) ERROR_EXIT( expectedToPass, NULL, ; );

   valid = vdseLinkedListIsValid( &list, &node2 );
   if ( valid ) ERROR_EXIT( expectedToPass, NULL, ; );

   vdseLinkedListFini( &list );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

