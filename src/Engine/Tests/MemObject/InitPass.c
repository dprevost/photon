/*
 * Copyright (C) 2006-2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

#include "MemoryObject.h"
#include "EngineTestCommon.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   vdseMemObject obj;
   vdsErrors errcode;
   
   initTest( expectedToPass );

   errcode = vdseMemObjectInit( &obj, 
                                VDSE_IDENT_ALLOCATOR,
                                7 );
   if ( errcode != VDS_OK )
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( obj.objType != VDSE_IDENT_ALLOCATOR )
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( obj.accessCounter != 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( obj.totalPages != 7 )
      ERROR_EXIT( expectedToPass, NULL, ; );

   vdseMemObjectFini( &obj );
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

