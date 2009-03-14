/* -*- c++ -*- */
/* :mode=c++:  - For jedit, previous line for emacs */
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

#ifndef PSO_SHELL_SHELL_H
#define PSO_SHELL_SHELL_H

#include "Common/Common.h"
#include <photon/photon>
#if defined(WIN32)
#  pragma warning(disable: 4786)
#endif
#include <string>
#include <iostream>
#include <vector>
#include "Tools/Shell/constants.h"

using namespace std;
using namespace pso;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

class psoShell
{
public:
   psoShell(Session & s);
   ~psoShell();

   void Run();
   
private:

   string currentLocation;
   Session & session;
   vector<string> tokens;
   psoConstants constants;
   
   bool Dispatch();
   
   void Parse( string & inStr );

   string & Trim( string & inStr );
   
   // The "list" of currently implemented pseudo-shell commands starts here.

   // cat is complex. It is implemented using an object (see code for details).
   void Cat();
   
   void Cd();

   void Cp();
   
   void Echo();
   
   void Free();
   
   void Ls();
   
public:
   // man is public so that it can be use if the user do psosh --help (or
   // variants).
   void Man();
   
private:
   void Mkdir();
   
   void Rm();
   
   void Rmdir();

   void Stat();
   
   void Touch();
      
   // mv, file, history(?), wc, vi/ed(?), chmod
   // cmp, grep, diff, head, tail, du, df, uname, ln, find,
   // uptime
};

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#endif // PSO_SHELL_SHELL_H

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

