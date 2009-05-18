/*
 * Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#include "Common/Common.h"
#include <photon/photon>
#include <iostream>
#include <vector>

#include "API/api.h"
#include "API/HashMap.h"
#include "API/Queue.h"
#include "API/Lifo.h"

using namespace std;
using namespace pso;

#define NUM_MAPS    8
#define NUM_QUEUES 10
#define NUM_LIFOS  10

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

struct myQueue
{
   myQueue()
      : name  ( "TestFolder/Queue" ) {}

   Queue queue;
   string   name;
};

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

struct myMap
{
   myMap()
      : name ( "TestFolder/HashMap" ) {}

   HashMap map;
   string     name;
};

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

struct myLifo
{
   myLifo()
      : name  ( "TestFolder/Lifo" ) {}

   Lifo queue;
   string  name;
};

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

string foldername("TestFolder");

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int AddDefectsHashMaps( vector<myMap> & h )
{
   int api_offset = -1;
   struct psoaHashMap ** apiHashMap;
   struct psonHashMap * pHashMap;
   psonTxStatus * txItemStatus, * txHashMapStatus;
   unsigned long i, ** apiObj;
   ptrdiff_t offset, previousOffset;
   psonHashTxItem * pItem;
   ptrdiff_t* pArray;   
   bool ok;
   
   // Using a (void *) cast to eliminate a gcc warning (dereferencing 
   // type-punned pointer will break strict-aliasing rules)
   apiObj = (unsigned long **) ( (void *) &h[0].map );

   // We have to go around the additional data member inserted by the
   // compiler for the virtual table (true for g++)
   for ( i = 0; i < sizeof(HashMap)/sizeof(void*); ++i, apiObj++ ) {
      if ( **apiObj == PSOA_HASH_MAP ) {
         api_offset = i * sizeof(void*);
         break;
      }
   }
   if ( api_offset == -1 ) {
      cerr << "Can't calculate offset!" << endl;
      return -1;
   }
   
   cout << "Defect for " << h[0].name << ": None" << endl;

   cout << "Defect for " << h[1].name << ": 3 ref. couters" << endl;
   apiHashMap = (psoaHashMap **) ( (unsigned char *) &h[1].map + api_offset );
   pHashMap = (psonHashMap *) (*apiHashMap)->object.pMyMemObject;
   pHashMap->nodeObject.txCounter++;
   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, psonTxStatus );
   txHashMapStatus->usageCounter++;

   ok = psonHashTxGetFirst( &pHashMap->hashObj, &offset );
   i = 0;
   while ( ok ) {
      previousOffset = offset;
      ok = psonHashTxGetNext( &pHashMap->hashObj,
                              previousOffset,
                              &offset );
      i++;
      if ( i >= 6 ) break;
   }
   if ( ! ok ) {
      cerr << "Iteration error in " << h[1].name << endl;
      return -1;
   }
   GET_PTR( pItem, offset, psonHashTxItem );
   txItemStatus = &pItem->txStatus;
   txItemStatus->usageCounter++;

   cout << "Defect for " << h[2].name << ": object added - not committed" << endl;
   apiHashMap = (psoaHashMap **) ( (unsigned char *) &h[2].map + api_offset );
   pHashMap = (psonHashMap *) (*apiHashMap)->object.pMyMemObject;
   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, psonTxStatus );
   txHashMapStatus->txOffset = SET_OFFSET( pHashMap ); 
   txHashMapStatus->status = PSON_TXS_ADDED;

   cout << "Defect for " << h[3].name << ": 5 items removed-committed," << endl;
   cout << "                                  4 items removed - not committed," << endl;
   cout << "                                  9 items added - not committed" << endl;
   apiHashMap = (psoaHashMap **) ( (unsigned char *) &h[3].map + api_offset );
   pHashMap = (psonHashMap *) (*apiHashMap)->object.pMyMemObject;

   ok = psonHashTxGetFirst( &pHashMap->hashObj, &offset );
   i = 0;
   while ( ok ) {      
      GET_PTR( pItem, offset, psonHashTxItem );
      txItemStatus = &pItem->txStatus;

      if ( i < 5 ) { /* removed committed */
         txItemStatus->txOffset = SET_OFFSET( pHashMap ); 
         txItemStatus->status = PSON_TXS_DESTROYED_COMMITTED;
      }
      else if ( i < 9 ) { /* removed uncommitted */
         txItemStatus->txOffset = SET_OFFSET( pHashMap ); 
         txItemStatus->status = PSON_TXS_DESTROYED;
      }
      else if ( i >= 11 ) { /* Added */
         txItemStatus->txOffset = SET_OFFSET( pHashMap ); 
         txItemStatus->status = PSON_TXS_ADDED;
      }

      previousOffset = offset;
      ok = psonHashTxGetNext( &pHashMap->hashObj,
                              previousOffset,
                              &offset );
      i++;

   }
   if ( i < 20 ) {
      cerr << "Iteration error in " << h[3].name << endl;
      return -1;
   }
   
   cout << "Defect for " << h[4].name << ": object locked" << endl;
   apiHashMap = (psoaHashMap **) ( (unsigned char *) &h[4].map + api_offset );
   pHashMap = (psonHashMap *) (*apiHashMap)->object.pMyMemObject;
   if ( ! psocTryAcquireProcessLock(&pHashMap->memObject.lock, getpid(), 0) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }

   cout << "Defect for " << h[5].name << ": items - invalid offset" << endl;
   apiHashMap = (psoaHashMap **) ( (unsigned char *) &h[5].map + api_offset );
   pHashMap = (psonHashMap *) (*apiHashMap)->object.pMyMemObject;
   if ( ! psocTryAcquireProcessLock(&pHashMap->memObject.lock, getpid(), 0) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }

   GET_PTR( pArray, pHashMap->hashObj.arrayOffset, ptrdiff_t );

   for ( i = 0; i < g_psonArrayLengths[pHashMap->hashObj.lengthIndex]; ++i ) {
      if ( pArray[i] != PSON_NULL_OFFSET ) pArray[i] = 0;
   }
   
   cout << "Defect for " << h[6].name << ": item - invalid key length" << endl;
   apiHashMap = (psoaHashMap **) ( (unsigned char *) &h[6].map + api_offset );
   pHashMap = (psonHashMap *) (*apiHashMap)->object.pMyMemObject;
   if ( ! psocTryAcquireProcessLock(&pHashMap->memObject.lock, getpid(), 0) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }
   ok = psonHashTxGetFirst( &pHashMap->hashObj, &offset );
   i = 0;
   while ( ok ) {
      previousOffset = offset;
      ok = psonHashTxGetNext( &pHashMap->hashObj,
                              previousOffset,
                              &offset );
      i++;
      if ( i >= 6 ) break;
   }
   if ( ! ok ) {
      cerr << "Iteration error in " << h[6].name << endl;
      return -1;
   }
   GET_PTR( pItem, offset, psonHashTxItem );
   pItem->keyLength = 0;
   
   cout << "Defect for " << h[7].name << ": item - invalid data offset" << endl;
   apiHashMap = (psoaHashMap **) ( (unsigned char *) &h[7].map + api_offset );
   pHashMap = (psonHashMap *) (*apiHashMap)->object.pMyMemObject;
   if ( ! psocTryAcquireProcessLock(&pHashMap->memObject.lock, getpid(), 0) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }
   ok = psonHashTxGetFirst( &pHashMap->hashObj, &offset );
   i = 0;
   while ( ok ) {
      previousOffset = offset;
      ok = psonHashTxGetNext( &pHashMap->hashObj,
                              previousOffset,
                              &offset );
      i++;
      if ( i >= 6 ) break;
   }
   if ( ! ok ) {
      cerr << "Iteration error in " << h[7].name << endl;
      return -1;
   }
   GET_PTR( pItem, offset, psonHashTxItem );
   pItem->dataOffset = 0;

   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int AddDefectsLifos( vector<myLifo> & l )
{
   int api_offset = -1;
   struct psoaLifo ** apiLifo;
   struct psonQueue * pQueue;
   psonTxStatus * txItemStatus, * txQueueStatus;
   unsigned long i, ** apiObj;
   psonLinkNode * pNode = NULL, *pSavedNode = NULL;
   psonQueueItem* pQueueItem = NULL;
   bool okList;
   
   // Using a (void *) cast to eliminate a gcc warning (dereferencing 
   // type-punned pointer will break strict-aliasing rules)
   apiObj = (unsigned long **) ( (void *) &l[0].queue );
   // We have to go around the additional data member inserted by the
   // compiler for the virtual table
   for ( i = 0; i < sizeof(Lifo)/sizeof(void*); ++i, apiObj++ ) {
      if ( **apiObj == PSOA_LIFO ) {
         api_offset = i * sizeof(void*);
         break;
      }
   }
   if ( api_offset == -1 ) {
      cerr << "Can't calculate offset!" << endl;
      return -1;
   }
   
   cout << "Defect for " << l[0].name << ": None" << endl;

   cout << "Defect for " << l[1].name << ": 3 ref. couters" << endl;
   apiLifo = (psoaLifo **) ( (unsigned char *) &l[1].queue + api_offset );
   pQueue = (psonQueue *) (*apiLifo)->object.pMyMemObject;
   pQueue->nodeObject.txCounter++;
   GET_PTR( txQueueStatus, pQueue->nodeObject.txStatusOffset, psonTxStatus );
   txQueueStatus->usageCounter++;
   
   okList = psonLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
   i = 0;
   while ( okList ) {
      okList =  psonLinkedListPeakNext( &pQueue->listOfElements, 
                                        pNode, 
                                        &pNode );
      i++;
      if ( i >= 6 ) break;
   }
   if ( ! okList ) {
      cerr << "Iteration error in " << l[1].name << endl;
      return -1;
   }
   pQueueItem = (psonQueueItem*) 
      ((char*)pNode - offsetof( psonQueueItem, node ));
   txItemStatus = &pQueueItem->txStatus;
   txItemStatus->usageCounter++;

   cout << "Defect for " << l[2].name << ": object added - not committed" << endl;
   apiLifo = (psoaLifo **) ( (unsigned char *) &l[2].queue + api_offset );
   pQueue = (psonQueue *) (*apiLifo)->object.pMyMemObject;
   GET_PTR( txQueueStatus, pQueue->nodeObject.txStatusOffset, psonTxStatus );
   txQueueStatus->txOffset = SET_OFFSET( pQueue ); 
   txQueueStatus->status = PSON_TXS_ADDED;

   // Queue 2. Defects: 
   //  - Items added (not committed) and items removed (committed + non-comm)
   cout << "Defect for " << l[3].name << ": 5 items removed-committed," << endl;
   cout << "                                  4 items removed - not committed," << endl;
   cout << "                                  9 items added - not committed" << endl;
   apiLifo = (psoaLifo **) ( (unsigned char *) &l[3].queue + api_offset );
   pQueue = (psonQueue *) (*apiLifo)->object.pMyMemObject;

   okList = psonLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
   i = 0;
   while ( okList ) {
      pQueueItem = (psonQueueItem*) 
         ((char*)pNode - offsetof( psonQueueItem, node ));
      txItemStatus = &pQueueItem->txStatus;

      if ( i < 5 ) { /* removed committed */
         txItemStatus->txOffset = SET_OFFSET( pQueue ); 
         txItemStatus->status = PSON_TXS_DESTROYED_COMMITTED;
      }
      else if ( i < 9 ) { /* removed uncommitted */
         txItemStatus->txOffset = SET_OFFSET( pQueue ); 
         txItemStatus->status = PSON_TXS_DESTROYED;
      }
      else if ( i >= 11 ) { /* Added */
         txItemStatus->txOffset = SET_OFFSET( pQueue ); 
         txItemStatus->status = PSON_TXS_ADDED;
      }
      
      okList =  psonLinkedListPeakNext( &pQueue->listOfElements, 
                                             pNode, 
                                             &pNode );
      i++;
   }
   
   cout << "Defect for " << l[4].name << ": object locked" << endl;
   apiLifo = (psoaLifo **) ( (unsigned char *) &l[4].queue + api_offset );
   pQueue = (psonQueue *) (*apiLifo)->object.pMyMemObject;
   if ( ! psocTryAcquireProcessLock(&pQueue->memObject.lock, getpid(), 0) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }

   cout << "Defect for " << l[5].name << ": broken forward link" << endl;
   apiLifo = (psoaLifo **) ( (unsigned char *) &l[5].queue + api_offset );
   pQueue = (psonQueue *) (*apiLifo)->object.pMyMemObject;
   if ( ! psocTryAcquireProcessLock(&pQueue->memObject.lock, getpid(), 0) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }

   okList = psonLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
   i = 0;
   while ( okList ) {
      okList =  psonLinkedListPeakNext( &pQueue->listOfElements, 
                                        pNode, 
                                        &pNode );
      i++;
      if ( i == 9 ) {
         pNode->nextOffset = PSON_NULL_OFFSET;
         break;
      }
   }
   if ( ! okList ) {
      cerr << "Iteration error in " << l[5].name << endl;
      return -1;
   }

   cout << "Defect for " << l[6].name << ": broken backward link" << endl;
   apiLifo = (psoaLifo **) ( (unsigned char *) &l[6].queue + api_offset );
   pQueue = (psonQueue *) (*apiLifo)->object.pMyMemObject;
   if ( ! psocTryAcquireProcessLock(&pQueue->memObject.lock, getpid(), 0) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }

   okList = psonLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
   i = 0;
   while ( okList ) {
      okList =  psonLinkedListPeakNext( &pQueue->listOfElements, 
                                        pNode, 
                                        &pNode );
      i++;
      if ( i == 9 ) {
         pNode->previousOffset = PSON_NULL_OFFSET;
         break;
      }
   }
   if ( ! okList ) {
      cerr << "Iteration error in " << l[6].name << endl;
      return -1;
   }
   
   cout << "Defect for " << l[7].name << ": 2 broken forward links" << endl;
   apiLifo = (psoaLifo **) ( (unsigned char *) &l[7].queue + api_offset );
   pQueue = (psonQueue *) (*apiLifo)->object.pMyMemObject;
   if ( ! psocTryAcquireProcessLock(&pQueue->memObject.lock, getpid(), 0) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }

   okList = psonLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
   i = 0;
   while ( okList ) {
      okList =  psonLinkedListPeakNext( &pQueue->listOfElements, 
                                        pNode, 
                                        &pNode );
      i++;
      if ( i == 9 ) pSavedNode = pNode;
      if ( i == 13 ) {
         pSavedNode->nextOffset = PSON_NULL_OFFSET;
         pNode->nextOffset = PSON_NULL_OFFSET;
         break;
      }
   }
   if ( ! okList ) {
      cerr << "Iteration error in " << l[7].name << endl;
      return -1;
   }

   cout << "Defect for " << l[8].name << ": 2 broken backward links" << endl;
   apiLifo = (psoaLifo **) ( (unsigned char *) &l[8].queue + api_offset );
   pQueue = (psonQueue *) (*apiLifo)->object.pMyMemObject;
   if ( ! psocTryAcquireProcessLock(&pQueue->memObject.lock, getpid(), 0) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }

   okList = psonLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
   i = 0;
   while ( okList ) {
      okList =  psonLinkedListPeakNext( &pQueue->listOfElements, 
                                        pNode, 
                                        &pNode );
      i++;
      if ( i == 9 ) pSavedNode = pNode;
      if ( i == 13 ) {
         pSavedNode->previousOffset = PSON_NULL_OFFSET;
         pNode->previousOffset = PSON_NULL_OFFSET;
         break;
      }
   }
   if ( ! okList ) {
      cerr << "Iteration error in " << l[8].name << endl;
      return -1;
   }

   cout << "Defect for " << l[9].name << ": broken bw+fw links (eq)" << endl;
   apiLifo = (psoaLifo **) ( (unsigned char *) &l[9].queue + api_offset );
   pQueue = (psonQueue *) (*apiLifo)->object.pMyMemObject;
   if ( ! psocTryAcquireProcessLock(&pQueue->memObject.lock, getpid(), 0) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }

   okList = psonLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
   i = 0;
   while ( okList ) {
      okList =  psonLinkedListPeakNext( &pQueue->listOfElements, 
                                        pNode, 
                                        &pNode );
      i++;
      if ( i == 9 ) {
         pNode->previousOffset = PSON_NULL_OFFSET;
         pNode->nextOffset = PSON_NULL_OFFSET;
         break;
      }
   }
   if ( ! okList ) {
      cerr << "Iteration error in " << l[9].name << endl;
      return -1;
   }

   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int AddDefectsQueues( vector<myQueue> & q )
{
   int api_offset = -1;
   struct psoaQueue ** apiQueue;
   struct psonQueue * pQueue;
   psonTxStatus * txItemStatus, * txQueueStatus;
   unsigned long i, ** apiObj;
   psonLinkNode * pNode = NULL, *pSavedNode = NULL;
   psonQueueItem* pQueueItem = NULL;
   bool okList;
   
   // Using a (void *) cast to eliminate a gcc warning (dereferencing 
   // type-punned pointer will break strict-aliasing rules)
   apiObj = (unsigned long **) ( (void *) &q[0].queue );
   // We have to go around the additional data member inserted by the
   // compiler for the virtual table
   for ( i = 0; i < sizeof(Queue)/sizeof(void*); ++i, apiObj++ ) {
      if ( **apiObj == PSOA_QUEUE ) {
         api_offset = i * sizeof(void*);
         break;
      }
   }
   if ( api_offset == -1 ) {
      cerr << "Can't calculate offset!" << endl;
      return -1;
   }
   
   cout << "Defect for " << q[0].name << ": None" << endl;

   cout << "Defect for " << q[1].name << ": 3 ref. couters" << endl;
   apiQueue = (psoaQueue **) ( (unsigned char *) &q[1].queue + api_offset );
   pQueue = (psonQueue *) (*apiQueue)->object.pMyMemObject;
   pQueue->nodeObject.txCounter++;
   GET_PTR( txQueueStatus, pQueue->nodeObject.txStatusOffset, psonTxStatus );
   txQueueStatus->usageCounter++;
   
   okList = psonLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
   i = 0;
   while ( okList ) {
      okList =  psonLinkedListPeakNext( &pQueue->listOfElements, 
                                        pNode, 
                                        &pNode );
      i++;
      if ( i >= 6 ) break;
   }
   if ( ! okList ) {
      cerr << "Iteration error in " << q[1].name << endl;
      return -1;
   }
   pQueueItem = (psonQueueItem*) 
      ((char*)pNode - offsetof( psonQueueItem, node ));
   txItemStatus = &pQueueItem->txStatus;
   txItemStatus->usageCounter++;

   cout << "Defect for " << q[2].name << ": object added - not committed" << endl;
   apiQueue = (psoaQueue **) ( (unsigned char *) &q[2].queue + api_offset );
   pQueue = (psonQueue *) (*apiQueue)->object.pMyMemObject;
   GET_PTR( txQueueStatus, pQueue->nodeObject.txStatusOffset, psonTxStatus );
   txQueueStatus->txOffset = SET_OFFSET( pQueue ); 
   txQueueStatus->status = PSON_TXS_ADDED;

   // Queue 2. Defects: 
   //  - Items added (not committed) and items removed (committed + non-comm)
   cout << "Defect for " << q[3].name << ": 5 items removed-committed," << endl;
   cout << "                                  4 items removed - not committed," << endl;
   cout << "                                  9 items added - not committed" << endl;
   apiQueue = (psoaQueue **) ( (unsigned char *) &q[3].queue + api_offset );
   pQueue = (psonQueue *) (*apiQueue)->object.pMyMemObject;

   okList = psonLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
   i = 0;
   while ( okList ) {
      pQueueItem = (psonQueueItem*) 
         ((char*)pNode - offsetof( psonQueueItem, node ));
      txItemStatus = &pQueueItem->txStatus;

      if ( i < 5 ) { /* removed committed */
         txItemStatus->txOffset = SET_OFFSET( pQueue ); 
         txItemStatus->status = PSON_TXS_DESTROYED_COMMITTED;
      }
      else if ( i < 9 ) { /* removed uncommitted */
         txItemStatus->txOffset = SET_OFFSET( pQueue ); 
         txItemStatus->status = PSON_TXS_DESTROYED;
      }
      else if ( i >= 11 ) { /* Added */
         txItemStatus->txOffset = SET_OFFSET( pQueue ); 
         txItemStatus->status = PSON_TXS_ADDED;
      }
      
      okList =  psonLinkedListPeakNext( &pQueue->listOfElements, 
                                             pNode, 
                                             &pNode );
      i++;
   }
   
   cout << "Defect for " << q[4].name << ": object locked" << endl;
   apiQueue = (psoaQueue **) ( (unsigned char *) &q[4].queue + api_offset );
   pQueue = (psonQueue *) (*apiQueue)->object.pMyMemObject;
   if ( ! psocTryAcquireProcessLock(&pQueue->memObject.lock, getpid(), 0) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }

   cout << "Defect for " << q[5].name << ": broken forward link" << endl;
   apiQueue = (psoaQueue **) ( (unsigned char *) &q[5].queue + api_offset );
   pQueue = (psonQueue *) (*apiQueue)->object.pMyMemObject;
   if ( ! psocTryAcquireProcessLock(&pQueue->memObject.lock, getpid(), 0) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }

   okList = psonLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
   i = 0;
   while ( okList ) {
      okList =  psonLinkedListPeakNext( &pQueue->listOfElements, 
                                        pNode, 
                                        &pNode );
      i++;
      if ( i == 9 ) {
         pNode->nextOffset = PSON_NULL_OFFSET;
         break;
      }
   }
   if ( ! okList ) {
      cerr << "Iteration error in " << q[5].name << endl;
      return -1;
   }

   cout << "Defect for " << q[6].name << ": broken backward link" << endl;
   apiQueue = (psoaQueue **) ( (unsigned char *) &q[6].queue + api_offset );
   pQueue = (psonQueue *) (*apiQueue)->object.pMyMemObject;
   if ( ! psocTryAcquireProcessLock(&pQueue->memObject.lock, getpid(), 0) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }

   okList = psonLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
   i = 0;
   while ( okList ) {
      okList =  psonLinkedListPeakNext( &pQueue->listOfElements, 
                                        pNode, 
                                        &pNode );
      i++;
      if ( i == 9 ) {
         pNode->previousOffset = PSON_NULL_OFFSET;
         break;
      }
   }
   if ( ! okList ) {
      cerr << "Iteration error in " << q[6].name << endl;
      return -1;
   }
   
   cout << "Defect for " << q[7].name << ": 2 broken forward links" << endl;
   apiQueue = (psoaQueue **) ( (unsigned char *) &q[7].queue + api_offset );
   pQueue = (psonQueue *) (*apiQueue)->object.pMyMemObject;
   if ( ! psocTryAcquireProcessLock(&pQueue->memObject.lock, getpid(), 0) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }

   okList = psonLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
   i = 0;
   while ( okList ) {
      okList =  psonLinkedListPeakNext( &pQueue->listOfElements, 
                                        pNode, 
                                        &pNode );
      i++;
      if ( i == 9 ) pSavedNode = pNode;
      if ( i == 13 ) {
         pSavedNode->nextOffset = PSON_NULL_OFFSET;
         pNode->nextOffset = PSON_NULL_OFFSET;
         break;
      }
   }
   if ( ! okList ) {
      cerr << "Iteration error in " << q[7].name << endl;
      return -1;
   }

   cout << "Defect for " << q[8].name << ": 2 broken backward links" << endl;
   apiQueue = (psoaQueue **) ( (unsigned char *) &q[8].queue + api_offset );
   pQueue = (psonQueue *) (*apiQueue)->object.pMyMemObject;
   if ( ! psocTryAcquireProcessLock(&pQueue->memObject.lock, getpid(), 0) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }

   okList = psonLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
   i = 0;
   while ( okList ) {
      okList =  psonLinkedListPeakNext( &pQueue->listOfElements, 
                                        pNode, 
                                        &pNode );
      i++;
      if ( i == 9 ) pSavedNode = pNode;
      if ( i == 13 ) {
         pSavedNode->previousOffset = PSON_NULL_OFFSET;
         pNode->previousOffset = PSON_NULL_OFFSET;
         break;
      }
   }
   if ( ! okList ) {
      cerr << "Iteration error in " << q[8].name << endl;
      return -1;
   }

   cout << "Defect for " << q[9].name << ": broken bw+fw links (eq)" << endl;
   apiQueue = (psoaQueue **) ( (unsigned char *) &q[9].queue + api_offset );
   pQueue = (psonQueue *) (*apiQueue)->object.pMyMemObject;
   if ( ! psocTryAcquireProcessLock(&pQueue->memObject.lock, getpid(), 0) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }

   okList = psonLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
   i = 0;
   while ( okList ) {
      okList =  psonLinkedListPeakNext( &pQueue->listOfElements, 
                                        pNode, 
                                        &pNode );
      i++;
      if ( i == 9 ) {
         pNode->previousOffset = PSON_NULL_OFFSET;
         pNode->nextOffset = PSON_NULL_OFFSET;
         break;
      }
   }
   if ( ! okList ) {
      cerr << "Iteration error in " << q[9].name << endl;
      return -1;
   }

   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void CleanupPreviousRun( Session & session )
{
   Folder folder;
   psoFolderEntry entry;
   int ok;
   string s;
   
   try {
      folder.Open( session, foldername );
      ok = folder.GetFirst( entry );
      while ( ok == 0 ) {
         s = foldername + "/" + entry.name;
         session.DestroyObject( s );
         ok = folder.GetNext( entry );
      }
      session.Commit();
   }
   catch ( pso::Exception exc ) {
      cerr << "Cleanup of previous session failed, error = " << exc.Message() << endl;
      exit(1);
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void PopulateHashMaps( Session & session, vector<myMap> & h )
{
   int i, j;
   string data, key;
   char s[4];
   psoObjectDefinition mapDef = { PSO_HASH_MAP, 0, 0, 0 };
   
   for ( i = 0; i < NUM_MAPS; ++i ) {
      session.CreateMap( h[i].name, mapDef, "Default", "Default" );
      h[i].map.Open( session, h[i].name );

      for ( j = 0; j < 20; ++j ) {
         sprintf(s, "%d", j);
         key = string("Key ") + s;
         data = string("Inserted data item = ") + s;
         sprintf(s, "%d", i);
         data += string(" in hashMap = ") + s;
         h[i].map.Insert( key.c_str(), key.length(), data.c_str(), data.length() );
      }
   }

   session.Commit();
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void PopulateLifos( Session & session, vector<myLifo> & l )
{
   int i, j;
   string data;
   char s[4];
   psoObjectDefinition queueDef = { PSO_LIFO, 0, 0, 0 };
   
   for ( i = 0; i < NUM_LIFOS; ++i ) {
      session.CreateQueue( l[i].name, queueDef, "Default" );
      l[i].queue.Open( session, l[i].name );

      for ( j = 0; j < 20; ++j ) {
         sprintf(s, "%d", j);
         data = string("Inserted data item = ") + s;
         sprintf(s, "%d", i);
         data += string(" in queue = ") + s;
         l[i].queue.Push( data.c_str(), data.length() );
      }
   }

   session.Commit();
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void PopulateQueues( Session & session, vector<myQueue> & q )
{
   int i, j;
   string data;
   char s[4];
   psoObjectDefinition queueDef = { PSO_QUEUE, 0, 0, 0 };
   
   for ( i = 0; i < NUM_QUEUES; ++i ) {
      cout << " i = " << i << ", " << q[i].name << ", " << &session << endl;
      session.CreateQueue( q[i].name, queueDef, "Default" );
      q[i].queue.Open( session, q[i].name );

      for ( j = 0; j < 20; ++j ) {
         sprintf(s, "%d", j);
         data = string("Inserted data item = ") + s;
         sprintf(s, "%d", i);
         data += string(" in queue = ") + s;
         q[i].queue.Push( data.c_str(), data.length() );
      }
   }

   session.Commit();
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int main()
{
   Process process;
   Session session;
   int i, rc;
   psoObjectDefinition folderDef;

   memset( &folderDef, 0, sizeof folderDef );
   folderDef.type = PSO_FOLDER;
   
   try {
      process.Init( "10701", "InjectErrors" );
      session.Init();
      session.CreateFolder( foldername );
   }
   catch( pso::Exception exc ) {
      rc = exc.ErrorCode();
      if ( rc == PSO_OBJECT_ALREADY_PRESENT ) {
         CleanupPreviousRun( session );
      }
      else {
         cerr << "Init Photon failed, error = " << exc.Message() << endl;
         if ( rc == PSO_CONNECT_ERROR ) cerr << "Is the server running?" << endl;
         return 1;
      }
   }
   
   cout << " ------- Photon defects injector ------- " << endl << endl;
   cout << " This program will inject pseudo-random defects in a shared memory." << endl << endl;

   vector<myQueue> q( NUM_QUEUES, myQueue() );
   vector<myMap>   h( NUM_MAPS,   myMap() );
   vector<myLifo>  l( NUM_LIFOS,  myLifo() );

   for ( i = 0; i < NUM_QUEUES; ++ i ) {
      q[i].name += ('0' + i/10 );
      q[i].name += ('0' + (i%10) );
   }

   for ( i = 0; i < NUM_MAPS; ++i ) {
      h[i].name += ('0' + i/10 );
      h[i].name += ('0' + (i%10) );
   }
   for ( i = 0; i < NUM_LIFOS; ++i ) {
      l[i].name += ('0' + i/10 );
      l[i].name += ('0' + (i%10) );
   }
   
   try {
      PopulateQueues( session, q );
      PopulateHashMaps( session, h );
      PopulateLifos( session, l );
   }
   catch( pso::Exception exc ) {
      cerr << "Creating and populating the objects failed, error = " << exc.Message() << endl;
      return 1;
   }
   cout << "Queues, maps, etc. are created and populated." << endl << endl;

   rc = AddDefectsQueues( q );
   if ( rc != 0 ) {
      cerr << "Adding defect to queues failed!" << endl;
      return 1;
   }
   cout << "All defects were added to queues." << endl << endl;

   rc = AddDefectsHashMaps( h );
   if ( rc != 0 ) {
      cerr << "Adding defect to hash maps failed!" << endl;
      return 1;
   }
   cout << "All defects were added to hash maps." << endl << endl;

   rc = AddDefectsLifos( l );
   if ( rc != 0 ) {
      cerr << "Adding defect to LIFO queues failed!" << endl;
      return 1;
   }
   cout << "All defects were added to LIFO queues." << endl << endl;
   
   return 0;
}

