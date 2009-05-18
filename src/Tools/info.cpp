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

#include <photon/photon>
#include <iostream>

using namespace std;
using namespace pso;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int main()
{
   Process process;
   Session session;
   psoInfo info;
   string msg;
   
   try {
      process.Init( "10701", "psoinfo" );
      session.Init();
   }
   catch( Exception exc ) {
      cerr << "Init of Photon failed, error = " << exc.Message( msg ) << endl;
      cerr << "Is quasar running?" << endl;
      return 1;
   }

   try {
      session.GetInfo( info );
   }
   catch( Exception exc ) {
      cerr << exc.Message( msg ) << endl; 
      return 1;
   }

   cout << "Information from Photon shared memory at port 10701" << endl << endl;
   
   cout << "Memory total size (kb)  " << info.totalSizeInBytes/1024 << endl;
   cout << "allocated memory (kb)   " << info.allocatedSizeInBytes/1024 << endl;
   cout << "number of objects       " << info.numObjects << endl;
   cout << "number of groups        " << info.numGroups << endl;
   cout << "number of mallocs       " << info.numMallocs << endl;
   cout << "number of frees         " << info.numFrees << endl;
   cout << "largest free space (kb) " << info.largestFreeInBytes/1024 << endl;

   cout << "Photon memory version   " << info.memoryVersion << endl;
   cout << "C compiler name         " << info.compiler << endl;
   cout << "Compiler version        " << info.compilerVersion << endl;
   cout << "Platform                " << info.platform << endl;
   cout << "Shared libs version     " << info.dllVersion << endl;
   cout << "Quasar version          " << info.quasarVersion << endl;
   cout << "Memory time of creation " << info.creationTime << endl;

   if ( info.bigEndian == 1 )
      cout << "This platform is big endian" << endl;
   else
      cout << "This platform is little endian" << endl;
   
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
