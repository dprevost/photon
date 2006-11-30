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
#include "MemoryAllocator.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   vdseMemObject* pObj;
   vdsErrors errcode;
   vdseSessionContext context;
   vdseMemAlloc*     pAlloc;
   unsigned char* ptr, *buff[9];
   size_t allocatedLength = PAGESIZE*10;
   vdsePageGroup *pageGroup = NULL;
   
   initTest( true );

   vdscInitErrorHandler( &context.errorHandler );

   ptr = malloc( allocatedLength );

   g_pBaseAddr = ptr;
   pAlloc = (vdseMemAlloc*)(g_pBaseAddr + PAGESIZE);
   vdseMemAllocInit( pAlloc, ptr, allocatedLength, &context );
   
   pObj = vdseMallocPages( pAlloc, 4, &context );
   
   if ( pObj == NULL ) return 1;
   
   errcode = vdseMemObjectInit( pObj, 
                                VDSE_IDENT_ALLOCATOR,
                                4 );
   if ( errcode != VDS_OK ) ERROR_EXIT(1);
   
   pageGroup = (vdsePageGroup*) ((unsigned char*)pObj + sizeof(vdseMemObject));
   vdsePageGroupInit( pageGroup,
                      2*PAGESIZE,
                      4,
                      VDSE_ALLOCATION_UNIT );

   /* Add the pageGroup to the list of groups of the memObject */
   vdseLinkedListPutFirst( &pObj->listPageGroup, 
                           &pageGroup->node );

   buff[0] = vdseMalloc( pObj, PAGESIZE, &context );
   if ( buff[0] == NULL ) ERROR_EXIT(1);
   
   buff[1] = vdseMalloc( pObj, PAGESIZE, &context );
   if ( buff[1] == NULL ) ERROR_EXIT(1);

   buff[2] = vdseMalloc( pObj, PAGESIZE, &context );
   if ( buff[2] == NULL ) ERROR_EXIT(1);

   /* Needs two new pages at this point */
   buff[3] = vdseMalloc( pObj, PAGESIZE, &context );
   if ( buff[3] == NULL ) ERROR_EXIT(1);
   if ( pObj->totalPages != 6 ) ERROR_EXIT(1);
   
   /* Needs NO new page at this point */
   buff[4] = vdseMalloc( pObj, PAGESIZE/2, &context );
   if ( buff[4] == NULL ) ERROR_EXIT(1);
   if ( pObj->totalPages != 6 ) ERROR_EXIT(1);   

   /* Needs NO new page at this point */
   buff[5] = vdseMalloc( pObj, PAGESIZE/2, &context );
   if ( buff[5] == NULL ) ERROR_EXIT(1);
   if ( pObj->totalPages != 6 ) ERROR_EXIT(1);   

   /* Needs a new page at this point */
   buff[6] = vdseMalloc( pObj, PAGESIZE/2, &context );
   if ( buff[6] == NULL ) ERROR_EXIT(1);
   if ( pObj->totalPages != 7 ) ERROR_EXIT(1);      

   /* Needs a new page at this point */
   buff[7] = vdseMalloc( pObj, PAGESIZE/2, &context );
   if ( buff[7] == NULL ) ERROR_EXIT(1);
   if ( pObj->totalPages != 8 ) ERROR_EXIT(1);

   /* We exhausted all the memory */
   buff[8] = vdseMalloc( pObj, PAGESIZE, &context );
   if ( buff[8] != NULL ) ERROR_EXIT(1);
   if ( pObj->totalPages != 8 ) ERROR_EXIT(1);
   
   /////   
   errcode = vdseMemObjectFini( pObj );
   if ( errcode != VDS_OK ) ERROR_EXIT(1);
   
   return 0;
}

