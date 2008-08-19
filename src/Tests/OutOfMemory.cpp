/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of photon (photonsoftware.org).
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

using namespace std;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

string folderName("Out_Folder");
string queueName1("Out_Folder/Queue_1");
string queueName2("Out_Folder/Queue_2");

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Cleanup( psoSession & session )
{
   try {
      session.DestroyObject( queueName1 );
      session.DestroyObject( queueName2 );
      session.DestroyObject( folderName );
      session.Commit();
   }
   catch ( psoException exc ) {
      cerr << "Cleanup failed, error = " << exc.Message() << endl;
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int main()
{
   psoProcess process;
   psoSession session;
   psoInfo info1; 
   const char * dataIn = "1234567890123456789012345";
   char dataOut[50];
   size_t length;
   int countIn = 0, countOut = 0, errcode;
   psoQueue q1(session), q2(session);

   psoObjectDefinition queueDef = { 
      PSO_QUEUE,
      1, 
      { PSO_KEY_INTEGER, 0, 0, 0 }, 
      { { "Field_1", PSO_VAR_STRING, 0, 1, 100, 0, 0 } } 
   };
   psoObjectDefinition folderDef;

   memset( &folderDef, 0, sizeof folderDef );
   folderDef.type = PSO_FOLDER;
   
   try {
      process.Init( "10701" );
      session.Init();

      try {
         session.DestroyObject( queueName1 );
         session.DestroyObject( queueName2 );
         session.DestroyObject( folderName );
         session.Commit();
      }
      catch ( psoException exc ) {}

      session.CreateObject( folderName, &folderDef );
      session.CreateObject( queueName1, &queueDef );
      session.CreateObject( queueName2, &queueDef );
      session.GetInfo( &info1 );
   }
   catch( psoException exc ) {
      cerr << "Init VDSF failed, error = " << exc.Message() << endl;
      cerr << "Is the watchdog running?" << endl;
      return 1;
   }
   cerr << "Total allocated in vds: " << info1.allocatedSizeInBytes << endl;
   cerr << "                        " << info1.totalSizeInBytes << endl;

   try {
      q1.Open( queueName1 );
      q2.Open( queueName2 );
   }
   catch( psoException exc ) {
      cerr << "Error opening queues, error = " << exc.Message() << endl;
      return 1;
   }
   
   try {
      while ( true ) {
         q1.Push( dataIn, strlen(dataIn) );
         countIn++;
         q2.Push( dataIn, strlen(dataIn) );
         countIn++;
      }
   }
   catch( psoException exc ) {
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
   catch ( psoException exc ) {
      cerr << "Commit error = " << exc.Message() << endl;
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
   catch( psoException exc ) {
      cerr << "Error retrieving data, error = " << exc.Message() << endl;
      return 1;
   }
   if ( countIn != countOut ) {
      cerr << "Number of retrieved records: " << countOut << endl;
   }

   try {
      session.Commit();
   }
   catch ( psoException exc ) {
      cerr << "Commit error = " << exc.Message() << endl;
   }
   
  
   session.GetInfo( &info1 );
   cerr << "Total allocated in vds: " << info1.allocatedSizeInBytes << endl;
   cerr << info1.totalSizeInBytes << endl;
 
   psoObjStatus status;

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

