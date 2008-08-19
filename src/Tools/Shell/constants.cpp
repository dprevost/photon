/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of photon (photonsoftware.org).
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

#include "Tools/Shell/constants.h"
#include <string>
#include <sstream>
#include <iostream>
#include <vector>

#define KILO_INT          1024
#define KILO_DOUBLE       1024.0
#define MEGA_INT       1048576
#define MEGA_DOUBLE    1048576.0
#define GIGA_INT    1073741824
#define GIGA_DOUBLE 1073741824.0

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

vdsConstants::vdsConstants()
{
   // Make sure that they have equal length.
   types.push_back( "Unknown " );
   types.push_back( "Folder  " );
   types.push_back( "Queue   " );
   types.push_back( "Hash Map" );
   headerType =     "Type    ";

   stats.push_back( "Normal " );
   stats.push_back( "Deleted" );
   stats.push_back( "Added  " );
   headerStatus =   "Status ";
//   stats.push_back( "Replaced" );
//   PSN_TXS_DESTROYED_COMMITTED
   stats.push_back( "Unknown" );
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

vdsConstants::~vdsConstants()
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

const string &
vdsConstants::Bytes( long numBytes )
{
   double d = numBytes;
   ostringstream oss;
   
   oss.precision(2);
   if ( numBytes > GIGA_INT ) {
      d = d / GIGA_DOUBLE;
      oss << fixed << d << " Gbytes";
   }
   else if ( numBytes > MEGA_INT ) {
      d = d / MEGA_DOUBLE;
      oss << fixed << d << " Mbytes";
   }
   else if ( numBytes > 10*KILO_INT ) {
      d = d / KILO_DOUBLE;
      oss << fixed << d << " Kbytes";
   }
   else {
      oss << numBytes << " bytes";
   }
   
   return strSize = oss.str();
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

const string &
vdsConstants::Type( vdsObjectType type )
{
   int i = type;
   
   if ( type < VDS_FOLDER || type >= VDS_LAST_OBJECT_TYPE ) i = 0;
   
   return types[i];
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

const string &
vdsConstants::Status( int status )
{
   if ( status < 0 || status > 2 ) status = 3;
   
   return stats[status];
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

