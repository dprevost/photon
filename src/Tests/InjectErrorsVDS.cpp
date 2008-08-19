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

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#include "Common/Common.h"
#include <photon/vds>
#include <iostream>
#include <vector>

#include "API/api.h"
#include "API/HashMap.h"
#include "API/Queue.h"
#include "API/Lifo.h"

using namespace std;

#define NUM_MAPS    8
#define NUM_QUEUES 10
#define NUM_LIFOS  10

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

struct myQueue
{
   myQueue( vdsSession & session )
      : queue ( session ),
        name  ( "TestFolder/Queue" ) {}

   vdsQueue queue;
   string   name;
};

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

struct myMap
{
   myMap( vdsSession & session )
      : map  ( session ),
        name ( "TestFolder/HashMap" ) {}

   vdsHashMap map;
   string     name;
};

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

struct myLifo
{
   myLifo( vdsSession & session )
      : queue ( session ),
        name  ( "TestFolder/Lifo" ) {}

   vdsLifo queue;
   string  name;
};

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

string foldername("TestFolder");

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int AddDefectsHashMaps( vector<myMap> & h )
{
   int api_offset = -1;
   struct psaHashMap ** apiHashMap;
   struct psnHashMap * pHashMap;
   psnTxStatus * txItemStatus, * txHashMapStatus;
   unsigned long i, ** apiObj;
   ptrdiff_t offset, previousOffset;
   psnHashItem * pItem;
   ptrdiff_t* pArray;   
   bool ok;
   
   // Using a (void *) cast to eliminate a gcc warning (dereferencing 
   // type-punned pointer will break strict-aliasing rules)
   apiObj = (unsigned long **) ( (void *) &h[0].map );

   // We have to go around the additional data member inserted by the
   // compiler for the virtual table (true for g++)
   for ( i = 0; i < sizeof(vdsHashMap)/sizeof(void*); ++i, apiObj++ ) {
      if ( **apiObj == PSA_HASH_MAP ) {
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
   apiHashMap = (psaHashMap **) ( (unsigned char *) &h[1].map + api_offset );
   pHashMap = (psnHashMap *) (*apiHashMap)->object.pMyVdsObject;
   pHashMap->nodeObject.txCounter++;
   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, psnTxStatus );
   txHashMapStatus->usageCounter++;

   ok = psnHashGetFirst( &pHashMap->hashObj, &offset );
   i = 0;
   while ( ok ) {
      previousOffset = offset;
      ok = psnHashGetNext( &pHashMap->hashObj,
                            previousOffset,
                            &offset );
      i++;
      if ( i >= 6 ) break;
   }
   if ( ! ok ) {
      cerr << "Iteration error in " << h[1].name << endl;
      return -1;
   }
   GET_PTR( pItem, offset, psnHashItem );
   txItemStatus = &pItem->txStatus;
   txItemStatus->usageCounter++;

   cout << "Defect for " << h[2].name << ": object added - not committed" << endl;
   apiHashMap = (psaHashMap **) ( (unsigned char *) &h[2].map + api_offset );
   pHashMap = (psnHashMap *) (*apiHashMap)->object.pMyVdsObject;
   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, psnTxStatus );
   txHashMapStatus->txOffset = SET_OFFSET( pHashMap ); 
   txHashMapStatus->status = PSN_TXS_ADDED;

   cout << "Defect for " << h[3].name << ": 5 items removed-committed," << endl;
   cout << "                                  4 items removed - not committed," << endl;
   cout << "                                  9 items added - not committed" << endl;
   apiHashMap = (psaHashMap **) ( (unsigned char *) &h[3].map + api_offset );
   pHashMap = (psnHashMap *) (*apiHashMap)->object.pMyVdsObject;

   ok = psnHashGetFirst( &pHashMap->hashObj, &offset );
   i = 0;
   while ( ok ) {      
      GET_PTR( pItem, offset, psnHashItem );
      txItemStatus = &pItem->txStatus;

      if ( i < 5 ) { /* removed committed */
         txItemStatus->txOffset = SET_OFFSET( pHashMap ); 
         txItemStatus->status = PSN_TXS_DESTROYED_COMMITTED;
      }
      else if ( i < 9 ) { /* removed uncommitted */
         txItemStatus->txOffset = SET_OFFSET( pHashMap ); 
         txItemStatus->status = PSN_TXS_DESTROYED;
      }
      else if ( i >= 11 ) { /* Added */
         txItemStatus->txOffset = SET_OFFSET( pHashMap ); 
         txItemStatus->status = PSN_TXS_ADDED;
      }

      previousOffset = offset;
      ok = psnHashGetNext( &pHashMap->hashObj,
                            previousOffset,
                            &offset );
      i++;

   }
   if ( i < 20 ) {
      cerr << "Iteration error in " << h[3].name << endl;
      return -1;
   }
   
   cout << "Defect for " << h[4].name << ": object locked" << endl;
   apiHashMap = (psaHashMap **) ( (unsigned char *) &h[4].map + api_offset );
   pHashMap = (psnHashMap *) (*apiHashMap)->object.pMyVdsObject;
   if ( ! pscTryAcquireProcessLock(&pHashMap->memObject.lock, getpid(), 0) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }

   cout << "Defect for " << h[5].name << ": items - invalid offset" << endl;
   apiHashMap = (psaHashMap **) ( (unsigned char *) &h[5].map + api_offset );
   pHashMap = (psnHashMap *) (*apiHashMap)->object.pMyVdsObject;
   if ( ! pscTryAcquireProcessLock(&pHashMap->memObject.lock, getpid(), 0) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }

   GET_PTR( pArray, pHashMap->hashObj.arrayOffset, ptrdiff_t );

   for ( i = 0; i < g_psnArrayLengths[pHashMap->hashObj.lengthIndex]; ++i ) {
      if ( pArray[i] != PSN_NULL_OFFSET ) pArray[i] = 0;
   }
   
   cout << "Defect for " << h[6].name << ": item - invalid key length" << endl;
   apiHashMap = (psaHashMap **) ( (unsigned char *) &h[6].map + api_offset );
   pHashMap = (psnHashMap *) (*apiHashMap)->object.pMyVdsObject;
   if ( ! pscTryAcquireProcessLock(&pHashMap->memObject.lock, getpid(), 0) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }
   ok = psnHashGetFirst( &pHashMap->hashObj, &offset );
   i = 0;
   while ( ok ) {
      previousOffset = offset;
      ok = psnHashGetNext( &pHashMap->hashObj,
                            previousOffset,
                            &offset );
      i++;
      if ( i >= 6 ) break;
   }
   if ( ! ok ) {
      cerr << "Iteration error in " << h[6].name << endl;
      return -1;
   }
   GET_PTR( pItem, offset, psnHashItem );
   pItem->keyLength = 0;
   
   cout << "Defect for " << h[7].name << ": item - invalid data offset" << endl;
   apiHashMap = (psaHashMap **) ( (unsigned char *) &h[7].map + api_offset );
   pHashMap = (psnHashMap *) (*apiHashMap)->object.pMyVdsObject;
   if ( ! pscTryAcquireProcessLock(&pHashMap->memObject.lock, getpid(), 0) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }
   ok = psnHashGetFirst( &pHashMap->hashObj, &offset );
   i = 0;
   while ( ok ) {
      previousOffset = offset;
      ok = psnHashGetNext( &pHashMap->hashObj,
                            previousOffset,
                            &offset );
      i++;
      if ( i >= 6 ) break;
   }
   if ( ! ok ) {
      cerr << "Iteration error in " << h[7].name << endl;
      return -1;
   }
   GET_PTR( pItem, offset, psnHashItem );
   pItem->dataOffset = 0;

   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int AddDefectsLifos( vector<myLifo> & l )
{
   int api_offset = -1;
   struct psaLifo ** apiLifo;
   struct psnQueue * pQueue;
   psnTxStatus * txItemStatus, * txQueueStatus;
   unsigned long i, ** apiObj;
   psnLinkNode * pNode = NULL, *pSavedNode = NULL;
   psnQueueItem* pQueueItem = NULL;
   bool okList;
   
   // Using a (void *) cast to eliminate a gcc warning (dereferencing 
   // type-punned pointer will break strict-aliasing rules)
   apiObj = (unsigned long **) ( (void *) &l[0].queue );
   // We have to go around the additional data member inserted by the
   // compiler for the virtual table
   for ( i = 0; i < sizeof(vdsQueue)/sizeof(void*); ++i, apiObj++ ) {
      if ( **apiObj == PSA_LIFO ) {
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
   apiLifo = (psaLifo **) ( (unsigned char *) &l[1].queue + api_offset );
   pQueue = (psnQueue *) (*apiLifo)->object.pMyVdsObject;
   pQueue->nodeObject.txCounter++;
   GET_PTR( txQueueStatus, pQueue->nodeObject.txStatusOffset, psnTxStatus );
   txQueueStatus->usageCounter++;
   
   okList = psnLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
   i = 0;
   while ( okList ) {
      okList =  psnLinkedListPeakNext( &pQueue->listOfElements, 
                                        pNode, 
                                        &pNode );
      i++;
      if ( i >= 6 ) break;
   }
   if ( ! okList ) {
      cerr << "Iteration error in " << l[1].name << endl;
      return -1;
   }
   pQueueItem = (psnQueueItem*) 
      ((char*)pNode - offsetof( psnQueueItem, node ));
   txItemStatus = &pQueueItem->txStatus;
   txItemStatus->usageCounter++;

   cout << "Defect for " << l[2].name << ": object added - not committed" << endl;
   apiLifo = (psaLifo **) ( (unsigned char *) &l[2].queue + api_offset );
   pQueue = (psnQueue *) (*apiLifo)->object.pMyVdsObject;
   GET_PTR( txQueueStatus, pQueue->nodeObject.txStatusOffset, psnTxStatus );
   txQueueStatus->txOffset = SET_OFFSET( pQueue ); 
   txQueueStatus->status = PSN_TXS_ADDED;

   // Queue 2. Defects: 
   //  - Items added (not committed) and items removed (committed + non-comm)
   cout << "Defect for " << l[3].name << ": 5 items removed-committed," << endl;
   cout << "                                  4 items removed - not committed," << endl;
   cout << "                                  9 items added - not committed" << endl;
   apiLifo = (psaLifo **) ( (unsigned char *) &l[3].queue + api_offset );
   pQueue = (psnQueue *) (*apiLifo)->object.pMyVdsObject;

   okList = psnLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
   i = 0;
   while ( okList ) {
      pQueueItem = (psnQueueItem*) 
         ((char*)pNode - offsetof( psnQueueItem, node ));
      txItemStatus = &pQueueItem->txStatus;

      if ( i < 5 ) { /* removed committed */
         txItemStatus->txOffset = SET_OFFSET( pQueue ); 
         txItemStatus->status = PSN_TXS_DESTROYED_COMMITTED;
      }
      else if ( i < 9 ) { /* removed uncommitted */
         txItemStatus->txOffset = SET_OFFSET( pQueue ); 
         txItemStatus->status = PSN_TXS_DESTROYED;
      }
      else if ( i >= 11 ) { /* Added */
         txItemStatus->txOffset = SET_OFFSET( pQueue ); 
         txItemStatus->status = PSN_TXS_ADDED;
      }
      
      okList =  psnLinkedListPeakNext( &pQueue->listOfElements, 
                                             pNode, 
                                             &pNode );
      i++;
   }
   
   cout << "Defect for " << l[4].name << ": object locked" << endl;
   apiLifo = (psaLifo **) ( (unsigned char *) &l[4].queue + api_offset );
   pQueue = (psnQueue *) (*apiLifo)->object.pMyVdsObject;
   if ( ! pscTryAcquireProcessLock(&pQueue->memObject.lock, getpid(), 0) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }

   cout << "Defect for " << l[5].name << ": broken forward link" << endl;
   apiLifo = (psaLifo **) ( (unsigned char *) &l[5].queue + api_offset );
   pQueue = (psnQueue *) (*apiLifo)->object.pMyVdsObject;
   if ( ! pscTryAcquireProcessLock(&pQueue->memObject.lock, getpid(), 0) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }

   okList = psnLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
   i = 0;
   while ( okList ) {
      okList =  psnLinkedListPeakNext( &pQueue->listOfElements, 
                                        pNode, 
                                        &pNode );
      i++;
      if ( i == 9 ) {
         pNode->nextOffset = PSN_NULL_OFFSET;
         break;
      }
   }
   if ( ! okList ) {
      cerr << "Iteration error in " << l[5].name << endl;
      return -1;
   }

   cout << "Defect for " << l[6].name << ": broken backward link" << endl;
   apiLifo = (psaLifo **) ( (unsigned char *) &l[6].queue + api_offset );
   pQueue = (psnQueue *) (*apiLifo)->object.pMyVdsObject;
   if ( ! pscTryAcquireProcessLock(&pQueue->memObject.lock, getpid(), 0) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }

   okList = psnLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
   i = 0;
   while ( okList ) {
      okList =  psnLinkedListPeakNext( &pQueue->listOfElements, 
                                        pNode, 
                                        &pNode );
      i++;
      if ( i == 9 ) {
         pNode->previousOffset = PSN_NULL_OFFSET;
         break;
      }
   }
   if ( ! okList ) {
      cerr << "Iteration error in " << l[6].name << endl;
      return -1;
   }
   
   cout << "Defect for " << l[7].name << ": 2 broken forward links" << endl;
   apiLifo = (psaLifo **) ( (unsigned char *) &l[7].queue + api_offset );
   pQueue = (psnQueue *) (*apiLifo)->object.pMyVdsObject;
   if ( ! pscTryAcquireProcessLock(&pQueue->memObject.lock, getpid(), 0) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }

   okList = psnLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
   i = 0;
   while ( okList ) {
      okList =  psnLinkedListPeakNext( &pQueue->listOfElements, 
                                        pNode, 
                                        &pNode );
      i++;
      if ( i == 9 ) pSavedNode = pNode;
      if ( i == 13 ) {
         pSavedNode->nextOffset = PSN_NULL_OFFSET;
         pNode->nextOffset = PSN_NULL_OFFSET;
         break;
      }
   }
   if ( ! okList ) {
      cerr << "Iteration error in " << l[7].name << endl;
      return -1;
   }

   cout << "Defect for " << l[8].name << ": 2 broken backward links" << endl;
   apiLifo = (psaLifo **) ( (unsigned char *) &l[8].queue + api_offset );
   pQueue = (psnQueue *) (*apiLifo)->object.pMyVdsObject;
   if ( ! pscTryAcquireProcessLock(&pQueue->memObject.lock, getpid(), 0) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }

   okList = psnLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
   i = 0;
   while ( okList ) {
      okList =  psnLinkedListPeakNext( &pQueue->listOfElements, 
                                        pNode, 
                                        &pNode );
      i++;
      if ( i == 9 ) pSavedNode = pNode;
      if ( i == 13 ) {
         pSavedNode->previousOffset = PSN_NULL_OFFSET;
         pNode->previousOffset = PSN_NULL_OFFSET;
         break;
      }
   }
   if ( ! okList ) {
      cerr << "Iteration error in " << l[8].name << endl;
      return -1;
   }

   cout << "Defect for " << l[9].name << ": broken bw+fw links (eq)" << endl;
   apiLifo = (psaLifo **) ( (unsigned char *) &l[9].queue + api_offset );
   pQueue = (psnQueue *) (*apiLifo)->object.pMyVdsObject;
   if ( ! pscTryAcquireProcessLock(&pQueue->memObject.lock, getpid(), 0) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }

   okList = psnLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
   i = 0;
   while ( okList ) {
      okList =  psnLinkedListPeakNext( &pQueue->listOfElements, 
                                        pNode, 
                                        &pNode );
      i++;
      if ( i == 9 ) {
         pNode->previousOffset = PSN_NULL_OFFSET;
         pNode->nextOffset = PSN_NULL_OFFSET;
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
   struct psaQueue ** apiQueue;
   struct psnQueue * pQueue;
   psnTxStatus * txItemStatus, * txQueueStatus;
   unsigned long i, ** apiObj;
   psnLinkNode * pNode = NULL, *pSavedNode = NULL;
   psnQueueItem* pQueueItem = NULL;
   bool okList;
   
   // Using a (void *) cast to eliminate a gcc warning (dereferencing 
   // type-punned pointer will break strict-aliasing rules)
   apiObj = (unsigned long **) ( (void *) &q[0].queue );
   // We have to go around the additional data member inserted by the
   // compiler for the virtual table
   for ( i = 0; i < sizeof(vdsQueue)/sizeof(void*); ++i, apiObj++ ) {
      if ( **apiObj == PSA_QUEUE ) {
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
   apiQueue = (psaQueue **) ( (unsigned char *) &q[1].queue + api_offset );
   pQueue = (psnQueue *) (*apiQueue)->object.pMyVdsObject;
   pQueue->nodeObject.txCounter++;
   GET_PTR( txQueueStatus, pQueue->nodeObject.txStatusOffset, psnTxStatus );
   txQueueStatus->usageCounter++;
   
   okList = psnLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
   i = 0;
   while ( okList ) {
      okList =  psnLinkedListPeakNext( &pQueue->listOfElements, 
                                        pNode, 
                                        &pNode );
      i++;
      if ( i >= 6 ) break;
   }
   if ( ! okList ) {
      cerr << "Iteration error in " << q[1].name << endl;
      return -1;
   }
   pQueueItem = (psnQueueItem*) 
      ((char*)pNode - offsetof( psnQueueItem, node ));
   txItemStatus = &pQueueItem->txStatus;
   txItemStatus->usageCounter++;

   cout << "Defect for " << q[2].name << ": object added - not committed" << endl;
   apiQueue = (psaQueue **) ( (unsigned char *) &q[2].queue + api_offset );
   pQueue = (psnQueue *) (*apiQueue)->object.pMyVdsObject;
   GET_PTR( txQueueStatus, pQueue->nodeObject.txStatusOffset, psnTxStatus );
   txQueueStatus->txOffset = SET_OFFSET( pQueue ); 
   txQueueStatus->status = PSN_TXS_ADDED;

   // Queue 2. Defects: 
   //  - Items added (not committed) and items removed (committed + non-comm)
   cout << "Defect for " << q[3].name << ": 5 items removed-committed," << endl;
   cout << "                                  4 items removed - not committed," << endl;
   cout << "                                  9 items added - not committed" << endl;
   apiQueue = (psaQueue **) ( (unsigned char *) &q[3].queue + api_offset );
   pQueue = (psnQueue *) (*apiQueue)->object.pMyVdsObject;

   okList = psnLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
   i = 0;
   while ( okList ) {
      pQueueItem = (psnQueueItem*) 
         ((char*)pNode - offsetof( psnQueueItem, node ));
      txItemStatus = &pQueueItem->txStatus;

      if ( i < 5 ) { /* removed committed */
         txItemStatus->txOffset = SET_OFFSET( pQueue ); 
         txItemStatus->status = PSN_TXS_DESTROYED_COMMITTED;
      }
      else if ( i < 9 ) { /* removed uncommitted */
         txItemStatus->txOffset = SET_OFFSET( pQueue ); 
         txItemStatus->status = PSN_TXS_DESTROYED;
      }
      else if ( i >= 11 ) { /* Added */
         txItemStatus->txOffset = SET_OFFSET( pQueue ); 
         txItemStatus->status = PSN_TXS_ADDED;
      }
      
      okList =  psnLinkedListPeakNext( &pQueue->listOfElements, 
                                             pNode, 
                                             &pNode );
      i++;
   }
   
   cout << "Defect for " << q[4].name << ": object locked" << endl;
   apiQueue = (psaQueue **) ( (unsigned char *) &q[4].queue + api_offset );
   pQueue = (psnQueue *) (*apiQueue)->object.pMyVdsObject;
   if ( ! pscTryAcquireProcessLock(&pQueue->memObject.lock, getpid(), 0) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }

   cout << "Defect for " << q[5].name << ": broken forward link" << endl;
   apiQueue = (psaQueue **) ( (unsigned char *) &q[5].queue + api_offset );
   pQueue = (psnQueue *) (*apiQueue)->object.pMyVdsObject;
   if ( ! pscTryAcquireProcessLock(&pQueue->memObject.lock, getpid(), 0) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }

   okList = psnLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
   i = 0;
   while ( okList ) {
      okList =  psnLinkedListPeakNext( &pQueue->listOfElements, 
                                        pNode, 
                                        &pNode );
      i++;
      if ( i == 9 ) {
         pNode->nextOffset = PSN_NULL_OFFSET;
         break;
      }
   }
   if ( ! okList ) {
      cerr << "Iteration error in " << q[5].name << endl;
      return -1;
   }

   cout << "Defect for " << q[6].name << ": broken backward link" << endl;
   apiQueue = (psaQueue **) ( (unsigned char *) &q[6].queue + api_offset );
   pQueue = (psnQueue *) (*apiQueue)->object.pMyVdsObject;
   if ( ! pscTryAcquireProcessLock(&pQueue->memObject.lock, getpid(), 0) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }

   okList = psnLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
   i = 0;
   while ( okList ) {
      okList =  psnLinkedListPeakNext( &pQueue->listOfElements, 
                                        pNode, 
                                        &pNode );
      i++;
      if ( i == 9 ) {
         pNode->previousOffset = PSN_NULL_OFFSET;
         break;
      }
   }
   if ( ! okList ) {
      cerr << "Iteration error in " << q[6].name << endl;
      return -1;
   }
   
   cout << "Defect for " << q[7].name << ": 2 broken forward links" << endl;
   apiQueue = (psaQueue **) ( (unsigned char *) &q[7].queue + api_offset );
   pQueue = (psnQueue *) (*apiQueue)->object.pMyVdsObject;
   if ( ! pscTryAcquireProcessLock(&pQueue->memObject.lock, getpid(), 0) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }

   okList = psnLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
   i = 0;
   while ( okList ) {
      okList =  psnLinkedListPeakNext( &pQueue->listOfElements, 
                                        pNode, 
                                        &pNode );
      i++;
      if ( i == 9 ) pSavedNode = pNode;
      if ( i == 13 ) {
         pSavedNode->nextOffset = PSN_NULL_OFFSET;
         pNode->nextOffset = PSN_NULL_OFFSET;
         break;
      }
   }
   if ( ! okList ) {
      cerr << "Iteration error in " << q[7].name << endl;
      return -1;
   }

   cout << "Defect for " << q[8].name << ": 2 broken backward links" << endl;
   apiQueue = (psaQueue **) ( (unsigned char *) &q[8].queue + api_offset );
   pQueue = (psnQueue *) (*apiQueue)->object.pMyVdsObject;
   if ( ! pscTryAcquireProcessLock(&pQueue->memObject.lock, getpid(), 0) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }

   okList = psnLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
   i = 0;
   while ( okList ) {
      okList =  psnLinkedListPeakNext( &pQueue->listOfElements, 
                                        pNode, 
                                        &pNode );
      i++;
      if ( i == 9 ) pSavedNode = pNode;
      if ( i == 13 ) {
         pSavedNode->previousOffset = PSN_NULL_OFFSET;
         pNode->previousOffset = PSN_NULL_OFFSET;
         break;
      }
   }
   if ( ! okList ) {
      cerr << "Iteration error in " << q[8].name << endl;
      return -1;
   }

   cout << "Defect for " << q[9].name << ": broken bw+fw links (eq)" << endl;
   apiQueue = (psaQueue **) ( (unsigned char *) &q[9].queue + api_offset );
   pQueue = (psnQueue *) (*apiQueue)->object.pMyVdsObject;
   if ( ! pscTryAcquireProcessLock(&pQueue->memObject.lock, getpid(), 0) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }

   okList = psnLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
   i = 0;
   while ( okList ) {
      okList =  psnLinkedListPeakNext( &pQueue->listOfElements, 
                                        pNode, 
                                        &pNode );
      i++;
      if ( i == 9 ) {
         pNode->previousOffset = PSN_NULL_OFFSET;
         pNode->nextOffset = PSN_NULL_OFFSET;
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

void CleanupPreviousRun( vdsSession & session )
{
   vdsFolder folder( session );
   vdsFolderEntry entry;
   int ok;
   string s;
   
   try {
      folder.Open( foldername );
      ok = folder.GetFirst( &entry );
      while ( ok == 0 ) {
         s = foldername + "/" + entry.name;
         session.DestroyObject( s );
         ok = folder.GetNext( &entry );
      }
      session.Commit();
   }
   catch ( vdsException exc ) {
      cerr << "Cleanup of previous session failed, error = " << exc.Message() << endl;
      exit(1);
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void PopulateHashMaps( vdsSession & session, vector<myMap> & h )
{
   int i, j;
   string data, key;
   char s[4];
   vdsObjectDefinition mapDef = { 
      VDS_HASH_MAP,
      1, 
      { VDS_KEY_VAR_STRING, 0, 1, 200 }, 
      { { "Field_1", VDS_VAR_STRING, 0, 1, 200, 0, 0 } } 
   };
   
   for ( i = 0; i < NUM_MAPS; ++i ) {
      session.CreateObject( h[i].name, &mapDef );
      h[i].map.Open( h[i].name );

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

void PopulateLifos( vdsSession & session, vector<myLifo> & l )
{
   int i, j;
   string data;
   char s[4];
   vdsObjectDefinition queueDef = { 
      VDS_LIFO,
      1, 
      { VDS_KEY_INTEGER, 0, 0, 0 }, 
      { { "Field_1", VDS_VAR_STRING, 0, 4, 100, 0, 0 } } 
   };
   
   for ( i = 0; i < NUM_LIFOS; ++i ) {
      session.CreateObject( l[i].name, &queueDef );
      l[i].queue.Open( l[i].name );

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

void PopulateQueues( vdsSession & session, vector<myQueue> & q )
{
   int i, j;
   string data;
   char s[4];
   vdsObjectDefinition queueDef = { 
      VDS_QUEUE,
      1, 
      { VDS_KEY_INTEGER, 0, 0, 0 }, 
      { { "Field_1", VDS_VAR_STRING, 0, 4, 100, 0, 0 } } 
   };
   
   for ( i = 0; i < NUM_QUEUES; ++i ) {
      cout << " i = " << i << ", " << q[i].name << ", " << &session << endl;
      session.CreateObject( q[i].name, &queueDef );
      q[i].queue.Open( q[i].name );

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
   vdsProcess process;
   vdsSession session;
   int i, rc;
   vdsObjectDefinition folderDef;

   memset( &folderDef, 0, sizeof folderDef );
   folderDef.type = VDS_FOLDER;
   
   try {
      process.Init( "10701" );
      session.Init();
      session.CreateObject( foldername, &folderDef );
   }
   catch( vdsException exc ) {
      rc = exc.ErrorCode();
      if ( rc == VDS_OBJECT_ALREADY_PRESENT ) {
         CleanupPreviousRun( session );
      }
      else {
         cerr << "Init VDSF failed, error = " << exc.Message() << endl;
         if ( rc == VDS_CONNECT_ERROR ) cerr << "Is the watchdog running?" << endl;
         return 1;
      }
   }
   
   cout << " ------- VDSF defects injector ------- " << endl << endl;
   cout << " This program will inject pseudo-random defects in a VDS." << endl << endl;

   vector<myQueue>   q( NUM_QUEUES,   myQueue(session) );
   vector<myMap>   h( NUM_MAPS,   myMap(session) );
   vector<myLifo>  l( NUM_LIFOS,  myLifo(session) );

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
   catch( vdsException exc ) {
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

