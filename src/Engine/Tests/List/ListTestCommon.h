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

#ifndef LIST_TEST_COMMON_H
#define LIST_TEST_COMMON_H

#include "LinkedList.h"
#include "ErrorHandler.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* 1 kb, should be plenty (more than enough in fact since we only use 
 * the base address to calculate the offsets but never use the memory itself.
 */

#define MEM_SIZE (1024)

vdscErrorHandler g_info;
vdseMemAlloc     g_alloc;
unsigned char    g_dum[MEM_SIZE];

/* Return 0 on success and -1 on error */
int InitMem()
{
   vdsErrors err;

   vdscInitErrorDefs();
   vdscInitErrorHandler( &g_info );
   
   err = vdseMemAllocInit( &g_alloc, 
                           g_dum, 
                           g_dum, 
                           MEM_SIZE, 
                           &g_info );
   if ( err != 0 )
      return -1;

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Used by the regression tests (ListTestSmall and ListTestBig) */
typedef struct dummyStruct
{
   int dummy1;
   char dummy2[7];
   vdseLinkNode node;
   int isInUse;
   char dummy3[3];
   
} dummyStruct;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int TestList( vdseLinkedList* pList,
              vdseMemAlloc*   pAllocator )
{
   size_t testSize = pList->currentSize;
   vdseLinkNode* pItem = (vdseLinkNode*) &pList->head;

   /* Check the chain going forward. */ 
   while ( testSize > 0 )
   {
      pItem = GET_PTR( pItem->nextOffset, vdseLinkNode, pAllocator );
      if ( pItem == &pList->head )
      {
         fprintf( stderr, "Error 1\n" );
         return -1;
      }
      
      testSize--;
   }
   pItem = GET_PTR( pItem->nextOffset, vdseLinkNode, pAllocator );
   if ( pItem != &pList->head )
   {
      fprintf( stderr, "Error 2\n" );
      return -2;
   }   

   /* Check the chain going backward. */ 
   testSize = pList->currentSize;
   pItem =  (vdseLinkNode*) &pList->head; 

   while ( testSize > 0 )
   {
      pItem = GET_PTR( pItem->previousOffset, 
                       vdseLinkNode, 
                       pAllocator );
      if ( pItem == &pList->head )
      {
         fprintf( stderr, "Error 3\n" );
         return -3;
      }
      testSize--;
   }
   pItem = GET_PTR( pItem->previousOffset, vdseLinkNode, pAllocator );
   if ( pItem != &pList->head )
   {
      fprintf( stderr, "Error 4\n" );
      return -4;
   }   

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* A very simple pseudo-random generator. Taken from the man pages
 * of rand() on linux which took it from an example in POSIX  1003.1-2003.
 * I'm assuming that this code is in the public domain. 
 */

static unsigned long g_next = 1;

#define MY_RAND_MAX 32767

/* RAND_MAX assumed to be 32767 */
unsigned int myrand(void) {
   g_next = g_next * 1103515245 + 12345;   
   return((unsigned)(g_next/65536) % 32768);
}

void mysrand(unsigned seed) {
   g_next = seed;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void DumpList( vdseLinkedList* pList,
               vdseMemAlloc*   pAllocator )
{
   size_t testSize = pList->currentSize;
   vdseLinkNode* pItem = &pList->head;

   while ( testSize > 0 )
   {
      pItem = GET_PTR(pItem->nextOffset,vdseLinkNode, pAllocator);
      fprintf( stderr, " %d = %p %p %p %p \n",
               testSize,
               pItem ,
               GET_PTR(pItem->nextOffset,vdseLinkNode, pAllocator),
               GET_PTR(pItem->previousOffset,vdseLinkNode,pAllocator),
               &pList->head );
      testSize--;
   }
}

#endif /* LIST_TEST_COMMON_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
