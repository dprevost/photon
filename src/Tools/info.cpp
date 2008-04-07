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

#include <vdsf/vds>
#include <iostream>

using namespace std;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int main()
{
   vdsProcess process;
   vdsSession session;
   vdsInfo info;
   string msg;
   
   try {
      process.Init( "10701" );
      session.Init();
   }
   catch( vdsException exc ) {
      cerr << "Init VDSF failed, error = " << exc.Message( msg ) << endl;
      cerr << "Is the watchdog running?" << endl;
      return 1;
   }

   try {
      session.GetInfo( &info );
   }
   catch( vdsException exc ) {
      cerr << exc.Message( msg ) << endl; 
      return 1;
   }

   cout << "Information from vds at port 10701" << endl << endl;
   
   cout << "vds total size     " << info.totalSizeInBytes << endl;
   cout << "allocated memory   " << info.allocatedSizeInBytes << endl;
   cout << "number of objects  " << info.numObjects << endl;
   cout << "number of groups   " << info.numGroups << endl;
   cout << "number of mallocs  " << info.numMallocs << endl;
   cout << "number of frees    " << info.numFrees << endl;
   cout << "largest free space " << info.largestFreeInBytes << endl;

   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
