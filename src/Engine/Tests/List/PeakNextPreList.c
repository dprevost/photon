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
#include "EngineTestCommon.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char* argv[] )
{
   vdseLinkedList list;
   vdseLinkNode node1, node2, *pNode;
   enum ListErrors error;
   
   initTest( false );
   InitMem();
   
   vdseLinkNodeInit( &node1 );
   vdseLinkNodeInit( &node2 );
   vdseLinkedListInit( &list );
   
   vdseLinkedListPutLast( &list, &node1 );
   vdseLinkedListPutLast( &list, &node2 );

   error = vdseLinkedListPeakNext( NULL, &node1, &pNode );

   return 0;
}
