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
#include "Tools/Shell/constants.h"

using namespace std;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

class vdsShell
{
public:
   vdsShell(vdsSession & s);
   ~vdsShell();

   void Run();
   
private:

   string currentLocation;
   vdsSession & session;
   vector<string> tokens;
   vdsConstants constants;
   
   bool Dispatch();
   
   void Parse( string & inStr );

   string & Trim( string & inStr );
   
   // The "list" of currently implemented pseudo-shell commands starts here.

   void cd();
   
   void free();
   
   void ls();
   
   void man();
   
   void mkdir();
   
   void rmdir();

   void stat();
   
   // cp, mv, file cat, history(?), wc, vi/ed(?), chmod
   // cmp, rm, grep, diff, head, tail, du, df, uname, ln, find, touch,
   // uptime
};

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#endif // VDSTOOLS_SHELL_H

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

