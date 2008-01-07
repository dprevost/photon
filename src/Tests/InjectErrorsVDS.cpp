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

//#include "Engine/ProcessManager.h"
//#include "Engine/SessionContext.h"
//#include "Engine/InitEngine.h"
//#include "Engine/Folder.h"
//#include "Engine/MemoryAllocator.h"
//#include "Engine/Queue.h"

using namespace std;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

string foldername("Test Folder");
string queueName0("Test Folder/Queue 0");
string queueName1("Test Folder/Queue 1");
string queueName2("Test Folder/Queue 2");
string queueName3("Test Folder/Queue 3");
string hashMapName0("Test Folder/HashMap 0");
string hashMapName1("Test Folder/HashMap 1");
string hashMapName2("Test Folder/HashMap 2");
string hashMapName3("Test Folder/HashMap 3");


// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int AddDefectsHashMaps( vector<vdsHashMap> & h )
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
   
   // We have to go around the additional data member inserted by the
   // compiler for the virtual table (true for g++)
   for ( i = 0, apiObj = (unsigned long **) &h[0]; 
      i < sizeof(vdsHashMap)/sizeof(void*); ++i, apiObj++ )
   {
      if ( **apiObj == VDSA_HASH_MAP )
      {
         api_offset = i * sizeof(void*);
         break;
      }
   }
   if ( api_offset == -1 )
   {
      cerr << "Can't calculate offset!" << endl;
      return -1;
   }
   
   // HashMap 0. Defects: 
   //  - ref counters 
   apiHashMap = (vdsaHashMap **) ( (unsigned char *) &h[0] + api_offset );
   pHashMap = (vdseHashMap *) (*apiHashMap)->object.pMyVdsObject;
   pHashMap->nodeObject.txCounter++;
   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, vdseTxStatus );
   txHashMapStatus->usageCounter++;

   listErrCode = vdseHashGetFirst( &pHashMap->hashObj,
                                   &bucket, 
                                   &offset );
   i = 0;
   while ( listErrCode == LIST_OK )
   {
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
   if ( listErrCode != LIST_OK )
   {
      cerr << "Iteration error in hashMap 0, list err = " << listErrCode << endl;
      return -1;
   }
   GET_PTR( pItem, offset, vdseHashItem );
   txItemStatus = &pItem->txStatus;
   txItemStatus->usageCounter++;

   // HashMap 1. Defects: 
   //  - added - not committed
   apiHashMap = (vdsaHashMap **) ( (unsigned char *) &h[1] + api_offset );
   pHashMap = (vdseHashMap *) (*apiHashMap)->object.pMyVdsObject;
   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, vdseTxStatus );
   txHashMapStatus->txOffset = SET_OFFSET( pHashMap ); 
   txHashMapStatus->enumStatus = VDSE_TXS_ADDED;

   // HashMap 2. Defects: 
   //  - Items added (not committed) and items removed (committed + non-comm)
   apiHashMap = (vdsaHashMap **) ( (unsigned char *) &h[2] + api_offset );
   pHashMap = (vdseHashMap *) (*apiHashMap)->object.pMyVdsObject;

   listErrCode = vdseHashGetFirst( &pHashMap->hashObj,
                                   &bucket, 
                                   &offset );
   i = 0;
   while ( listErrCode == LIST_OK )
   {      
      GET_PTR( pItem, offset, vdseHashItem );
      txItemStatus = &pItem->txStatus;

      if ( i < 5 ) /* removed committed */
      {
         txItemStatus->txOffset = SET_OFFSET( pHashMap ); 
         txItemStatus->enumStatus = VDSE_TXS_DESTROYED_COMMITTED;
      }
      else if ( i < 9 ) /* removed uncommitted */
      {
         txItemStatus->txOffset = SET_OFFSET( pHashMap ); 
         txItemStatus->enumStatus = VDSE_TXS_DESTROYED;
      }
      else if ( i >= 11 ) /* Added */
      {
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
   if ( listErrCode != LIST_END_OF_LIST )
   {
      cerr << "Iteration error in queue 2, list err = " << listErrCode << endl;
      return -1;
   }
   
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int AddDefectsQueues( vector<vdsQueue> & q )
{
   int api_offset = -1;
   struct vdsaQueue ** apiQueue;
   struct vdseQueue * pQueue;
   vdseTxStatus * txItemStatus, * txQueueStatus;
   unsigned long i, ** apiObj;
   enum ListErrors listErrCode;
   vdseLinkNode * pNode = NULL;
   vdseQueueItem* pQueueItem = NULL;
   
   // We have to go around the additional data member inserted by the
   // compiler for the virtual table
   for ( i = 0, apiObj = (unsigned long **) &q[0]; 
      i < sizeof(vdsQueue)/sizeof(void*); ++i, apiObj++ )
   {
      if ( **apiObj == VDSA_QUEUE )
      {
         api_offset = i * sizeof(void*);
         break;
      }
   }
   if ( api_offset == -1 )
   {
      cerr << "Can't calculate offset!" << endl;
      return -1;
   }
   else
      cout << "Offset = " << api_offset << endl;
   
   // Queue 0. Defects: 
   //  - ref counters 
   apiQueue = (vdsaQueue **) ( (unsigned char *) &q[0] + api_offset );
   cout << apiQueue << endl;
   pQueue = (vdseQueue *) (*apiQueue)->object.pMyVdsObject;
   cout << pQueue << endl;
   pQueue->nodeObject.txCounter++;
   GET_PTR( txQueueStatus, pQueue->nodeObject.txStatusOffset, vdseTxStatus );
   txQueueStatus->usageCounter++;
   
   listErrCode = vdseLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
   i = 0;
   while ( listErrCode == LIST_OK )
   {
      listErrCode =  vdseLinkedListPeakNext( &pQueue->listOfElements, 
                                             pNode, 
                                             &pNode );
      i++;
      if ( i >= 6 ) break;
   }
   if ( listErrCode != LIST_OK )
   {
      cerr << "Iteration error in queue 0, list err = " << listErrCode << endl;
      return -1;
   }
   pQueueItem = (vdseQueueItem*) 
      ((char*)pNode - offsetof( vdseQueueItem, node ));
   txItemStatus = &pQueueItem->txStatus;
   txItemStatus->usageCounter++;

   // Queue 1. Defects: 
   //  - added - not committed
   apiQueue = (vdsaQueue **) ( (unsigned char *) &q[1] + api_offset );
   pQueue = (vdseQueue *) (*apiQueue)->object.pMyVdsObject;
   GET_PTR( txQueueStatus, pQueue->nodeObject.txStatusOffset, vdseTxStatus );
   txQueueStatus->txOffset = SET_OFFSET( pQueue ); 
   txQueueStatus->enumStatus = VDSE_TXS_ADDED;

   // Queue 2. Defects: 
   //  - Items added (not committed) and items removed (committed + non-comm)
   apiQueue = (vdsaQueue **) ( (unsigned char *) &q[2] + api_offset );
   pQueue = (vdseQueue *) (*apiQueue)->object.pMyVdsObject;

   listErrCode = vdseLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
   i = 0;
   while ( listErrCode == LIST_OK )
   {      
      pQueueItem = (vdseQueueItem*) 
         ((char*)pNode - offsetof( vdseQueueItem, node ));
      txItemStatus = &pQueueItem->txStatus;

      if ( i < 5 ) /* removed committed */
      {
         txItemStatus->txOffset = SET_OFFSET( pQueue ); 
         txItemStatus->enumStatus = VDSE_TXS_DESTROYED_COMMITTED;
      }
      else if ( i < 9 ) /* removed uncommitted */
      {
         txItemStatus->txOffset = SET_OFFSET( pQueue ); 
         txItemStatus->enumStatus = VDSE_TXS_DESTROYED;
      }
      else if ( i >= 11 ) /* Added */
      {
         txItemStatus->txOffset = SET_OFFSET( pQueue ); 
         txItemStatus->enumStatus = VDSE_TXS_ADDED;
      }
      
      listErrCode =  vdseLinkedListPeakNext( &pQueue->listOfElements, 
                                             pNode, 
                                             &pNode );
      i++;
   }
   if ( listErrCode != LIST_END_OF_LIST )
   {
      cerr << "Iteration error in queue 2, list err = " << listErrCode << endl;
      return -1;
   }
   
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void CleanupPreviousRun( vdsSession & session )
{
   try {
      session.DestroyObject( queueName0 );
      session.DestroyObject( queueName2 );
      session.DestroyObject( queueName3 );
      session.DestroyObject( hashMapName0 );
      session.DestroyObject( hashMapName2 );
      session.DestroyObject( hashMapName3 );
      session.Commit();
   }
   catch ( int rc )
   {
      cerr << "Cleanup of previous session failed, error = " << rc << endl;
      cerr << "Is the watchdog running?" << endl;
      exit(1);
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void PopulateHashMaps( vdsSession & session, vector<vdsHashMap> & h )
{
   int i, j;
   string data, key;
   char s[4];
   
   session.CreateObject( hashMapName0, VDS_HASH_MAP );
   session.CreateObject( hashMapName1, VDS_HASH_MAP );
   session.CreateObject( hashMapName2, VDS_HASH_MAP );
   session.CreateObject( hashMapName3, VDS_HASH_MAP );
   h[0].Open( hashMapName0 );
   h[1].Open( hashMapName1 );
   h[2].Open( hashMapName2 );
   h[3].Open( hashMapName3 );

   for ( i = 0; i < 4; ++i )
   {
      for ( j = 0; j < 20; ++j )
      {
         sprintf(s, "%d", j);
         key = string("Key ") + s;
         data = string("Inserted data item = ") + s;
         sprintf(s, "%d", i);
         data += string(" in hashMap = ") + s;
         h[i].Insert( key.c_str(), key.length(), data.c_str(), data.length() );
      }
   }

   session.Commit();
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void PopulateQueues( vdsSession & session, vector<vdsQueue> & q )
{
   int i, j;
   string data;
   char s[4];
   
   session.CreateObject( queueName0, VDS_QUEUE );
   session.CreateObject( queueName1, VDS_QUEUE );
   session.CreateObject( queueName2, VDS_QUEUE );
   session.CreateObject( queueName3, VDS_QUEUE );
   q[0].Open( queueName0 );
   q[1].Open( queueName1 );
   q[2].Open( queueName2 );
   q[3].Open( queueName3 );

   for ( i = 0; i < 4; ++i )
   {
      for ( j = 0; j < 20; ++j )
      {
         sprintf(s, "%d", j);
         data = string("Inserted data item = ") + s;
         sprintf(s, "%d", i);
         data += string(" in queue = ") + s;
         q[i].Push( data.c_str(), data.length() );
      }
   }

   session.Commit();
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int main()
{
   vdsProcess process;
   vdsSession session;
   
   try {
      process.Init( "10701" );
      session.Init();
      session.CreateObject( foldername, VDS_FOLDER );
   }
   catch( int rc ) {
      if ( rc == VDS_OBJECT_ALREADY_PRESENT )
      {
         CleanupPreviousRun( session );
      }
      else
      {
         cerr << "Init VDSF failed, error = " << rc << endl;
         cerr << "Is the watchdog running?" << endl;
         return 1;
      }
   }
   
   cout << " ------- VDSF defects injector ------- " << endl << endl;
   cout << " This program will inject pseudo-random defects in a VDS." << endl << endl;

   vector<vdsQueue> q( 4, vdsQueue(session) );
   vector<vdsHashMap> h( 4, vdsHashMap(session) );

   try {
      PopulateQueues( session, q );
      PopulateHashMaps( session, h );
   }
   catch( int rc ) {
      cerr << "Creating and populating the queues failed, error = " << rc << endl;
      return 1;
   }
   cout << "Queues are created and populated." << endl;

   int rc = AddDefectsQueues( q );
   if ( rc != 0 )
   {
      cerr << "Adding defect to queues failed!" << endl;
      return 1;
   }
   cout << "Defects were added to queues." << endl;

   rc = AddDefectsHashMaps( h );
   if ( rc != 0 )
   {
      cerr << "Adding defect to hash maps failed!" << endl;
      return 1;
   }
   cout << "Defects were added to hash maps." << endl;
   
   return 0;
}

