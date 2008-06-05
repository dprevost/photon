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
#include <vdsf/vds>
#include <iostream>
#include <vector>

#include "API/api.h"
#include "API/HashMap.h"
#include "API/Queue.h"

using namespace std;

#define NUM_MAPS    8
#define NUM_QUEUES 10

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

struct myQueue
{
   myQueue( vdsSession & session )
      : queue ( session ),
        name  ( "Test Folder/Queue " ) {}

   vdsQueue queue;
   string   name;
};

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

struct myMap
{
   myMap( vdsSession & session )
      : map  ( session ),
        name ( "Test Folder/HashMap " ) {}

   vdsHashMap map;
   string     name;
};

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

string foldername("Test Folder");

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int AddDefectsHashMaps( vector<myMap> & h )
{
   int api_offset = -1;
   struct vdsaHashMap ** apiHashMap;
   struct vdseHashMap * pHashMap;
   vdseTxStatus * txItemStatus, * txHashMapStatus;
   unsigned long i, ** apiObj;
   enum ListErrors listErrCode;
   size_t bucket, previousBucket;
   ptrdiff_t offset, previousOffset;
   vdseHashItem * pItem;
   ptrdiff_t* pArray;   

   // Using a (void *) cast to eliminate a gcc warning (dereferencing 
   // type-punned pointer will break strict-aliasing rules)
   apiObj = (unsigned long **) ( (void *) &h[0].map );

   // We have to go around the additional data member inserted by the
   // compiler for the virtual table (true for g++)
   for ( i = 0; i < sizeof(vdsHashMap)/sizeof(void*); ++i, apiObj++ ) {
      if ( **apiObj == VDSA_HASH_MAP ) {
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
   apiHashMap = (vdsaHashMap **) ( (unsigned char *) &h[1].map + api_offset );
   pHashMap = (vdseHashMap *) (*apiHashMap)->object.pMyVdsObject;
   pHashMap->nodeObject.txCounter++;
   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, vdseTxStatus );
   txHashMapStatus->usageCounter++;

   listErrCode = vdseHashGetFirst( &pHashMap->hashObj,
                                   &bucket, 
                                   &offset );
   i = 0;
   while ( listErrCode == LIST_OK ) {
      previousBucket = bucket;
      previousOffset = offset;
      listErrCode = vdseHashGetNext( &pHashMap->hashObj,
                                     previousBucket,
                                     previousOffset,
                                     &bucket, 
                                     &offset );
      i++;
      if ( i >= 6 ) break;
   }
   if ( listErrCode != LIST_OK ) {
      cerr << "Iteration error in " << h[1].name << ", list err = " << listErrCode << endl;
      return -1;
   }
   GET_PTR( pItem, offset, vdseHashItem );
   txItemStatus = &pItem->txStatus;
   txItemStatus->usageCounter++;

   cout << "Defect for " << h[2].name << ": object added - not committed" << endl;
   apiHashMap = (vdsaHashMap **) ( (unsigned char *) &h[2].map + api_offset );
   pHashMap = (vdseHashMap *) (*apiHashMap)->object.pMyVdsObject;
   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, vdseTxStatus );
   txHashMapStatus->txOffset = SET_OFFSET( pHashMap ); 
   txHashMapStatus->enumStatus = VDSE_TXS_ADDED;

   cout << "Defect for " << h[3].name << ": 5 items removed-committed," << endl;
   cout << "                                  4 items removed - not committed," << endl;
   cout << "                                  9 items added - not committed" << endl;
   apiHashMap = (vdsaHashMap **) ( (unsigned char *) &h[3].map + api_offset );
   pHashMap = (vdseHashMap *) (*apiHashMap)->object.pMyVdsObject;

   listErrCode = vdseHashGetFirst( &pHashMap->hashObj,
                                   &bucket, 
                                   &offset );
   i = 0;
   while ( listErrCode == LIST_OK ) {      
      GET_PTR( pItem, offset, vdseHashItem );
      txItemStatus = &pItem->txStatus;

      if ( i < 5 ) { /* removed committed */
         txItemStatus->txOffset = SET_OFFSET( pHashMap ); 
         txItemStatus->enumStatus = VDSE_TXS_DESTROYED_COMMITTED;
      }
      else if ( i < 9 ) { /* removed uncommitted */
         txItemStatus->txOffset = SET_OFFSET( pHashMap ); 
         txItemStatus->enumStatus = VDSE_TXS_DESTROYED;
      }
      else if ( i >= 11 ) { /* Added */
         txItemStatus->txOffset = SET_OFFSET( pHashMap ); 
         txItemStatus->enumStatus = VDSE_TXS_ADDED;
      }

      previousBucket = bucket;
      previousOffset = offset;
      listErrCode = vdseHashGetNext( &pHashMap->hashObj,
                                     previousBucket,
                                     previousOffset,
                                     &bucket, 
                                     &offset );
      i++;

   }
   if ( listErrCode != LIST_END_OF_LIST ) {
      cerr << "Iteration error in " << h[3].name << ", list err = " << listErrCode << endl;
      return -1;
   }
   
   cout << "Defect for " << h[4].name << ": object locked" << endl;
   apiHashMap = (vdsaHashMap **) ( (unsigned char *) &h[4].map + api_offset );
   pHashMap = (vdseHashMap *) (*apiHashMap)->object.pMyVdsObject;
   if ( vdscTryAcquireProcessLock ( &pHashMap->memObject.lock, getpid(), 0 ) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }

   cout << "Defect for " << h[5].name << ": items - invalid offset" << endl;
   apiHashMap = (vdsaHashMap **) ( (unsigned char *) &h[5].map + api_offset );
   pHashMap = (vdseHashMap *) (*apiHashMap)->object.pMyVdsObject;
   if ( vdscTryAcquireProcessLock ( &pHashMap->memObject.lock, getpid(), 0 ) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }

   GET_PTR( pArray, pHashMap->hashObj.arrayOffset, ptrdiff_t );

   for ( i = 0; i < g_vdseArrayLengths[pHashMap->hashObj.lengthIndex]; ++i ) {
      if ( pArray[i] != NULL_OFFSET ) pArray[i] = 0;
   }
   
   cout << "Defect for " << h[6].name << ": item - invalid key length" << endl;
   apiHashMap = (vdsaHashMap **) ( (unsigned char *) &h[6].map + api_offset );
   pHashMap = (vdseHashMap *) (*apiHashMap)->object.pMyVdsObject;
   if ( vdscTryAcquireProcessLock ( &pHashMap->memObject.lock, getpid(), 0 ) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }
   listErrCode = vdseHashGetFirst( &pHashMap->hashObj,
                                   &bucket, 
                                   &offset );
   i = 0;
   while ( listErrCode == LIST_OK ) {
      previousBucket = bucket;
      previousOffset = offset;
      listErrCode = vdseHashGetNext( &pHashMap->hashObj,
                                     previousBucket,
                                     previousOffset,
                                     &bucket, 
                                     &offset );
      i++;
      if ( i >= 6 ) break;
   }
   if ( listErrCode != LIST_OK ) {
      cerr << "Iteration error in " << h[6].name << ", list err = " << listErrCode << endl;
      return -1;
   }
   GET_PTR( pItem, offset, vdseHashItem );
   pItem->keyLength = 0;
   
   cout << "Defect for " << h[7].name << ": item - invalid data offset" << endl;
   apiHashMap = (vdsaHashMap **) ( (unsigned char *) &h[7].map + api_offset );
   pHashMap = (vdseHashMap *) (*apiHashMap)->object.pMyVdsObject;
   if ( vdscTryAcquireProcessLock ( &pHashMap->memObject.lock, getpid(), 0 ) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }
   listErrCode = vdseHashGetFirst( &pHashMap->hashObj,
                                   &bucket, 
                                   &offset );
   i = 0;
   while ( listErrCode == LIST_OK ) {
      previousBucket = bucket;
      previousOffset = offset;
      listErrCode = vdseHashGetNext( &pHashMap->hashObj,
                                     previousBucket,
                                     previousOffset,
                                     &bucket, 
                                     &offset );
      i++;
      if ( i >= 6 ) break;
   }
   if ( listErrCode != LIST_OK ) {
      cerr << "Iteration error in " << h[7].name << ", list err = " << listErrCode << endl;
      return -1;
   }
   GET_PTR( pItem, offset, vdseHashItem );
   pItem->dataOffset = 0;

   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int AddDefectsQueues( vector<myQueue> & q )
{
   int api_offset = -1;
   struct vdsaQueue ** apiQueue;
   struct vdseQueue * pQueue;
   vdseTxStatus * txItemStatus, * txQueueStatus;
   unsigned long i, ** apiObj;
   enum ListErrors listErrCode;
   vdseLinkNode * pNode = NULL, *pSavedNode = NULL;
   vdseQueueItem* pQueueItem = NULL;
   
   // Using a (void *) cast to eliminate a gcc warning (dereferencing 
   // type-punned pointer will break strict-aliasing rules)
   apiObj = (unsigned long **) ( (void *) &q[0].queue );
   // We have to go around the additional data member inserted by the
   // compiler for the virtual table
   for ( i = 0; i < sizeof(vdsQueue)/sizeof(void*); ++i, apiObj++ ) {
      if ( **apiObj == VDSA_QUEUE ) {
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
   apiQueue = (vdsaQueue **) ( (unsigned char *) &q[1].queue + api_offset );
   pQueue = (vdseQueue *) (*apiQueue)->object.pMyVdsObject;
   pQueue->nodeObject.txCounter++;
   GET_PTR( txQueueStatus, pQueue->nodeObject.txStatusOffset, vdseTxStatus );
   txQueueStatus->usageCounter++;
   
   listErrCode = vdseLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
   i = 0;
   while ( listErrCode == LIST_OK ) {
      listErrCode =  vdseLinkedListPeakNext( &pQueue->listOfElements, 
                                             pNode, 
                                             &pNode );
      i++;
      if ( i >= 6 ) break;
   }
   if ( listErrCode != LIST_OK ) {
      cerr << "Iteration error in " << q[1].name << ", list err = " << listErrCode << endl;
      return -1;
   }
   pQueueItem = (vdseQueueItem*) 
      ((char*)pNode - offsetof( vdseQueueItem, node ));
   txItemStatus = &pQueueItem->txStatus;
   txItemStatus->usageCounter++;

   cout << "Defect for " << q[2].name << ": object added - not committed" << endl;
   apiQueue = (vdsaQueue **) ( (unsigned char *) &q[2].queue + api_offset );
   pQueue = (vdseQueue *) (*apiQueue)->object.pMyVdsObject;
   GET_PTR( txQueueStatus, pQueue->nodeObject.txStatusOffset, vdseTxStatus );
   txQueueStatus->txOffset = SET_OFFSET( pQueue ); 
   txQueueStatus->enumStatus = VDSE_TXS_ADDED;

   // Queue 2. Defects: 
   //  - Items added (not committed) and items removed (committed + non-comm)
   cout << "Defect for " << q[3].name << ": 5 items removed-committed," << endl;
   cout << "                                  4 items removed - not committed," << endl;
   cout << "                                  9 items added - not committed" << endl;
   apiQueue = (vdsaQueue **) ( (unsigned char *) &q[3].queue + api_offset );
   pQueue = (vdseQueue *) (*apiQueue)->object.pMyVdsObject;

   listErrCode = vdseLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
   i = 0;
   while ( listErrCode == LIST_OK ) {
      pQueueItem = (vdseQueueItem*) 
         ((char*)pNode - offsetof( vdseQueueItem, node ));
      txItemStatus = &pQueueItem->txStatus;

      if ( i < 5 ) { /* removed committed */
         txItemStatus->txOffset = SET_OFFSET( pQueue ); 
         txItemStatus->enumStatus = VDSE_TXS_DESTROYED_COMMITTED;
      }
      else if ( i < 9 ) { /* removed uncommitted */
         txItemStatus->txOffset = SET_OFFSET( pQueue ); 
         txItemStatus->enumStatus = VDSE_TXS_DESTROYED;
      }
      else if ( i >= 11 ) { /* Added */
         txItemStatus->txOffset = SET_OFFSET( pQueue ); 
         txItemStatus->enumStatus = VDSE_TXS_ADDED;
      }
      
      listErrCode =  vdseLinkedListPeakNext( &pQueue->listOfElements, 
                                             pNode, 
                                             &pNode );
      i++;
   }
   if ( listErrCode != LIST_END_OF_LIST ) {
      cerr << "Iteration error in " << q[3].name << ", list err = " << listErrCode << endl;
      return -1;
   }
   
   cout << "Defect for " << q[4].name << ": object locked" << endl;
   apiQueue = (vdsaQueue **) ( (unsigned char *) &q[4].queue + api_offset );
   pQueue = (vdseQueue *) (*apiQueue)->object.pMyVdsObject;
   if ( vdscTryAcquireProcessLock ( &pQueue->memObject.lock, getpid(), 0 ) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }

   cout << "Defect for " << q[5].name << ": broken forward link" << endl;
   apiQueue = (vdsaQueue **) ( (unsigned char *) &q[5].queue + api_offset );
   pQueue = (vdseQueue *) (*apiQueue)->object.pMyVdsObject;
   if ( vdscTryAcquireProcessLock ( &pQueue->memObject.lock, getpid(), 0 ) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }

   listErrCode = vdseLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
   i = 0;
   while ( listErrCode == LIST_OK ) {
      listErrCode =  vdseLinkedListPeakNext( &pQueue->listOfElements, 
                                             pNode, 
                                             &pNode );
      i++;
      if ( i == 9 ) {
         pNode->nextOffset = NULL_OFFSET;
         break;
      }
   }
   if ( listErrCode != LIST_OK ) {
      cerr << "Iteration error in " << q[5].name << ", list err = " << listErrCode << endl;
      return -1;
   }

   cout << "Defect for " << q[6].name << ": broken backward link" << endl;
   apiQueue = (vdsaQueue **) ( (unsigned char *) &q[6].queue + api_offset );
   pQueue = (vdseQueue *) (*apiQueue)->object.pMyVdsObject;
   if ( vdscTryAcquireProcessLock ( &pQueue->memObject.lock, getpid(), 0 ) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }

   listErrCode = vdseLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
   i = 0;
   while ( listErrCode == LIST_OK ) {
      listErrCode =  vdseLinkedListPeakNext( &pQueue->listOfElements, 
                                             pNode, 
                                             &pNode );
      i++;
      if ( i == 9 ) {
         pNode->previousOffset = NULL_OFFSET;
         break;
      }
   }
   if ( listErrCode != LIST_OK ) {
      cerr << "Iteration error in " << q[6].name << ", list err = " << listErrCode << endl;
      return -1;
   }
   
   cout << "Defect for " << q[7].name << ": 2 broken forward links" << endl;
   apiQueue = (vdsaQueue **) ( (unsigned char *) &q[7].queue + api_offset );
   pQueue = (vdseQueue *) (*apiQueue)->object.pMyVdsObject;
   if ( vdscTryAcquireProcessLock ( &pQueue->memObject.lock, getpid(), 0 ) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }

   listErrCode = vdseLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
   i = 0;
   while ( listErrCode == LIST_OK ) {
      listErrCode =  vdseLinkedListPeakNext( &pQueue->listOfElements, 
                                             pNode, 
                                             &pNode );
      i++;
      if ( i == 9 ) pSavedNode = pNode;
      if ( i == 13 ) {
         pSavedNode->nextOffset = NULL_OFFSET;
         pNode->nextOffset = NULL_OFFSET;
         break;
      }
   }
   if ( listErrCode != LIST_OK ) {
      cerr << "Iteration error in " << q[7].name << ", list err = " << listErrCode << endl;
      return -1;
   }

   cout << "Defect for " << q[8].name << ": 2 broken backward links" << endl;
   apiQueue = (vdsaQueue **) ( (unsigned char *) &q[8].queue + api_offset );
   pQueue = (vdseQueue *) (*apiQueue)->object.pMyVdsObject;
   if ( vdscTryAcquireProcessLock ( &pQueue->memObject.lock, getpid(), 0 ) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }

   listErrCode = vdseLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
   i = 0;
   while ( listErrCode == LIST_OK ) {
      listErrCode =  vdseLinkedListPeakNext( &pQueue->listOfElements, 
                                             pNode, 
                                             &pNode );
      i++;
      if ( i == 9 ) pSavedNode = pNode;
      if ( i == 13 ) {
         pSavedNode->previousOffset = NULL_OFFSET;
         pNode->previousOffset = NULL_OFFSET;
         break;
      }
   }
   if ( listErrCode != LIST_OK ) {
      cerr << "Iteration error in " << q[8].name << ", list err = " << listErrCode << endl;
      return -1;
   }

   cout << "Defect for " << q[9].name << ": broken bw+fw links (eq)" << endl;
   apiQueue = (vdsaQueue **) ( (unsigned char *) &q[9].queue + api_offset );
   pQueue = (vdseQueue *) (*apiQueue)->object.pMyVdsObject;
   if ( vdscTryAcquireProcessLock ( &pQueue->memObject.lock, getpid(), 0 ) ) {
      cerr << "Error - cannot lock the object" << endl;
      return -1;
   }

   listErrCode = vdseLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
   i = 0;
   while ( listErrCode == LIST_OK ) {
      listErrCode =  vdseLinkedListPeakNext( &pQueue->listOfElements, 
                                             pNode, 
                                             &pNode );
      i++;
      if ( i == 9 ) {
         pNode->previousOffset = NULL_OFFSET;
         pNode->nextOffset = NULL_OFFSET;
         break;
      }
   }
   if ( listErrCode != LIST_OK ) {
      cerr << "Iteration error in " << q[9].name << ", list err = " << listErrCode << endl;
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
      { "", VDS_VAR_STRING, 1, 200, 0, 0}, 
      { { "Field_1", VDS_VAR_STRING, 1, 200, 0, 0 } } 
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

void PopulateQueues( vdsSession & session, vector<myQueue> & q )
{
   int i, j;
   string data;
   char s[4];
   vdsObjectDefinition queueDef = { 
      VDS_QUEUE,
      1, 
      { "", VDS_INTEGER, 0, 0, 0, 0}, 
      { { "Field_1", VDS_VAR_STRING, 4, 10, 0, 0 } } 
   };
   
   for ( i = 0; i < NUM_QUEUES; ++i ) {
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
         cerr << "Is the watchdog running?" << endl;
         return 1;
      }
   }
   
   cout << " ------- VDSF defects injector ------- " << endl << endl;
   cout << " This program will inject pseudo-random defects in a VDS." << endl << endl;

   vector<myQueue> q( NUM_QUEUES, myQueue(session) );
   vector<myMap>   h( NUM_MAPS,   myMap(session) );
   for ( i = 0; i < NUM_QUEUES; ++i ) {
      q[i].name += ('0' + i/10 );
      q[i].name += ('0' + (i%10) );
   }
   for ( i = 0; i < NUM_MAPS; ++i ) {
      h[i].name += ('0' + i/10 );
      h[i].name += ('0' + (i%10) );
   }
   
   try {
      PopulateQueues( session, q );
      PopulateHashMaps( session, h );
   }
   catch( vdsException exc ) {
      cerr << "Creating and populating the objects failed, error = " << exc.Message() << endl;
      return 1;
   }
   cout << "Queues are created and populated." << endl << endl;

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
   
   return 0;
}

