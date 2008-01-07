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

using namespace std;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

string folderName("Out Folder");
string queueName1("Out Folder/Queue 1");
string queueName2("Out Folder/Queue 2");

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Cleanup( vdsSession & session )
{
   try {
      session.DestroyObject( queueName1 );
      session.DestroyObject( queueName2 );
      session.DestroyObject( folderName );
      session.Commit();
   }
   catch ( int rc )
   {
      cerr << "Cleanup failed, error = " << rc << endl;
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int main()
{
   vdsProcess process;
   vdsSession session;
   vdsInfo info1, info2;
   const char * dataIn = "1234567890123456789012345";
   char dataOut[50];
   size_t length;
   int countIn = 0, countOut = 0, errcode;
   
   try {
      process.Init( "10701" );
      session.Init();
      Cleanup( session );

      session.CreateObject( folderName, VDS_FOLDER );
      session.CreateObject( queueName1, VDS_QUEUE );
      session.CreateObject( queueName2, VDS_QUEUE );
      session.GetInfo( &info1 );
   }
   catch( int rc ) {
      cerr << "Init VDSF failed, error = " << rc << endl;
      cerr << "Is the watchdog running?" << endl;
      return 1;
   }
   cerr << "Total allocated in vds: " << info1.allocatedSizeInBytes << endl;
   cerr << "                        " << info1.totalSizeInBytes << endl;

   vdsQueue q1(session), q2(session);
   try {
      q1.Open( queueName1 );
      q2.Open( queueName2 );
   }
   catch( int rc ) {
      cerr << "Error opening queues, error = " << rc << endl;
      return 1;
   }
   
   try {
      while ( true )
      {
         q1.Push( dataIn, strlen(dataIn) );
         countIn++;
         q2.Push( dataIn, strlen(dataIn) );
         countIn++;
      }
   }
   catch( int rc ) {
      if ( rc == VDS_NOT_ENOUGH_VDS_MEMORY )
         cout << "Number of inserted records: " << countIn << endl;
      else
      {
         cerr << "Error inserting data, error = " << rc << endl;
         return 1;
      }
   }
   
   try {
      session.Commit();
   }
   catch ( int rc ) {
      cerr << "Commit error = " << rc << endl;
   }
   
   try {
      do {
         errcode = q1.Pop( dataOut, 50, &length );
         if ( errcode == 0 ) countOut++;
      } while ( errcode == 0 );
      do {
         errcode = q2.Pop( dataOut, 50, &length );
         if ( errcode == 0 ) countOut++;
      } while ( errcode == 0 );
   }
   catch( int rc ) {
      cerr << "Error retrieving data, error = " << rc << endl;
      return 1;
   }
   if ( countIn != countOut )
   {
      cerr << "Number of retrieved records: " << countOut << endl;
   }

   try {
      session.Commit();
   }
   catch ( int rc ) {
      cerr << "Commit error = " << rc << endl;
   }
   
  
   session.GetInfo( &info1 );
   cerr << "Total allocated in vds: " << info1.allocatedSizeInBytes << endl;
   cerr << info1.totalSizeInBytes << endl;
 
   vdsObjStatus status;

   q1.Status( &status );
   cout << status.numBlocks << endl;
   cout << status.numBlockGroup << endl;
   cout << status.numDataItem << endl;
   cout << status.freeBytes << endl;
   q2.Status( &status );
   cout << status.numBlocks << endl;
   cout << status.numBlockGroup << endl;
   cout << status.numDataItem << endl;
   cout << status.freeBytes << endl;
 
   q1.Close();
   q2.Close();

   Cleanup( session );

   return 0;
}
