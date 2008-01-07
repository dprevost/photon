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

#include "analyser.h"
#include "inputHeader.h"
#include "outputFiles.h"
#include <iostream>
#include <string>

using namespace std;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

analyser::analyser( inputHeader * input, outputFiles * output )
   : m_pInput             ( input  ),
     m_pOutput            ( output ),
     m_enumNumericalValue ( 0      ),
     m_commentsBeforeCode ( true   )
{
   // This is way too big but... not a big waste (and it makes the code
   // faster.
   m_inputData.reserve( 100*1024 );
   m_stripData.reserve  ( m_inputData.length() );

}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

analyser::~analyser()
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

string analyser::GetComment( string::size_type startComment,
                             string::size_type endComment )
{
   string::size_type i;
   string s, s1;
   
   if ( startComment == string::npos )
      return s;
   
   for ( i = startComment; i < endComment; ++i )
   {
      if ( m_pattern[i] == 'c' || m_pattern[i] == '+' )
      {
         // A spacial case, '"' needs to be escaped. 
         if ( m_inputData[i] == '"' )
            s += '\\';
         s += m_inputData[i];
      }

      // A \n at the beginning would not be inside a comment so we can 
      // skip that one. Note also that we don't care about c++ comments,
      // only c comments. See next loop for the why...
      if ( m_pattern[i] == 'n' && i > startComment )
      {
         if (  m_pattern[i-1] == 'c' )
            s += m_inputData[i];
         // A limit case where \n is just after a /*
         else if ( i < endComment-1 && m_pattern[i-1] == 's' )
         {
            if (  m_pattern[i+1] == 'c' )
               s += m_inputData[i];
         }
      }
   }
   
   // Now we use the \n in our comment to see if there are occurences of
   // multilines c comments where each line starts with a " * ". For example:
   /*
    * Example...
    */
   // We want to return "Example..." not "* Example..."
   i = 0;
   while ( i < s.length() )
   {
      if ( s[i] == '\n' )
      {
         s1 += ' ';
         
         i++;
         while ( i < s.length()-1 )
         {
            if ( s[i] == '*' )
            {
               // We only skip if the following character is a space
               if ( ! isspace(s[i+1]) )
                  s1 += s[i];
               break;
            }
            s1 += s[i];
            
            if ( ! isspace(s[i]) )
               break;
            
            i++;
         }         
      }
      else
         s1 += s[i];

      i++;
   }
   
   s.resize(0);

   // Trim white space (remove it from the beginning the end and everywhere
   // there is more than one occurence).
   bool starting = true;
   for ( i = 0; i < s1.length()-1; ++i )
   {
      if ( isspace(s1[i]) )
      {
         if ( starting ) continue;

         if ( ! isspace(s1[i+1]) )
              s += s1[i];
      }
      else
      {
         s += s1[i];
         starting = false;
      }
   }
   if ( ! isspace(s1[s1.length()-1]) )
      s += s1[i];

   return s;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void analyser::GetEnumValue( string::size_type startToken,
                             string::size_type endToken,
                             string&           enumName )
{
   string::size_type equalLocation;
   string::size_type valueLocation;
   string s;
   const char* str;
   char* endStr;
   
   GetNextToken( startToken, enumName );

   equalLocation = m_stripData.find( '=', startToken );

   if ( equalLocation > endToken )
      equalLocation = string::npos;
   
   if ( equalLocation == string::npos )
   {
      m_enumNumericalValue++;
   }
   else
   {
      valueLocation = GetNextToken( equalLocation+1, s );
      if ( valueLocation == string::npos )
         throw m_formatErrorException;
   
      str = m_stripData.c_str();
      m_enumNumericalValue = strtol( &str[valueLocation], &endStr, 0 );
      if ( endStr == &str[valueLocation] )
         throw m_formatErrorException;
   }

}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

string::size_type analyser::GetEnumKeyword() throw(noEnumException)
{
   using namespace std;
   
   string::size_type currentLocation = 0;
   string::size_type enumLocation;
   bool found = false;

   // We need to make sure that we found the real enum keyword, not
   // something like "#define MY_ENUM my_enum"... it probably won't
   // happen but...
   while ( ! found )
   {
      enumLocation = m_stripData.find( "enum", currentLocation );
      if ( enumLocation == string::npos )
      {
         // Not a valid header file
         throw m_noEnumException;
      }
      found = true;
      
      if ( enumLocation > 0 )
         if ( ! isspace(m_stripData[enumLocation-1]) )
            found = false;
      if ( enumLocation+4 < m_stripData.length() )
         if ( ! isspace(m_stripData[enumLocation+4]) )
            found = false;

      currentLocation = enumLocation + 4;      
   }

   return enumLocation;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

string::size_type analyser::GetNextToken( 
   string::size_type startSearch,
   string& tokenString )
{
   string::size_type i;
   string::size_type beginToken = string::npos;

   tokenString.resize(0);
   
   // Find the first "non-space" character.
   for ( i = startSearch; i < m_stripData.length(); ++i )
   {
      if ( ! isspace(m_stripData[i]) ) 
      {
         beginToken = i;
         break;
      }
   }
   
   // Stop processing when we see a white space. Of course...
   if ( beginToken != string::npos )
   {
      for ( i = beginToken; i < m_stripData.length(); ++i )
      {
         if ( ! isspace(m_stripData[i]) ) 
            tokenString += m_stripData[i];
         else
            break;
      }
   }

   return beginToken;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void analyser::ProcessInput()
{
   using namespace std;
   
   string::size_type currentLocation = 0;
   string::size_type location;
   
   string s;
//   bool found = false;
   
   CreatePattern(  m_inputData, m_pattern )  ;

   StripComments();

   // This call will throw an exception if not found. We let main()
   // handle this.
   location = GetEnumKeyword();

   currentLocation = location + 4;
   
   location = GetNextToken( currentLocation + 4, s );
   if ( location == string::npos )
   {
      // Not a valid header file
      throw m_noNameException;      
   }
   currentLocation = location + s.length();

   // We do it that way to avoid giving an invalid position to the
   // call to string::find() (which would throw an exception). This
   // way the diagnostic is easier.
   if ( currentLocation >= m_stripData.length() )
      throw m_noOpenCurlyException;

   location = m_stripData.find( '{', currentLocation );
   if ( location == string::npos )
   {
      throw m_noOpenCurlyException;
   }
   currentLocation = location + 1;

   if ( currentLocation >= m_stripData.length() )
      throw m_noCloseCurlyException;

   location = m_stripData.find( '}', currentLocation );
   if ( location == string::npos )
   {
      throw m_noCloseCurlyException;
   }

   m_commentsBeforeCode = AreCommentsBeforeCode( currentLocation, location );
 
   ParseEnum( currentLocation, location );

}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void analyser::ReadInput()
{
   // GetData from the inputHeader class can throw usual exceptions
   // (allocation issue with the string class, error reading the
   // file, etc.). These will be caught by main().

   m_pInput->GetData( m_inputData );

   m_stripData.reserve( m_inputData.length() );
   m_pattern.resize   ( m_inputData.length() );
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

/*
 * This function is a bit limited in some ways but it does the job so...
 *
 * It scans the input string, searching for a comment. It will mark the
 * "pattern" string to indicate its finding.
 *
 * Note: the function stops scanning the string when it encounters a C
 *       end-of-comments. It will then call itself again with what ever
 *       is left unscanned.
 */
void analyser::CreatePattern( const string& line,
                              string&       pattern )
{
   string::size_type len = line.length();
   string::size_type i;

   string::size_type startText    = 0;
   string::size_type endText      = 0;
   string::size_type startComment = 0;
   string::size_type endComment   = 0;

   string::size_type unfinished = string::npos;
   string::size_type findEndComment      = string::npos;
   string::size_type findStartCppComment = string::npos;
   string::size_type findStartCComment   = string::npos;

   bool cppComment = false;
   
   findStartCComment   = line.find( "/*" );
   findStartCppComment = line.find( "//" );
   // c comments can be in cpp comments and the other way around.
   // We must determine which came first.
   if ( findStartCComment != string::npos && 
        findStartCppComment != string::npos )
   {
      if ( findStartCComment < findStartCppComment )
         findStartCppComment = string::npos;
      else
         findStartCComment = string::npos;
   }
   
   if ( findStartCComment != string::npos )
   {
      /* We have a C comment */
      
      startText    = 0;
      endText = findStartCComment;
      startComment = findStartCComment + 2 ;
      
      for ( i = startText; i < endText; ++i )
         pattern[i] = 't';
      pattern[endText]   = 's';
      pattern[endText+1] = 's';

      /* look for an end comment here */
      findEndComment = line.find( "*/", startComment );
      if ( findEndComment ==  string::npos )
      {
         cerr << " oh oh 1" << endl;
         
         endComment = len;
      }
      else
      {
         endComment = findEndComment;
         
         pattern[endComment]   = 'e';
         pattern[endComment+1] = 'e';
         cppComment = false;
            
         // We recursively called ourselves (at the end of this function)
         // until we've analysed the whole string.
         if ( endComment+2 < len )
            unfinished = endComment+2;
      }
      for ( i = startComment; i < endComment; ++i )
         pattern[i] = 'c';
      
   }
   else if ( findStartCppComment != string::npos )
   {
      // We have a C++ comment
      
      startText    = 0;
      endText      = findStartCppComment;
      startComment = findStartCppComment + 2 ;
      cppComment = true;
      
      for ( i = startText; i < endText; ++i )
         pattern[i] = 't';
      pattern[endText]   = 's';
      pattern[endText+1] = 's';

      /* look for an end comment here */
      findEndComment = line.find( '\n', startComment );
      if ( findEndComment ==  string::npos )
      {
         endComment = len;
      }
      else
      {
         endComment = findEndComment;

         pattern[endComment]   = 'n';

         // We recursively called ourselves (at the end of this function)
         // until we've analysed the whole string.
         if ( endComment+1 < len )
            unfinished = endComment+1;
      }

      for ( i = startComment; i < endComment; ++i )
         pattern[i] = '+';

   }
   else
   {
      // Just plain regular code.
      startText    = 0;
      endText      = len;
      startComment = 0;
      endComment   = 0;

      for ( i = startText; i < endText; ++i )
         pattern[i] = 't';
   }
      
   for ( i = startText; i < endText; ++i )
   {
      if ( line[i] == '\n' )
      {
         pattern[i] = 'n';
      }
   }
   
   // Strip doxygen tags and white space from comments.
   if ( startComment < endComment )
   {
      if ( cppComment )
      {
         if ( line[startComment] == '/' )
         {
            pattern[startComment] = 's';
            (startComment)++;
         }
      }
      else
      {
         if ( line[startComment] == '*' || 
              line[startComment] == '!' )
         {
            pattern[startComment] = 's';
            (startComment)++;
         }
      }
   }

   for ( i = startComment; i < endComment; ++i )
      if ( line[i] == '\n' )
      {
         pattern[i] = 'n';
      }
   
   // We recursively called ourselves until we've analysed the whole string.
   if ( unfinished != string::npos )
   {
      string tmp1( line,    unfinished, line.length()-unfinished );
      string tmp2( pattern, unfinished, line.length()-unfinished );
      CreatePattern( tmp1, tmp2 );
      pattern.replace( unfinished, tmp2.length(), tmp2 );
   }

   return;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

string::size_type analyser::IdentifyElements( 
   string::size_type  currentLocation,
   string::size_type  endSearch,
   string::size_type& startTokenLocation,
   string::size_type& endTokenLocation,
   string::size_type& startCommentLocation,
   string::size_type& endCommentLocation )
{
   string::size_type i;
   string::size_type nextToken = string::npos;
   string::size_type comma = string::npos;
   string::size_type startLoop, endLoop;
   
   startTokenLocation   = string::npos;
   endTokenLocation     = string::npos;
   startCommentLocation = string::npos;
   endCommentLocation   = string::npos;

   // Search for the next token ( not the current one). The location of
   // the next token is used to determine where the end of the comment 
   // might be (if the comments are after the token).
   
   comma = m_stripData.find( ',', currentLocation );
   if ( comma != string::npos )
   {
      for ( i = comma+1; i < endSearch; ++i )
      {
         if ( isspace(m_inputData[i]) )
            continue;
         if ( m_pattern[i] == 't' )
         {
            nextToken = i;
            break;
         }
      }
   }
   
   // Where is our token?
   endLoop = endSearch;
   if ( comma != string::npos ) endLoop = comma;
   for ( i = currentLocation; i < endLoop; ++i )
   {
      if ( isspace(m_inputData[i]) )
         continue;
      if ( m_pattern[i] == 't' )
      {
         startTokenLocation = i;
         break;
      }
   }
   if ( startTokenLocation == string::npos )
      throw m_formatErrorException;
   for ( i = endLoop-1; i > startTokenLocation; --i )
   {
      if ( isspace(m_inputData[i]) )
         continue;
      if ( m_pattern[i] == 't' )
      {
         endTokenLocation = i;
         break;
      }
   }
   
   if ( endTokenLocation == string::npos )
      throw m_formatErrorException;

   // Where is the beginning of that comment? 
   if ( m_commentsBeforeCode )
   {
      startLoop = currentLocation;
      endLoop   = startTokenLocation;
   }
   else
   {
      startLoop = endTokenLocation+1;
      endLoop = endSearch;
      if ( nextToken != string::npos ) endLoop = nextToken;
   }
   
   for ( i = startLoop; i < endLoop; ++i )
   {
      if ( m_pattern[i] == 's' )
      {
         startCommentLocation = i;
         break;
      }
   }

   // And where is the end of that comment (if any)?
   if ( startCommentLocation != string::npos )
   {
      if ( m_commentsBeforeCode )
         startLoop = startTokenLocation;
      else
      {
         startLoop = endSearch;
         if ( nextToken != string::npos ) startLoop = nextToken;
      }
      
      for ( i = startLoop-1; i > startCommentLocation; --i )
      {
         if ( m_pattern[i] == 'e' || m_pattern[i] == '+' )
         {
            endCommentLocation = i;
            break;
         }
      }
      if ( endCommentLocation == string::npos )
         throw m_formatErrorException;
   }

   return comma;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

bool analyser::AreCommentsBeforeCode( string::size_type currentLocation,
                                      string::size_type endSearch )
{
   string::size_type i;
   
   for ( i = currentLocation; i < endSearch; ++i )
   {
      if ( isspace(m_inputData[i]) )
         continue;
      if ( m_pattern[i] == 't' )
         return false;
      if ( m_pattern[i] == 's' )
         return true;

      fprintf( stderr, "Pattern error = %c \n", m_pattern[i] );
      break;
   }

   throw m_emptyEnumException;   
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void analyser::ParseEnum( string::size_type startSearch,
                          string::size_type endSearch )
{
   string::size_type currentLocation = startSearch;
   string::size_type nextLocation;
   string::size_type startToken;
   string::size_type endToken;
   string::size_type startComment;
   string::size_type endComment;
   string s;
   string enumName;
   
   for ( ;; )
   {
      nextLocation = IdentifyElements( currentLocation,
                                       endSearch,
                                       startToken,
                                       endToken,
                                       startComment,
                                       endComment );

      GetEnumValue( startToken, endToken, enumName );

      s = GetComment( startComment, endComment );
      if ( s.length() == 0 )
      {
         cerr << "Warning - no valid comments were found for error " << 
            m_enumNumericalValue << " (" << enumName << ")" << endl;
         cerr << "The following will be inserted instead: " <<
            "\"To be written...\"" << endl;
         s = "To be written...";
      }
      
      m_pOutput->AddEntry( m_enumNumericalValue, s, enumName );
      
      if ( nextLocation == string::npos )
         break;

      currentLocation = nextLocation+1;
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void analyser::StripComments()
{
   bool textMode = true;
   string::size_type i;
   
   m_stripData.resize(0);

   if ( m_pattern.length() == 0 ) return;

   // Determine if we start with a comment or code
   if ( m_pattern[0] == 's' )
      textMode = false;
      
   i = 0;
   while ( i < m_pattern.length() )
   {
      if ( textMode )
      {
         if ( m_pattern[i] == 't' || 
              m_pattern[i] == 'n' )
         {
            m_stripData += m_inputData[i];
         }
         else 
         {
            // Add spaces where comments used to be to make sure
            // tokens are nicely separated.
            m_stripData += ' ';
            if ( m_pattern[i] == 's' )
               textMode = false;
         }
      }
      else
      {
         if (  m_pattern[i] == 'n' )
            m_stripData += m_inputData[i];
         else
            m_stripData += ' ';

         // For C comments
         if ( m_pattern[i] == 'e' )
            textMode = true;
         // For C++ comments
         if (  m_pattern[i] == 'n' && i > 0 )
            if ( m_pattern[i-1] == '+' )
               textMode = true;
         
      }
      ++i;
   }
   
   return;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
