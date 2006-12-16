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

const bool expectedToPass = true;

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
   
   initTest( expectedToPass );

   vdscInitErrorHandler( &context.errorHandler );

   ptr = malloc( allocatedLength );
   if ( ptr == NULL )
      ERROR_EXIT( expectedToPass, NULL, );
   
   g_pBaseAddr = ptr;
   pAlloc = (vdseMemAlloc*)(g_pBaseAddr + PAGESIZE);
   vdseMemAllocInit( pAlloc, ptr, allocatedLength, &context );
   
   pObj = vdseMallocPages( pAlloc, 4, &context );
   if ( pObj == NULL )
      ERROR_EXIT( expectedToPass, &context.errorHandler, );
   
   errcode = vdseMemObjectInit( pObj, 
                                VDSE_IDENT_ALLOCATOR,
                                4 );
   if ( errcode != VDS_OK )
      ERROR_EXIT( expectedToPass, NULL, );
   
   pageGroup = (vdsePageGroup*) ((unsigned char*)pObj + sizeof(vdseMemObject));
   vdsePageGroupInit( pageGroup,
                      2*PAGESIZE,
                      4 );

   /* Add the pageGroup to the list of groups of the memObject */
   vdseLinkedListPutFirst( &pObj->listPageGroup, 
                           &pageGroup->node );

   buff[0] = vdseMalloc( pObj, PAGESIZE, &context );
   if ( buff[0] == NULL ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, );
   
   buff[1] = vdseMalloc( pObj, PAGESIZE, &context );
   if ( buff[1] == NULL ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, );

   buff[2] = vdseMalloc( pObj, PAGESIZE, &context );
   if ( buff[2] == NULL )
      ERROR_EXIT( expectedToPass, &context.errorHandler, );

   vdseFree( pObj, buff[1], PAGESIZE, &context );
   vdseFree( pObj, buff[2], PAGESIZE, &context );
   vdseFree( pObj, buff[0], PAGESIZE, &context );
   if ( pObj->totalPages != 4 ) 
      ERROR_EXIT( expectedToPass, NULL, );
   
   buff[0] = vdseMalloc( pObj, 3*PAGESIZE, &context );
   if ( buff[0] == NULL )
      ERROR_EXIT( expectedToPass, &context.errorHandler, );
   if ( pObj->totalPages != 4 )
      ERROR_EXIT( expectedToPass, NULL, );
   
   /* Needs two new pages at this point */
   buff[3] = vdseMalloc( pObj, PAGESIZE, &context );
   if ( buff[3] == NULL )
      ERROR_EXIT( expectedToPass, &context.errorHandler, );
   if ( pObj->totalPages != 6 )
      ERROR_EXIT( expectedToPass, NULL, );
   
   vdseFree( pObj, buff[3], PAGESIZE, &context );
   if ( pObj->totalPages != 4 )
      ERROR_EXIT( expectedToPass, &context.errorHandler, );
   
   errcode = vdseMemObjectFini( pObj );
   if ( errcode != VDS_OK )
      ERROR_EXIT( expectedToPass, NULL, );
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

