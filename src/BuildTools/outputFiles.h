/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef OUTPUT_FILES_H
#define OUTPUT_FILES_H

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#include "Common/Common.h"
#include <exception>
#include <iostream>
#include <fstream>
#include <list>
#include <string>

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

class outfileException: public std::exception
{
   virtual const char* what() const throw() {
      return strerror(errno);
   }
};

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

class outputFiles
{
 public:

   /// The constructor does not throw exceptions.
   outputFiles() throw();
   
   /// The destructor does not throw exceptions.
   virtual ~outputFiles() throw();   

   void OpenFiles( const char* filenamePrefix, const char* dataPrefix );

   void CloseFiles() throw() {
      m_stream_c.close();
      m_stream_h.close();
   }   

   void AddEntry( int value, std::string& comment, std::string& enumName );
   
   void Write( const char* inFilename, const char* prefix );
   
 private:

   struct entry
   {
      int value;
      std::string comment;
      std::string enumName;
   };
   
   std::list<entry> m_list;
   
   std::string::size_type m_longestString;
   
   std::string m_ifdefname;
   std::string m_headerName;
   
   outfileException m_exception;
   
   std::ofstream m_stream_c;
   std::ofstream m_stream_h;
   
   /// The name of this code generator.
   static const char* m_programName;
   
   void WriteHeader( std::ofstream& stream, const char* inFilename );

};

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#endif // OUTPUT_FILES_H
