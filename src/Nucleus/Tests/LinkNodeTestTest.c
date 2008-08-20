/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of photon (photonsoftware.org).
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

#include "Nucleus/LinkNode.h"
#include "Tests/PrintError.h"

const bool expectedToPass = false;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   bool ok;
   psnLinkNode node;

   psnLinkNodeInit( &node );

   ok = psnLinkNodeTest( &node );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   node.nextOffset = 0x1234;
   ok = psnLinkNodeTest( &node );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psnLinkNodeInit( &node );

   node.previousOffset = 0x1234;
   ok = psnLinkNodeTest( &node );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   node.nextOffset = 0x1234;
   ok = psnLinkNodeTest( &node );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
