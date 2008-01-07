/*
 * Copyright (C) 2006, 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of the vdsf (Virtual Data Space Framework) Library.
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation and appearing in the file COPYING included in the
 * packaging of this library.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#include "inputHeader.h"

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

inputHeader::inputHeader()
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

inputHeader::~inputHeader()
{
   m_stream.close();
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void inputHeader::GetData( std::string& s )
{
   std::string tmp;
   int lineCounter = 0;
#if defined (WIN32)
   char tmpLine[10000];
#endif

   s.resize(0); // Just to be safe.
   
   while ( true )
   {
      lineCounter++;
      tmp.resize(0);
#if defined (WIN32)
      m_stream.getline( tmpLine, 10000 );
      tmp = tmpLine;
#else
      std::getline( m_stream, tmp );
#endif
      if ( ! m_stream.good() )
      {
         if ( m_stream.eof() )
            return;
         else
         {
            m_exception.SetMessage( "Error reading input file", lineCounter );
            throw m_exception;
         }
      }
      if ( tmp.length() > 0 )
      {
         s += tmp;
         // Needed for analysing comments (they are the terminators of c++
         // style comments, for example).
         s += "\n"; 
      }
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void inputHeader::OpenFile( const char* filename )
{
   m_stream.open( filename );
   if ( ! m_stream.is_open() )
   {
      m_exception.SetMessage( "Error opening file" );
      throw m_exception;
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

