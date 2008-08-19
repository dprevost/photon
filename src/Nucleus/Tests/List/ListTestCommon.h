/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#include "Nucleus/LinkedList.h"
#include "Common/ErrorHandler.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* 1 kb, should be plenty (more than enough in fact since we only use 
 * the base address to calculate the offsets but never use the memory itself.
 */

#define MEM_SIZE (1024)

pscErrorHandler g_info;
unsigned char    g_dum[MEM_SIZE];

void InitMem()
{
   g_pBaseAddr = g_dum;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Used by the regression tests (ListTestSmall and ListTestBig) */
struct dummyStruct
{
   int dummy1;
   char dummy2[7];
   psnLinkNode node;
   int isInUse;
   char dummy3[3];
};

typedef struct dummyStruct dummyStruct;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int TestList( psnLinkedList* pList )
{
   size_t testSize = pList->currentSize;
   psnLinkNode* pItem = (psnLinkNode*) &pList->head;

   /* Check the chain going forward. */ 
   while ( testSize > 0 ) {
      GET_PTR( pItem, pItem->nextOffset, psnLinkNode );
      if ( pItem == &pList->head ) {
         fprintf( stderr, "Error 1\n" );
         return -1;
      }
      
      testSize--;
   }
   GET_PTR( pItem, pItem->nextOffset, psnLinkNode );
   if ( pItem != &pList->head ) {
      fprintf( stderr, "Error 2\n" );
      return -2;
   }   

   /* Check the chain going backward. */ 
   testSize = pList->currentSize;
   pItem =  (psnLinkNode*) &pList->head; 

   while ( testSize > 0 ) {
      GET_PTR( pItem, pItem->previousOffset, psnLinkNode );
      if ( pItem == &pList->head ) {
         fprintf( stderr, "Error 3\n" );
         return -3;
      }
      testSize--;
   }
   GET_PTR( pItem, pItem->previousOffset, psnLinkNode );
   if ( pItem != &pList->head ) {
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

void DumpList( psnLinkedList* pList )
{
   size_t testSize = pList->currentSize;
   psnLinkNode* pItem = &pList->head;

   while ( testSize > 0 ) {
      GET_PTR( pItem, pItem->nextOffset, psnLinkNode );
      fprintf( stderr, VDSF_SIZE_T_FORMAT" = %p %p %p %p \n",
               testSize,
               pItem ,
               GET_PTR_FAST(pItem->nextOffset,psnLinkNode ),
               GET_PTR_FAST(pItem->previousOffset,psnLinkNode ),
               &pList->head );
      testSize--;
   }
}

#endif /* LIST_TEST_COMMON_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

