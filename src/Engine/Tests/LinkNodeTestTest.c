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

#include "LinkNode.h"
#include "PrintError.h"

const bool expectedToPass = false;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char* argv[] )
{
   int errcode, rc;
   vdseLinkNode node;

   vdseLinkNodeInit( &node );

   rc = vdseLinkNodeTest( &node );
   if ( rc != 0 )
      ERROR_EXIT( expectedToPass, NULL, );
   
   node.nextOffset = 0x1234;
   rc = vdseLinkNodeTest( &node );
   if ( rc != 0 )
      ERROR_EXIT( expectedToPass, NULL, );

   vdseLinkNodeInit( &node );

   node.previousOffset = 0x1234;
   rc = vdseLinkNodeTest( &node );
   if ( rc != 0 )
      ERROR_EXIT( expectedToPass, NULL, );
   
   node.nextOffset = 0x1234;
   rc = vdseLinkNodeTest( &node );
   if ( rc == 0 )
      ERROR_EXIT( expectedToPass, NULL, );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

