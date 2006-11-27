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
#include "EngineTestCommon.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   vdseMemObject obj;
   vdsErrors errcode;
   size_t len;
   
   initTest( true );

   errcode = vdseMemObjectInit( &obj, 
                                VDSE_IDENT_ALLOCATOR,
                                63,
                                7 );

   if ( errcode != VDS_OK ) return 1;
   if ( obj.objType != VDSE_IDENT_ALLOCATOR ) return 1;
   if ( obj.accessCounter != 0 ) return 1;
   if ( obj.navigator.numPagesGroup != 7 ) return 1;
   if ( obj.navigator.nextGroupOfPages != NULL_OFFSET ) return 1;

   /* 
    * Test the algorithm to calculate remaining free space. The test will
    * fail if run on a machine with more than 256 bits alignment...
    */
   len = 7*PAGESIZE-sizeof(vdsePageNavig)-64;
   fprintf( stderr, "%d %d\n", len, obj.remainingBytes );
   if ( obj.remainingBytes != len ) return 1;
   
   errcode = vdseMemObjectFini( &obj );
   if ( errcode != VDS_OK ) return 1;
   errcode = vdseMemObjectInit( &obj, 
                                VDSE_IDENT_ALLOCATOR,
                                64,
                                7 );
   if ( errcode != VDS_OK ) return 1;
   len = 7*PAGESIZE-sizeof(vdsePageNavig)-64;
   fprintf( stderr, "%d %d\n", len, obj.remainingBytes );
   if ( obj.remainingBytes != len ) return 1;

   errcode = vdseMemObjectFini( &obj );
   if ( errcode != VDS_OK ) return 1;
   errcode = vdseMemObjectInit( &obj, 
                                VDSE_IDENT_ALLOCATOR,
                                65,
                                7 );
   if ( errcode != VDS_OK ) return 1;
   len = 7*PAGESIZE-sizeof(vdsePageNavig)-64-VDSE_MEM_ALIGNMENT;
   fprintf( stderr, "%d %d\n", len, obj.remainingBytes );
   if ( obj.remainingBytes != len ) return 1;
   
   return 0;
}

