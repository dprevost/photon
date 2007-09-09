/*
 * Copyright (C) 2006-2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef INPUT_HEADER_H
#define INPUT_HEADER_H

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#include "Common/Common.h"
#include <exception>
#include <iostream>
#include <fstream>

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

// This class was, originally, supposed to be derived from std::exception. 
// However, there was a problem when I started to use a std::string to
// hold the message - the destructor of the string class can throw an 
// exception... but not the destructor of std::exception calling it (gcc 
// identify this, correctly, as an error).
//
// So, technically, this exception class can itself throw an exception. 
// Very unlikely (and the whole point is to inform the caller of the
// real potential issues like non-existing file or i/o errors).

class fileException
{
 public:

   fileException() 
      {
      }
   
   virtual ~fileException() 
      {
      }
   
   virtual void SetMessage( const char* msg ) 
      {
         s = msg;
         if ( errno != 0 )
         {
            s += ": ";
            s += strerror(errno);
         }
      }
   virtual void SetMessage( const char* msg, size_t lineNumber ) 
      {
         char dummy[128];

         SetMessage( msg );
         sprintf( dummy, "%s%d%s", 
                  "\n(approximate line number of the problem: ",
                  lineNumber,
                  ")" );
         
         s += dummy;
      }
   
   virtual const std::string& what() const throw()
      {
         return s; // strerror(errno);
      }

 private:
   std::string s;

};

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

class inputHeader
{
 public:

   inputHeader();
   
   ~inputHeader();
   
   /// Grab the whole file as a single string
   void GetData( std::string& s );

   void OpenFile( const char* filename );
   
 private:

   fileException m_exception;
   
   std::ifstream m_stream;

};

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#endif // INPUT_HEADER_H
