/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of Photon (photonsoftware.org).
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

#include "Nucleus/MemoryObject.h"
#include "MemObjTest.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   psoErrors errcode;
   psotObjDummy *pDummy;
   psnSessionContext context;
   
   pDummy = initMemObjTest( expectedToPass, &context );
   
   errcode = psnMemObjectInit( &pDummy->memObject, 
                                PSN_IDENT_FOLDER,
                                &pDummy->blockGroup,
                                1 );
   if ( errcode != PSO_OK ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = psnMemObjectFini( &pDummy->memObject, PSN_ALLOC_ANY, &context );
   if ( errcode != PSO_OK ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

