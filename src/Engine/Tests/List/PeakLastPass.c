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
   vdseLinkNode node, *pNode;
   enum ListErrors error;
   
   initTest( true );
   InitMem();
   
   vdseLinkNodeInit( &node );
   vdseLinkedListInit( &list );
   
   vdseLinkedListPutLast( &list, &node );
   
   error = vdseLinkedListPeakLast( &list, &pNode );
   if ( error != LIST_OK )
      return -1;
   if ( pNode != &node ) 
      return -1;
   if ( list.currentSize != 1 ) return -1;

   vdseLinkedListFini( &list );

   return 0;
}
