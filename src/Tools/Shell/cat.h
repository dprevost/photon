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

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#ifndef PSO_SHELL_CAT_H
#define PSO_SHELL_CAT_H

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

class psoCat
{
public:

   psoCat( Session s, string name );
   
   ~psoCat();

   // Returns an error string on error
   string Init();
   
   void Run();
   
private:

   Session session;
   string objectName;
   psoObjStatus status;
   unsigned char * key;
   unsigned char * buffer;
   uint32_t * keyOffsets;
   uint32_t * fieldOffsets;
   psoObjectDefinition objDefinition;
   unsigned char * fieldDef;
   unsigned char * keyDef;
   uint32_t fieldDefLength;
   uint32_t keyDefLength;
   
   string binToStr( unsigned char * buff, uint32_t length );

   string odbcFieldToStr( uint32_t length );

   string odbcKeyToStr( uint32_t length );

   void readBinary( string        & outStr,
                    size_t          binLength,
                    unsigned char * buffer );

   string readDate( unsigned char * buffer );
   
   void readDecimal( string        & outStr,
                     size_t          precision,
                     unsigned char * buffer );

   void readFloat( string        & outStr,
                   size_t          binLength,
                   unsigned char * buffer );

   void readInt( string        & outStr,
                 size_t          intLength,
                 unsigned char * buffer );

   void readString( string        & outStr,
                    size_t          strLength,
                    unsigned char * buffer );

   string readTime( unsigned char * buffer );
   
   string readTimeStamp( unsigned char * buffer );
};

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#endif // PSO_SHELL_CAT_H

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--


