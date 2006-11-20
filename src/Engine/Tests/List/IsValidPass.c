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
   vdseLinkNode node, node2;
   int valid;
   
   initTest( true );
   InitMem();

   vdseLinkNodeInit( &node );
   vdseLinkNodeInit( &node2 );
   vdseLinkedListInit( &list );
   
   vdseLinkedListPutLast( &list, &node );
   
   valid = vdseLinkedListIsValid( &list, &node );
   if ( ! valid ) return -1;

   valid = vdseLinkedListIsValid( &list, &node2 );
   if ( valid ) return -1;

   vdseLinkedListFini( &list );

   return 0;
}