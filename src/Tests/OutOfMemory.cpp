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

using namespace std;
using namespace pso;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

string folderName("Out_Folder");
string queueName1("Out_Folder/Queue_1");
string queueName2("Out_Folder/Queue_2");

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Cleanup( Session & session )
{
   try {
      session.DestroyObject( queueName1 );
      session.DestroyObject( queueName2 );
      session.DestroyObject( folderName );
      session.Commit();
   }
   catch ( pso::Exception exc ) {
      cerr << "Cleanup failed, error = " << exc.Message() << endl;
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int main()
{
   Process process;
   Session session;
   psoInfo info1; 
   const char * dataIn = "1234567890123456789012345";
   char dataOut[50];
   uint32_t length;
   int countIn = 0, countOut = 0, errcode;
   Queue q1, q2;

   psoObjectDefinition queueDef = { PSO_QUEUE, 0, 0, 0 };

   try {
      process.Init( "10701", "OutOfMemory" );
      session.Init();

      try {
         session.DestroyObject( queueName1 );
         session.DestroyObject( queueName2 );
         session.DestroyObject( folderName );
         session.Commit();
      }
      catch ( Exception exc ) {}

      session.CreateFolder( folderName );
      session.CreateQueue( queueName1, queueDef, "Default" );
      session.CreateQueue( queueName2, queueDef, "Default" );
      session.GetInfo( info1 );
   }
   catch( Exception exc ) {
      cerr << "Init Photon failed, error = " << exc.Message() << endl;
      cerr << "Is the server running?" << endl;
      return 1;
   }
   cerr << "Total allocated in memory: " << info1.allocatedSizeInBytes << endl;
   cerr << "Total memory size:         " << info1.totalSizeInBytes << endl;

   try {
      q1.Open( session, queueName1 );
      q2.Open( session, queueName2 );
   }
   catch( Exception exc ) {
      cerr << "Error opening queues, error = " << exc.Message() << endl;
      return 1;
   }
   
   try {
      while ( countIn < 100000 /* true */) {
         q1.Push( dataIn, strlen(dataIn) );
         countIn++;
         q2.Push( dataIn, strlen(dataIn) );
         countIn++;
      }
   }
   catch( Exception exc ) {
      if ( exc.ErrorCode() == PSO_NOT_ENOUGH_PSO_MEMORY ) {
         cout << "Number of inserted records: " << countIn << endl;
      }
      else {
         cerr << "Error inserting data, error = " << exc.Message() << endl;
         return 1;
      }
   }
   
   try {
      session.Commit();
   }
   catch ( Exception exc ) {
      cerr << "Commit error = " << exc.Message() << endl;
   }
   
   try {
      do {
         errcode = q1.Pop( dataOut, 50, length );
         if ( errcode == 0 ) countOut++;
	 if ( (countOut % 500) == 0 ) session.Commit();
      } while ( errcode == 0 );
      do {
         errcode = q2.Pop( dataOut, 50, length );
         if ( errcode == 0 ) countOut++;
	 if ( (countOut % 500) == 0 ) session.Commit();
      } while ( errcode == 0 );
   }
   catch( Exception exc ) {
      cerr << "Error retrieving data, error = " << exc.Message() << endl;
      return 1;
   }
   if ( countIn != countOut ) {
      cerr << "Number of retrieved records: " << countOut << endl;
   }

   try {
      session.Commit();
   }
   catch ( Exception exc ) {
      cerr << "Commit error = " << exc.Message() << endl;
   }
   
  
   session.GetInfo( info1 );
   cerr << "Total allocated in memory: " << info1.allocatedSizeInBytes << endl;
   cerr << "Total memory size:         " << info1.totalSizeInBytes << endl;
 
   psoObjStatus status;

   q1.Status( status );
   cout << status.numBlocks << endl;
   cout << status.numBlockGroup << endl;
   cout << status.numDataItem << endl;
   cout << status.freeBytes << endl;
   q2.Status( status );
   cout << status.numBlocks << endl;
   cout << status.numBlockGroup << endl;
   cout << status.numDataItem << endl;
   cout << status.freeBytes << endl;
 
   q1.Close();
   q2.Close();

   Cleanup( session );

   return 0;
}

