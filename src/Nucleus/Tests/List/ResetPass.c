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
   psonLinkNode* pNode = NULL;
   bool ok;
   psonSessionContext context;
  
   initTest( expectedToPass, &context );
   InitMem();
   
   psonLinkedListInit( &list );
   
   psonLinkedListReset( &list );

   if ( list.initialized != PSON_LIST_SIGNATURE ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( list.currentSize != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   ok = psonLinkedListGetFirst( &list, &pNode );
   if ( ok ) { /* The list is not empty after a reset... problem! */
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psonLinkedListFini( &list );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

