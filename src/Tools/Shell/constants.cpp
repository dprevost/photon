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

psoConstants::psoConstants()
{
   // Make sure that they have equal length.
   types.push_back( "Unknown   " );
   types.push_back( "Folder    " );
   types.push_back( "Hash Map  " );
   types.push_back( "LIFO Queue" );
   types.push_back( "Fast Map  " );
   types.push_back( "FIFO Queue" );
   headerType =     "Type      ";

   stats.push_back( "Normal " );
   stats.push_back( "Deleted" );
   stats.push_back( "Added  " );
   headerStatus =   "Status ";
//   stats.push_back( "Replaced" );
//   PSON_TXS_DESTROYED_COMMITTED
   stats.push_back( "Unknown" );
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

psoConstants::~psoConstants()
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

const string &
psoConstants::Bytes( long numBytes )
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
psoConstants::Type( psoObjectType type )
{
   int i = type;
   
   if ( type < PSO_FOLDER || type >= PSO_LAST_OBJECT_TYPE ) i = 0;
   
   return types[i];
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

const string &
psoConstants::Status( int status )
{
   if ( status < 0 || status > 2 ) status = 3;
   
   return stats[status];
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

