/*
 * Copyright (C) 2007 Daniel Prevost <dprevost@users.sourceforge.net>
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


// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int AddDefectsQueues( vector<vdsQueue> & q )
{
   int offset = -1;
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
         offset = i * sizeof(void*);
         break;
      }
   }
   if ( offset == -1 )
   {
      cerr << "Can't calculate offset!" << endl;
      return -1;
   }
   else
      cout << "Offset = " << offset << endl;
   
   // Queue 0. Defects: 
   //  - ref counters 
   apiQueue = (vdsaQueue **) ( (unsigned char **) &q[0] + offset );
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

   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void PopulateQueues( vdsSession & session, vector<vdsQueue> & q )
{
   int i, j;
   string data;
   
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
         data = "Inserted data item = " + j;
         data += + " in queue = " + i;
         q[i].Push( data.c_str(), data.length() );
      }
   }
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
      cerr << "Init VDSF failed, error = " << rc << endl;
      cerr << "Is the watchdog running?" << endl;
      return 1;
   }
   
   cout << " ------- VDSF defects injector ------- " << endl << endl;
   cout << " This program will inject pseudo-random defects in a VDS." << endl << endl;

   vector<vdsQueue> q( 4, vdsQueue(session) );

   try {
      PopulateQueues( session, q );
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
   
   session.Commit();

   return 0;
}

