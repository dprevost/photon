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

#ifndef VDSTOOLS_SHELL_H
#define VDSTOOLS_SHELL_H

#include "Common/Common.h"
#include <vdsf/vds>
#include <string>
#include <iostream>
#include <vector>

using namespace std;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

class vdsShell
{
public:
   vdsShell(vdsSession & s);
   ~vdsShell();
   
   bool Dispatch();
   
   void Parse( string & inStr );
   
   void Run();

   // The "list" of currently implemented pseudo-shell commands starts here.

   void cd();
   
   void ls();
   
   void mkdir();
   
private:

   string currentLocation;
   vdsSession & session;
   vector<string> tokens;
   
};
#endif // VDSTOOLS_SHELL_H

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
