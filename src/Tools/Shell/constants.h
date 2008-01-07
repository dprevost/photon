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

#ifndef VDSTOOLS_CONSTANTS_H
#define VDSTOOLS_CONSTANTS_H

#include "Common/Common.h"
#include <vdsf/vds>
#include <string>
#include <iostream>
#include <vector>

using namespace std;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

// This class is used for transforming raw data into a more screen-friendly
// output. 

class vdsConstants
{
public:
   vdsConstants();
   ~vdsConstants();
   
   const string & Type( vdsObjectType type );
   
   const string & Status( int status );

   const string & Bytes( long );
   
private:

   vector<string> types;
   vector<string> stats;
   
   string strSize;
};
#endif // VDSTOOLS_CONSTANTS_H

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
