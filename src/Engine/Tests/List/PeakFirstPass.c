/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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
#include "Engine/Tests/EngineTestCommon.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   vdseLinkedList list;
   vdseLinkNode node, *pNode;
   enum ListErrors error;
   vdseSessionContext context;
   
   initTest( expectedToPass, &context );
   InitMem();
   
   vdseLinkNodeInit( &node );
   vdseLinkedListInit( &list );
   
   vdseLinkedListPutLast( &list, &node );
   
   error = vdseLinkedListPeakFirst( &list, &pNode );
   if ( error != LIST_OK ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pNode != &node ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( list.currentSize != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   vdseLinkedListFini( &list );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

