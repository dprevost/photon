/*
 * Copyright (C) 2008-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Common/Common.h"
#include <photon/photon>
#include "Tests/PrintError.h"
#include <iostream>

using namespace std;
using namespace pso;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   Process process;
   Session session;
   Folder folder;
   string name = "/cpp_folder_getnext";
   string sub1name = name + "/f1";
   string sub2name = name + "/f2";
   psoFolderEntry entry;
   int rc;

   try {
      if ( argc > 1 ) {
         process.Init( argv[1], argv[0] );
      }
      else {
         process.Init( "10701", argv[0] );
      }
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed in init phase, error = " << exc.Message() << endl;
      cerr << "Is the server running?" << endl;
      return 1;
   }

   try {
      session.Init();
      session.CreateFolder( name );
      session.CreateFolder( sub1name );
      session.CreateFolder( sub2name );
      folder.Open( session, name );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   // No GetFirst
   try {
      folder.GetNext( entry );
   }
   catch( pso::Exception exc ) {
      if ( exc.ErrorCode() != PSO_INVALID_ITERATOR ) {
         cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
         return 1;
      }
   }

   try {
      folder.GetFirst( entry );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   // End of invalid args. This call should succeed.
   try {
      rc = folder.GetNext( entry );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }
   if ( rc != 0 ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   
   try {
      rc = folder.GetNext( entry );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }
   if ( rc != PSO_REACHED_THE_END ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

