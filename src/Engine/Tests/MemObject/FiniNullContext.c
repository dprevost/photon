/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#include "Engine/MemoryObject.h"
#include "MemObjTest.h"

const bool expectedToPass = false;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   vdsErrors errcode;
   vdstObjDummy *pDummy;
   vdseSessionContext context;
   
   pDummy = initMemObjTest( expectedToPass, &context );

   errcode = vdseMemObjectInit( &pDummy->memObject,
                                VDSE_IDENT_ALLOCATOR,
                                &pDummy->blockGroup,
                                4 );
   if ( errcode != VDS_OK ) 
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   /* Should crash at this point */
   vdseMemObjectFini( &pDummy->memObject, VDSE_ALLOC_ANY, NULL );
   
   ERROR_EXIT( expectedToPass, NULL, ; );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

