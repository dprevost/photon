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

#include "MemoryObject.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   vdseMemObject obj;
   vdsErrors errcode;
   
   errcode = vdseMemObjectInit( &obj, 
                                VDSE_IDENT_ALLOCATOR,
                                sizeof(obj),
                                7 );
   if ( errcode != VDS_OK ) return 1;
   
   errcode = vdseMemObjectFini( &obj );
   if ( errcode != VDS_OK ) return 1;
   
   if ( obj.objType != VDSE_IDENT_CLEAR ) return 1;
   if ( obj.accessCounter != 0 ) return 1;
   if ( obj.navigator.numPagesGroup != 0 ) return 1;
   if ( obj.navigator.nextGroupOfPages != NULL_OFFSET ) return 1;
   if ( obj.remainingBytes != 0 ) return 1;
   
   return 0;
}

