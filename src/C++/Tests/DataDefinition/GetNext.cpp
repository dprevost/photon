/*
 * Copyright (C) 2009 Daniel Prevost <dprevost@photonsoftware.org>
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
   DataDefinition dataDef;
   string name = "/cpp_datadefinition_get_definition";
   unsigned char byteData[50];
   unsigned int dataLength = 50;
   string fieldDescription;
   
   memcpy( &byteData[0],  "Field 1  ", 9 );
   memcpy( &byteData[10], "Field 2  ", 9 );
   memcpy( &byteData[20], "Field 3  ", 9 );
   memcpy( &byteData[30], "Field 4  ", 9 );
   memcpy( &byteData[40], "Field 5  ", 9 );
   byteData[9]  = 0;
   byteData[19] = 0;
   byteData[29] = 0;
   byteData[39] = 0;
   byteData[49] = 0;
   
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

   // DataDefinition is not initialized

   try {
      fieldDescription = dataDef.GetNext();
      // Should never come here
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   catch( pso::Exception exc ) {
      if ( exc.ErrorCode() != PSO_NULL_HANDLE ) {
         cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
         return 1;
      }
   }

   try {
      session.Init();
      dataDef.Create( session, 
                      name,
                      PSO_DEF_USER_DEFINED,
                      byteData,
                      dataLength );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   // This call should work
   
   try {
      do {
         fieldDescription = dataDef.GetNext();
         cout << fieldDescription << endl;
      } while ( fieldDescription.length() > 0 );
   }
   catch( pso::Exception exc ) {
      cerr << exc.ErrorCode() << endl;
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   dataDef.Close();
   
   return 0;
}
