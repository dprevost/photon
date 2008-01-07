/* -*- c++ -*- */
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

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#ifndef ANALYSER_H
#define ANALYSER_H

#include "Common/Common.h"
#include <exception>
#include <string>

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

class inputHeader;
class outputFiles;

// All the derived classes will use this class as the superclass.
class analyseException: public std::exception
{
};

class noEnumException: public analyseException
{
   virtual const char* what() const throw()
      {
         return "The keyword \"enum\" was not found.";
      }
};

class noNameException: public analyseException
{
   virtual const char* what() const throw()
      {
         return "The name of the enum was not found.";
      }
};

class noOpenCurlyException: public analyseException
{
   virtual const char* what() const throw()
      {
         return "The open curly bracket, \"{\", was not found.";
      }
};

class emptyEnumException: public analyseException
{
   virtual const char* what() const throw()
      {
         return "The enum data structure is empty.";
      }
};

class noCloseCurlyException: public analyseException
{
   virtual const char* what() const throw()
      {
         return "The close curly bracket, \"}\", was not found.";
      }
};

class formatErrorException: public analyseException
{
   virtual const char* what() const throw()
      {
         return "Format error - enum token not found.";
      }
};

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

class analyser
{
 public:

   analyser( inputHeader* input, outputFiles* output );
   
   ~analyser();

   void ReadInput();
   void ProcessInput();
   void WriteOutput();
   
   void StripComments();

 private:

   inputHeader  * m_pInput;
   outputFiles * m_pOutput;

   /// The input data provided to us by the inputHeader object.
   std::string m_inputData;

   /// A stripped version (comments were stripped) of the input data.
   std::string m_stripData;

   /// The current numerical value of the enum
   int m_enumNumericalValue;

   /// true if the comments are placed before the code, false otherwise.
   bool m_commentsBeforeCode;
   
   void CreatePattern( const std::string& line,
                       std::string&       pattern );

   std::string m_pattern;

   noEnumException       m_noEnumException;
   noNameException       m_noNameException;
   noOpenCurlyException  m_noOpenCurlyException;
   noCloseCurlyException m_noCloseCurlyException;
   emptyEnumException    m_emptyEnumException;
   formatErrorException  m_formatErrorException;
   
   bool AreCommentsBeforeCode( std::string::size_type currentLocation,
                               std::string::size_type endSearch );

   /// Get a new line from the inputHeader object and ajusts array sizes, if
   /// needed.
   void GetData();

   /// Retrieves the next valid token from the stripped input string.
   std::string::size_type GetNextToken( std::string::size_type startSearch,
                                        std::string& tokenString );

   std::string::size_type GetEnumKeyword() throw(noEnumException);


   std::string::size_type IdentifyElements( 
      std::string::size_type  currentLocation,
      std::string::size_type  endSearch,
      std::string::size_type& startTokenLocation,
      std::string::size_type& endTokenLocation,
      std::string::size_type& startCommentLocation,
      std::string::size_type& endCommentLocation );
   
   void ParseEnum( std::string::size_type startSearch,
                   std::string::size_type endSearch );

   std::string GetComment( std::string::size_type startComment,
                           std::string::size_type endComment );

   void GetEnumValue( std::string::size_type startToken,
                      std::string::size_type endToken,
                      std::string&           enumName );

   friend class analyserTest;
   
};

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#endif // ANALYSER_H
