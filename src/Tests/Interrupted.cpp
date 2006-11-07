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

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#include <stdlib.h> 
#include <stdio.h>
#include <signal.h>

#include <vdsf/vdsProcess.h>
#include <vdsf/vdsSession.h>
#include <vdsf/vdsFolder.h>
#include <vdsf/vdsQueue.h>
#include <vdsf/vdsErrors.h>

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

vdsSession* g_pSession1, *g_pSession2;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

/* 
 * This program tests the interruptability capabilities of VDS. In other 
 * words, it test the ability of the framework to handle crashed clients.
 * 
 * How it works: 
 * 
 * - there will be a series of tests with crashes number 1, 2, ...
 * - for each test there will be a test of the test, usually run after, to 
 *   make sure that the vds was properly cleaned.
 *
 * test cases:
 * 
 *  1) connect and crash
 *  2) create object and crash
 */

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test1()
{
   int errCode = 0;
   vdsQueue q1(*g_pSession1);
   
   errCode = g_pSession1->CreateObject( "A Queue", VDS_QUEUE );
   if ( errCode != VDS_OK )
      goto end;

   errCode = q1.Open( "A Queue" );
   if ( errCode != VDS_OK )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_OK,
              errCode );
      goto end;
   }

 end:

   q1.Close();
   g_pSession1->DestroyObject( "A Queue" ); 
   g_pSession1->Commit();
   
   if ( errCode != 0 )
      return -1;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test2()
{
   int errCode = 0;
   vdsQueue q1(*g_pSession2);
   
   errCode = g_pSession1->CreateObject( "A Queue", VDS_QUEUE );
   if ( errCode != VDS_OK )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_OK,
              errCode );
      goto end;
   }
   if ( errCode != VDS_OK )
      goto end;

   errCode = q1.Open( "A Queue" );
   if ( errCode != VDS_NO_SUCH_OBJECT )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_NO_SUCH_OBJECT, 
              errCode );
      errCode = -1;
      goto end;
   }
   errCode = VDS_OK;

 end:

   q1.Close();
   g_pSession1->DestroyObject( "A Queue" ); 
   g_pSession1->Commit();

   if ( errCode != 0 )
      return -2;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test3()
{
   int errCode = 0;
   vdsQueue q1(*g_pSession2);
   
   errCode = g_pSession1->CreateObject( "A Queue", VDS_QUEUE );
   if ( errCode != VDS_OK )
      goto end;
   errCode = g_pSession1->Commit();
   if ( errCode != VDS_OK )
      goto end;

   errCode = q1.Open( "A Queue" );
   if ( errCode != VDS_OK )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_OK, 
              errCode );
      goto end;
   }

 end:

   q1.Close();
   g_pSession1->DestroyObject( "A Queue" ); 
   g_pSession1->Commit();

   if ( errCode != 0 )
      return -3;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test4( bool bSession1 )
{
   int errCode = 0;
   vdsQueue q1(*g_pSession1);
   
   errCode = g_pSession1->CreateObject( "A Queue", VDS_QUEUE );
   if ( errCode != VDS_OK )
      goto end;
   errCode = g_pSession1->Rollback();
   if ( bSession1 == false ) // implicit
   {
      if ( errCode != VDS_NO_ROLLBACK )
      {
         printf( " Expected error = %d, returned error = %d\n", 
                 VDS_NO_ROLLBACK, 
                 errCode );
         errCode = -1;        
      }
      else
         errCode = 0;
      goto end;
   }
   else
      if ( errCode != VDS_OK )
         goto end;

   errCode = q1.Open( "A Queue" );
   if ( errCode != VDS_NO_SUCH_OBJECT )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_NO_SUCH_OBJECT, 
              errCode );
       errCode = -1;
      goto end;
   }
   errCode = 0;
   
 end:

   q1.Close();
   g_pSession1->DestroyObject( "A Queue" ); 
   g_pSession1->Commit();

   if ( errCode != 0 )
      return -4;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test5()
{
   int errCode = 0;
   vdsQueue q1(*g_pSession2);
   
   errCode = g_pSession1->CreateObject( "A Queue", VDS_QUEUE );
   if ( errCode != VDS_OK )
      goto end;
   errCode = g_pSession1->Commit();
   if ( errCode != VDS_OK )
      goto end;
   errCode = q1.Open( "A Queue" );
   if ( errCode != VDS_OK )
      goto end;

   errCode = g_pSession1->DestroyObject( "A Queue" ); 
   if ( errCode != VDS_OK )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_OK,
              errCode );
      goto end;
   }
   
 end:

   q1.Close();
   g_pSession1->DestroyObject( "A Queue" ); 
   g_pSession1->Commit();

   if ( errCode != 0 )
      return -5;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test6()
{
   int errCode = 0;
   vdsQueue q1(*g_pSession1);
   
   errCode = g_pSession1->CreateObject( "A Queue", VDS_QUEUE );
   if ( errCode != VDS_OK )
      goto end;
   errCode = g_pSession1->Commit();
   if ( errCode != VDS_OK )
      goto end;

   errCode = g_pSession1->DestroyObject( "A Queue" );
   if ( errCode != VDS_OK )
      goto end;

   errCode = q1.Open( "A Queue" );
   if ( errCode != VDS_NO_SUCH_OBJECT )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_NO_SUCH_OBJECT,
              errCode );
      errCode = -1;
      goto end;
   }
   errCode = 0;
   
 end:

   q1.Close();
   g_pSession1->DestroyObject( "A Queue" ); 
   g_pSession1->Commit();

   if ( errCode != 0 )
      return -6;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test7()
{
   int errCode = 0;
   vdsQueue q1(*g_pSession2);
   
   errCode = g_pSession1->CreateObject( "A Queue", VDS_QUEUE );
   if ( errCode != VDS_OK )
      goto end;
   errCode = g_pSession1->Commit();
   if ( errCode != VDS_OK )
      goto end;

   errCode = g_pSession1->DestroyObject( "A Queue" );
   if ( errCode != VDS_OK )
      goto end;

   errCode = q1.Open( "A Queue" );
   if ( errCode != VDS_NO_SUCH_OBJECT )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_NO_SUCH_OBJECT,
              errCode );
      errCode = -1;
      goto end;
   }
   errCode = 0;
   
 end:

   q1.Close();
   g_pSession1->DestroyObject( "A Queue" ); 
   g_pSession1->Commit();

   if ( errCode != 0 )
      return -7;
   return 0;
}
// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test8()
{
   int errCode = 0;
   vdsQueue q1(*g_pSession2);
   
   errCode = g_pSession1->CreateObject( "A Queue", VDS_QUEUE );
   if ( errCode != VDS_OK )
      goto end;
   errCode = g_pSession1->Commit();
   if ( errCode != VDS_OK )
      goto end;

   errCode = g_pSession1->DestroyObject( "A Queue" );
   if ( errCode != VDS_OK )
      goto end;
   errCode = g_pSession1->Commit();
   if ( errCode != VDS_OK )
      goto end;

   errCode = q1.Open( "A Queue" );
   if ( errCode != VDS_NO_SUCH_OBJECT )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_NO_SUCH_OBJECT,
              errCode );
      errCode = -1;
      goto end;
   }
   errCode = 0;
   
 end:

   q1.Close();
   g_pSession1->DestroyObject( "A Queue" ); 
   g_pSession1->Commit();

   if ( errCode != 0 )
      return -8;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test9( bool bSession1 )
{
   int errCode = 0;
   vdsQueue q1(*g_pSession1);
   
   errCode = g_pSession1->CreateObject( "A Queue", VDS_QUEUE );
   if ( errCode != VDS_OK )
      goto end;
   errCode = g_pSession1->Commit();
   if ( errCode != VDS_OK )
      goto end;

   errCode = g_pSession1->DestroyObject( "A Queue" );
   if ( errCode != VDS_OK )
      goto end;
   errCode = g_pSession1->Rollback();
   if ( bSession1 == false ) // implicit
   {
      if ( errCode != VDS_NO_ROLLBACK )
      {
         printf( " Expected error = %d, returned error = %d\n", 
                 VDS_NO_ROLLBACK, 
                 errCode );
         errCode = -1;        
      }
      else
         errCode = 0;
      goto end;
   }
   else
      if ( errCode != VDS_OK )
         goto end;

   errCode = q1.Open( "A Queue" );
   if ( errCode != VDS_OK )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_OK,
              errCode );
      goto end;
   }
   
 end:

   q1.Close();
   g_pSession1->DestroyObject( "A Queue" ); 
   g_pSession1->Commit();

   if ( errCode != 0 )
      return -9;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test10( bool bSession1 )
{
   int errCode = 0;
   vdsQueue q1(*g_pSession2);
   
   errCode = g_pSession1->CreateObject( "A Queue", VDS_QUEUE );
   if ( errCode != VDS_OK )
      goto end;
   errCode = g_pSession1->Commit();
   if ( errCode != VDS_OK )
      goto end;

   errCode = g_pSession1->DestroyObject( "A Queue" );
   if ( errCode != VDS_OK )
      goto end;
   errCode = g_pSession1->Rollback();
   if ( bSession1 == false ) // implicit
   {
      if ( errCode != VDS_NO_ROLLBACK )
      {
         printf( " Expected error = %d, returned error = %d\n", 
                 VDS_NO_ROLLBACK, 
                 errCode );
         errCode = -1;        
      }
      else
         errCode = 0;
      goto end;
   }
   else
      if ( errCode != VDS_OK )
         goto end;

   errCode = q1.Open( "A Queue" );
   if ( errCode != VDS_OK )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_OK,
              errCode );
      goto end;
   }
   
 end:

   q1.Close();
   g_pSession1->DestroyObject( "A Queue" ); 
   g_pSession1->Commit();

   if ( errCode != 0 )
      return -10;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test11()
{
   int errCode = 0;

   errCode = g_pSession1->CreateObject( "A Folder", VDS_FOLDER );
   if ( errCode != VDS_OK )
      goto end;

   errCode = g_pSession2->CreateObject( "A Folder/A Queue", VDS_QUEUE );
   if ( errCode != VDS_NO_SUCH_FOLDER )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_NO_SUCH_FOLDER,
              errCode );
      errCode = -1;
      goto end;
   }
   errCode = 0;

 end:

   g_pSession2->DestroyObject( "A Folder/A Queue" ); 
   g_pSession2->Commit();
   g_pSession1->DestroyObject( "A Folder" ); 
   g_pSession1->Commit();

   if ( errCode != 0 )
      return -11;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test12()
{
   int errCode = 0;

   errCode = g_pSession1->CreateObject( "A Folder", VDS_FOLDER );
   if ( errCode != VDS_OK )
      goto end;
   errCode = g_pSession1->Commit();
   if ( errCode != VDS_OK )
      goto end;

   errCode = g_pSession2->CreateObject( "A Folder/A Queue", VDS_QUEUE );
   if ( errCode != VDS_OK )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_OK,
              errCode );
      goto end;
   }

 end:

   g_pSession2->DestroyObject( "A Folder/A Queue" ); 
   g_pSession2->Commit();
   g_pSession1->DestroyObject( "A Folder" ); 
   g_pSession1->Commit();

   if ( errCode != 0 )
      return -12;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test13( bool bSession1 )
{
   int errCode = 0;

   errCode = g_pSession1->CreateObject( "A Folder", VDS_FOLDER );
   if ( errCode != VDS_OK )
      goto end;
   errCode = g_pSession1->Rollback();
   if ( bSession1 == false ) // implicit
   {
      if ( errCode != VDS_NO_ROLLBACK )
      {
         printf( " Expected error = %d, returned error = %d\n", 
                 VDS_NO_ROLLBACK, 
                 errCode );
         errCode = -1;        
      }
      else
         errCode = 0;
      goto end;
   }
   else
      if ( errCode != VDS_OK )
         goto end;

   errCode = g_pSession2->CreateObject( "A Folder/A Queue", VDS_QUEUE );
   if ( errCode != VDS_NO_SUCH_FOLDER )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_NO_SUCH_FOLDER,
              errCode );
      errCode = -1;
      goto end;
   }
   errCode = -0;

 end:

   g_pSession2->DestroyObject( "A Folder/A Queue" ); 
   g_pSession2->Commit();
   g_pSession1->DestroyObject( "A Folder" ); 
   g_pSession1->Commit();

   if ( errCode != 0 )
      return -13;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test21()
{
   int errCode = 0;
   vdsQueue q1(*g_pSession1);
   char str[25];
   
   errCode = g_pSession1->CreateObject( "A Queue", VDS_QUEUE );
   if ( errCode != VDS_OK )
      goto end;
   
   errCode = q1.Open( "A Queue" );
   if ( errCode != VDS_OK )
      goto end;
   errCode = q1.InsertItem( "1234567890123", 14 );
   if ( errCode != VDS_OK )
      goto end;
   
   errCode = q1.RemoveItem( str, 25 );
   if ( errCode != VDS_OK )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_OK,
              errCode );
      goto end;
   }

 end:

   q1.Close();
   g_pSession1->DestroyObject( "A Queue" ); 
   g_pSession1->Commit();

   if ( errCode != 0 )
      return -21;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test22()
{
   int errCode = 0;
   vdsQueue q1(*g_pSession1), q2(*g_pSession1);
   char str[25];
   
   errCode = g_pSession1->CreateObject( "A Queue", VDS_QUEUE );
   if ( errCode != VDS_OK )
      goto end;
   
   errCode = q1.Open( "A Queue" );
   if ( errCode != VDS_OK )
      goto end;
   errCode = q2.Open( "A Queue" );
   if ( errCode != VDS_OK )
      goto end;
   errCode = q1.InsertItem( "1234567890123", 14 );
   if ( errCode != VDS_OK )
      goto end;
   
   errCode = q2.RemoveItem( str, 25 );
   if ( errCode != VDS_OK )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_OK, 
              errCode );
      goto end;
   }

 end:

   q1.Close();
   q2.Close();
   g_pSession1->DestroyObject( "A Queue" ); 
   g_pSession1->Commit();

   if ( errCode != 0 )
      return -22;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test23()
{
   int errCode = 0;
   vdsQueue q1(*g_pSession1), q2(*g_pSession2);
   char str[25];
   
   errCode = g_pSession1->CreateObject( "A Queue", VDS_QUEUE );
   if ( errCode != VDS_OK )
      goto end;
   // So that the object can be open by session 2
   errCode = g_pSession1->Commit(); 
   if ( errCode != VDS_OK )
      goto end;

   errCode = q1.Open( "A Queue" );
   if ( errCode != VDS_OK )
      goto end;
   errCode = q2.Open( "A Queue" );
   if ( errCode != VDS_OK )
      goto end;
   errCode = q1.InsertItem( "1234567890123", 14 );
   if ( errCode != VDS_OK )
      goto end;
   
   errCode = q2.RemoveItem( str, 25 );
   if ( errCode != VDS_IS_EMPTY )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_IS_EMPTY, 
              errCode );
      errCode = -1;
      goto end;
   }
   errCode = 0;
   
 end:

   q1.Close();
   q2.Close();
   g_pSession1->DestroyObject( "A Queue" ); 
   g_pSession1->Commit();

   if ( errCode != 0 )
      return -23;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test24()
{
   int errCode = 0;
   vdsQueue q1(*g_pSession1), q2(*g_pSession2);
   char str[25];
   
   errCode = g_pSession1->CreateObject( "A Queue", VDS_QUEUE );
   if ( errCode != VDS_OK )
      goto end;
   // So that the object can be open by session 2
   errCode = g_pSession1->Commit(); 
   if ( errCode != VDS_OK )
      goto end;

   errCode = q1.Open( "A Queue" );
   if ( errCode != VDS_OK )
      goto end;
   errCode = q2.Open( "A Queue" );
   if ( errCode != VDS_OK )
      goto end;
   errCode = q1.InsertItem( "1234567890123", 14 );
   if ( errCode != VDS_OK )
      goto end;
   errCode = g_pSession1->Commit(); 
   if ( errCode != VDS_OK )
      goto end;
   
   errCode = q2.RemoveItem( str, 25 );
   if ( errCode != VDS_OK )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_OK, 
              errCode );
      goto end;
   }

 end:

   q1.Close();
   q2.Close();
   g_pSession1->DestroyObject( "A Queue" ); 
   g_pSession2->Commit();
   g_pSession1->Commit();

   if ( errCode != 0 )
      return -24;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test25( bool bSession1 )
{
   int errCode = 0;
   vdsQueue q1(*g_pSession1);
   char str[25];
   
   errCode = g_pSession1->CreateObject( "A Queue", VDS_QUEUE );   
   if ( errCode != VDS_OK )
      goto end;
   // So that the object can be open by session 2
   errCode = g_pSession1->Commit(); 
   if ( errCode != VDS_OK )
      goto end;

   errCode = q1.Open( "A Queue" );
   if ( errCode != VDS_OK )
      goto end;
   errCode = q1.InsertItem( "1234567890123", 14 );
   if ( errCode != VDS_OK )
      goto end;
   errCode = g_pSession1->Rollback(); 
   if ( bSession1 == false ) // implicit
   {
      if ( errCode != VDS_NO_ROLLBACK )
      {
         printf( " Expected error = %d, returned error = %d\n", 
                 VDS_NO_ROLLBACK, 
                 errCode );
         errCode = -1;        
      }
      else
         errCode = 0;
      
      goto end;
   }
   else
      if ( errCode != VDS_OK )
         goto end;
   
   errCode = q1.RemoveItem( str, 25 );
   if ( errCode != VDS_IS_EMPTY )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_IS_EMPTY, 
              errCode );
      errCode = -1;
      goto end;
   }
   errCode = 0;
   
 end:

   q1.Close();
   g_pSession1->DestroyObject( "A Queue" ); 
   g_pSession1->Commit();

   if ( errCode != 0 )
      return -25;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test26()
{
   int errCode = 0;
   vdsQueue q1(*g_pSession1);
   char str[25];
   
   errCode = g_pSession1->CreateObject( "A Queue", VDS_QUEUE );
   if ( errCode != VDS_OK )
      goto end;
   
   errCode = q1.Open( "A Queue" );
   if ( errCode != VDS_OK )
      goto end;
   errCode = q1.InsertItem( "1234567890123", 14 );
   if ( errCode != VDS_OK )
      goto end;
   errCode = g_pSession1->Commit();
   if ( errCode != VDS_OK )
      goto end;

   errCode = q1.RemoveItem( str, 25 );
   if ( errCode != VDS_OK )
      goto end;   

   // A bit trivial...
   errCode = q1.RemoveItem( str, 25 );
   if ( errCode != VDS_IS_EMPTY )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_IS_EMPTY, 
              errCode );
      errCode = -1;
      goto end;
   }
   errCode = 0;
   
 end:

   q1.Close();
   g_pSession1->DestroyObject( "A Queue" ); 
   g_pSession1->Commit();

   if ( errCode != 0 )
      return -26;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test27()
{
   int errCode = 0;
   vdsQueue q1(*g_pSession1);
   vdsQueue q2(*g_pSession1);
   char str[25];
   
   errCode = g_pSession1->CreateObject( "A Queue", VDS_QUEUE );
   if ( errCode != VDS_OK )
      goto end;
   
   errCode = q1.Open( "A Queue" );
   if ( errCode != VDS_OK )
      goto end;
   errCode = q2.Open( "A Queue" );
   if ( errCode != VDS_OK )
      goto end;
   errCode = q1.InsertItem( "1234567890123", 14 );
   if ( errCode != VDS_OK )
      goto end;
   errCode = g_pSession1->Commit();
   if ( errCode != VDS_OK )
      goto end;

   errCode = q1.RemoveItem( str, 25 );
   if ( errCode != VDS_OK )
      goto end;
   
   errCode = q2.RemoveItem( str, 25 );
   if ( errCode != VDS_IS_EMPTY )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_IS_EMPTY, 
              errCode );
      errCode = -1;
      goto end;
   }
   errCode = 0;

 end:

   q1.Close();
   q2.Close();
   g_pSession1->DestroyObject( "A Queue" ); 
   g_pSession1->Commit();

   if ( errCode != 0 )
      return -27;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test28()
{
   int errCode = 0;
   vdsQueue q1(*g_pSession1);
   vdsQueue q2(*g_pSession2);
   char str[25];
   
   errCode = g_pSession1->CreateObject( "A Queue", VDS_QUEUE );
   if ( errCode != VDS_OK )
      goto end;
   
   errCode = q1.Open( "A Queue" );
   if ( errCode != VDS_OK )
      goto end;
   errCode = q1.InsertItem( "1234567890123", 14 );
   if ( errCode != VDS_OK )
      goto end;
   errCode = g_pSession1->Commit();
   if ( errCode != VDS_OK )
      goto end;
   errCode = q2.Open( "A Queue" );
   if ( errCode != VDS_OK )
      goto end;

   errCode = q1.RemoveItem( str, 25 );
   if ( errCode != VDS_OK )
      goto end;
   
   errCode = q2.RemoveItem( str, 25 );
   if ( errCode != VDS_IS_EMPTY )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_IS_EMPTY, 
              errCode );
      errCode = -1;
      goto end;
   }
   errCode = 0;

 end:

   q1.Close();
   q2.Close();
   g_pSession1->DestroyObject( "A Queue" ); 
   g_pSession1->Commit();

   if ( errCode != 0 )
      return -28;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test29()
{
   int errCode = 0;
   vdsQueue q1(*g_pSession1);
   vdsQueue q2(*g_pSession2);
   char str[25];
   
   errCode = g_pSession1->CreateObject( "A Queue", VDS_QUEUE );
   if ( errCode != VDS_OK )
      goto end;
   
   errCode = q1.Open( "A Queue" );
   if ( errCode != VDS_OK )
      goto end;
   errCode = q1.InsertItem( "1234567890123", 14 );
   if ( errCode != VDS_OK )
      goto end;
   errCode = g_pSession1->Commit();
   if ( errCode != VDS_OK )
      goto end;
   errCode = q2.Open( "A Queue" );
   if ( errCode != VDS_OK )
      goto end;

   errCode = q1.RemoveItem( str, 25 );
   if ( errCode != VDS_OK )
      goto end;
   errCode = g_pSession1->Commit();
   if ( errCode != VDS_OK )
      goto end;
   
   errCode = q2.RemoveItem( str, 25 );
   if ( errCode != VDS_IS_EMPTY )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_IS_EMPTY, 
              errCode );
      errCode = -1;
      goto end;
   }
   errCode = 0;

 end:

   q1.Close();
   q2.Close();
   g_pSession1->DestroyObject( "A Queue" ); 
   g_pSession1->Commit();

   if ( errCode != 0 )
      return -29;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test30( bool bSession1 )
{
   int errCode = 0;
   vdsQueue q1(*g_pSession1);
   char str[25];
   
   errCode = g_pSession1->CreateObject( "A Queue", VDS_QUEUE );
   if ( errCode != VDS_OK )
      goto end;
   
   errCode = q1.Open( "A Queue" );
   if ( errCode != VDS_OK )
      goto end;
   errCode = q1.InsertItem( "1234567890123", 14 );
   if ( errCode != VDS_OK )
      goto end;
   errCode = g_pSession1->Commit();
   if ( errCode != VDS_OK )
      goto end;

   errCode = q1.RemoveItem( str, 25 );
   if ( errCode != VDS_OK )
      goto end;
   errCode = g_pSession1->Rollback(); 
   if ( bSession1 == false ) // implicit
   {
      if ( errCode != VDS_NO_ROLLBACK )
      {
         printf( " Expected error = %d, returned error = %d\n", 
                 VDS_NO_ROLLBACK, 
                 errCode );
         errCode = -1;        
      }
      else
         errCode = 0;
      
      goto end;
   }
   else
      if ( errCode != VDS_OK )
         goto end;

   errCode = q1.RemoveItem( str, 25 );
   if ( errCode != VDS_OK )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_OK, 
              errCode );
      goto end;
   }

 end:

   q1.Close();
   g_pSession1->DestroyObject( "A Queue" ); 
   g_pSession1->Commit();

   if ( errCode != 0 )
      return -30;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test31( bool bSession1 )
{
   int errCode = 0;
   vdsQueue q1(*g_pSession1);
   vdsQueue q2(*g_pSession1);
   char str[25];
   
   errCode = g_pSession1->CreateObject( "A Queue", VDS_QUEUE );
   if ( errCode != VDS_OK )
      goto end;
   
   errCode = q1.Open( "A Queue" );
   if ( errCode != VDS_OK )
      goto end;
   errCode = q1.InsertItem( "1234567890123", 14 );
   if ( errCode != VDS_OK )
      goto end;
   errCode = g_pSession1->Commit();
   if ( errCode != VDS_OK )
      goto end;
   errCode = q2.Open( "A Queue" );
   if ( errCode != VDS_OK )
      goto end;

   errCode = q1.RemoveItem( str, 25 );
   if ( errCode != VDS_OK )
      goto end;
   errCode = g_pSession1->Rollback(); 
   if ( bSession1 == false ) // implicit
   {
      if ( errCode != VDS_NO_ROLLBACK )
      {
         printf( " Expected error = %d, returned error = %d\n", 
                 VDS_NO_ROLLBACK, 
                 errCode );
         errCode = -1;        
      }
      else
         errCode = 0;
      
      goto end;
   }
   else
      if ( errCode != VDS_OK )
         goto end;

   errCode = q2.RemoveItem( str, 25 );
   if ( errCode != VDS_OK )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_OK, 
              errCode );
      goto end;
   }

 end:

   q1.Close();
   q2.Close();
   g_pSession1->DestroyObject( "A Queue" ); 
   g_pSession1->Commit();

   if ( errCode != 0 )
      return -31;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test32( bool bSession1 )
{
   int errCode = 0;
   vdsQueue q1(*g_pSession1);
   vdsQueue q2(*g_pSession2);
   char str[25];
   
   errCode = g_pSession1->CreateObject( "A Queue", VDS_QUEUE );
   if ( errCode != VDS_OK )
      goto end;
   
   errCode = q1.Open( "A Queue" );
   if ( errCode != VDS_OK )
      goto end;
   errCode = q1.InsertItem( "1234567890123", 14 );
   if ( errCode != VDS_OK )
      goto end;
   errCode = g_pSession1->Commit();
   if ( errCode != VDS_OK )
      goto end;
   errCode = q2.Open( "A Queue" );
   if ( errCode != VDS_OK )
      goto end;

   errCode = q1.RemoveItem( str, 25 );
   if ( errCode != VDS_OK )
      goto end;
   errCode = g_pSession1->Rollback(); 
   if ( bSession1 == false ) // implicit
   {
      if ( errCode != VDS_NO_ROLLBACK )
      {
         printf( " Expected error = %d, returned error = %d\n", 
                 VDS_NO_ROLLBACK, 
                 errCode );
         errCode = -1;        
      }
      else
         errCode = 0;
      
      goto end;
   }
   else
      if ( errCode != VDS_OK )
         goto end;

   errCode = q2.RemoveItem( str, 25 );
   if ( errCode != VDS_OK )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_OK, 
              errCode );
      goto end;
   }

 end:

   q1.Close();
   q2.Close();
   g_pSession1->DestroyObject( "A Queue" ); 
   g_pSession1->Commit();
   g_pSession2->Commit();

   if ( errCode != 0 )
      return -32;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Run( bool bSession1, bool /* bSession2 */ )
{
   int errCode = 0;

   printf( " Executing test 1\n" );
   if ( (errCode = Test1())  != 0 ) return errCode;
   printf( " Executing test 2\n" );
   if ( (errCode = Test2())  != 0 ) return errCode;
   printf( " Executing test 3\n" );
   if ( (errCode = Test3())  != 0 ) return errCode;
   printf( " Executing test 4\n" );
   if ( (errCode = Test4( bSession1 )) != 0 ) return errCode;
   printf( " Executing test 5\n" );
   if ( (errCode = Test5()) != 0 ) return errCode;
   printf( " Executing test 6\n" );
   if ( (errCode = Test6()) != 0 ) return errCode;
   printf( " Executing test 7\n" );
   if ( (errCode = Test7()) != 0 ) return errCode;
   printf( " Executing test 8\n" );
   if ( (errCode = Test8()) != 0 ) return errCode;
   printf( " Executing test 9\n" );
   if ( (errCode = Test9( bSession1 )) != 0 ) return errCode;
   printf( " Executing test 10\n" );
   if ( (errCode = Test10( bSession1 )) != 0 ) return errCode;
   printf( " Executing test 11\n" );
   if ( (errCode = Test11())  != 0 ) return errCode;
   printf( " Executing test 12\n" );
   if ( (errCode = Test12())  != 0 ) return errCode;
   printf( " Executing test 13\n" );
   if ( (errCode = Test13( bSession1 )) != 0 ) return errCode;

   printf( " Executing test 21\n" );
   if ( (errCode = Test21())  != 0 ) return errCode;
   printf( " Executing test 22\n" );
   if ( (errCode = Test22())  != 0 ) return errCode;
   printf( " Executing test 23\n" );
   if ( (errCode = Test23())  != 0 ) return errCode;
   printf( " Executing test 24\n" );
   if ( (errCode = Test24())  != 0 ) return errCode;
   printf( " Executing test 25\n" );
   if ( (errCode = Test25( bSession1 ))  != 0 ) return errCode;

   printf( " Executing test 26\n" );
   if ( (errCode = Test26())  != 0 ) return errCode;
   printf( " Executing test 27\n" );
   if ( (errCode = Test27())  != 0 ) return errCode;
   printf( " Executing test 28\n" );
   if ( (errCode = Test28())  != 0 ) return errCode;
   printf( " Executing test 29\n" );
   if ( (errCode = Test29())  != 0 ) return errCode;
   printf( " Executing test 30\n" );
   if ( (errCode = Test30( bSession1 ))  != 0 ) return errCode;
   printf( " Executing test 31\n" );
   if ( (errCode = Test31( bSession1 ))  != 0 ) return errCode;
   printf( " Executing test 32\n" );
   if ( (errCode = Test32( bSession1 ))  != 0 ) return errCode;

   return errCode;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int main(int argc, char *argv[]) 
{
   int errCode = 0;
   vdsProcess process;

   if ( argc != 4 )
   {
      printf( " Usage: %s WatchdogAddress 0/1 test_id\n", argv[0] );
      return -1;
   }
   
   errCode = process.Initialize( argv[1] );   
   if ( errCode != VDS_OK )
   {
      printf( " Error opening VDS = %d\n", errCode );
      return -1;
   }
   
   printf( " Both Sessions - implicit\n" );
   
   g_pSession1 = new vdsSession( false );
   g_pSession2 = new vdsSession( false );

   vdsQueue q1(*g_pSession1);

   errCode = g_pSession1->CreateObject( "A Queue", VDS_QUEUE );
   if ( errCode != VDS_OK )
      raise( 11 );

   raise( 9 );
   
   if ( (errCode = Run( false, false ))  != 0 ) goto error;
   
   //    
   printf( " Both Sessions - explicit\n" );

   delete g_pSession1;
   delete g_pSession2;
   g_pSession1 = new vdsSession( true );
   g_pSession2 = new vdsSession( true );

   if ( (errCode = Run( true, true ))  != 0 ) goto error;
  
   //    
   printf( " Session 1 implicit - session 2 explicit\n" );

   delete g_pSession1;
   delete g_pSession2;
   g_pSession1 = new vdsSession( false );
   g_pSession2 = new vdsSession( true );

   if ( (errCode = Run( false, true ))  != 0 ) goto error;
  
   //    
   printf( " Session 1 explicit - session 2 implicit\n" );

   delete g_pSession1;
   delete g_pSession2;
   g_pSession1 = new vdsSession( true );
   g_pSession2 = new vdsSession( false );

   if ( (errCode = Run( true, false ))  != 0 ) goto error;

   printf( "Terminating normally\n" );

   return 0;

 error:
   fprintf( stderr, " Error in test %d, Aborting anormally\n", errCode ); 

   return -1;
}


