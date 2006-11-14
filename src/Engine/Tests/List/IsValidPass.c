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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char* argv[] )
{
   vdseLinkedList list;
   vdseLinkNode node, node2;
   int valid;
   
   if ( InitMem() == -1 )
      return -1;

   vdseLinkNodeInit( &node );
   vdseLinkNodeInit( &node2 );
   vdseLinkedListInit( &list, &g_alloc );
   
   vdseLinkedListPutLast( &list, &node, &g_alloc );
   
   valid = vdseLinkedListIsValid( &list, &node, &g_alloc );
   if ( ! valid ) return -1;

   valid = vdseLinkedListIsValid( &list, &node2, &g_alloc );
   if ( valid ) return -1;

   vdseLinkedListFini( &list );

   return 0;
}
