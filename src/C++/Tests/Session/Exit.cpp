/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#include "Common/Common.h"
#include <vdsf/vds>
#include <iostream>

using namespace std;


/* 
 * This test is for ticket 1836613. It seems that vdsExitSession() is not
 * closing open objects - the bug will be seen when the session is reopen
 * and a new attempt will be made to create the object. This attempt should 
 * work since in the previous pass:
 *   - the object was created but not committed
 *   - an automated rollback was perform with ExistSession()
 *   - an automated close object was performed with vdsExitSession (which
 *     should put all ref. counters to zero, deleting the object from the vds)
 *
 * Note: for c++, we must do it a bit differently as there is no exit()
 *       function for sessions - it is implicitely called by the destructor.
 */
 
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   vdsProcess process;
   vdsSession * session = NULL;
   vdsFolder * folder = NULL;
   string name = "/cpp_session_exit";
   vdsObjStatus status;

   try {
      if ( argc > 1 ) {
         process.Init( argv[1] );
      }
      else {
         process.Init( "10701" );
      }
   }
   catch( vdsException exc ) {
      cerr << "Test failed in init phase, error = " << exc.Message() << endl;
      cerr << "Is the watchdog running?" << endl;
      return 1;
   }

   try {
      session = new vdsSession();
      folder = new vdsFolder(*session);
      
      session->Init();
      session->CreateObject( name, VDS_FOLDER );
      folder->Open( name );
      
      delete session;
   }
   catch( vdsException exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   
   // Try to use the folder object. 
   try {
      folder->Status( &status );
      // Should never come here
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   catch( vdsException exc ) {
      if ( exc.ErrorCode() != VDS_SESSION_IS_TERMINATED ) {
         cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
         return 1;
      }
   }
   
#if 0   
   VDS_HANDLE sessionHandle, objHandle;
   int errcode;
   

   errcode = vdsExitSession( sessionHandle );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsInitSession( &sessionHandle );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* */
   errcode = vdsCreateObject( sessionHandle, "test1", 5, VDS_FOLDER );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdsExitSession( sessionHandle );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   vdsExit();
#endif
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

