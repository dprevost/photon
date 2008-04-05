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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "analyserTest.h"

using namespace std;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

analyserTest::analyserTest( analyser& a )
   : m_a ( a )
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

analyserTest::~analyserTest()
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int analyserTest::TestCreatePattern()
{
   string pattern;
   string line;
   string expectedPattern;   

   pattern.resize( line.length() );
   m_a.CreatePattern( line, pattern );
   if ( pattern != expectedPattern ) return 1;

   line             = "  abc  /*    some text here  \n";
   expectedPattern  = "tttttttssccccccccccccccccccccn";
   line            += "  which is continued on the next  \n";
   expectedPattern += "ccccccccccccccccccccccccccccccccccn";
   line            += "  line      */    ";
   expectedPattern += "cccccccccccceetttt";
   pattern.resize( line.length() );
   m_a.CreatePattern( line, pattern );
   if ( pattern != expectedPattern ) return 2;

   line             = "  abc  /*    some text here  \n";
   expectedPattern  = "tttttttssccccccccccccccccccccn";
   line            += "  which is continued on the next  \n";
   expectedPattern += "ccccccccccccccccccccccccccccccccccn";
   line            += "  line      */    www ";
   expectedPattern += "cccccccccccceetttttttt";
   pattern.resize( line.length() );
   m_a.CreatePattern( line, pattern );
   if ( pattern != expectedPattern ) return 3;
 
   line            = "Z/*\n  */";
   expectedPattern = "tssnccee";
   pattern.resize( line.length() );
   m_a.CreatePattern( line, pattern );
   if ( pattern != expectedPattern ) return 4;
 
   line             = "   /**    some text here  \n";
   expectedPattern  = "tttsssccccccccccccccccccccn";
   line            += "  which is continued // on the next  \n";
   expectedPattern += "cccccccccccccccccccccccccccccccccccccn";
   line            += "  line      */hi there    ";
   expectedPattern += "cccccccccccceetttttttttttt";
   pattern.resize( line.length() );
   m_a.CreatePattern( line, pattern );
//    cerr << pattern << endl;
//    cerr << expectedPattern << endl;
   if ( pattern != expectedPattern ) return 5;
 
   line            = "/*! How about this */";
   expectedPattern = "sssccccccccccccccccee";
   pattern.resize( line.length() );
   m_a.CreatePattern( line, pattern );
   if ( pattern != expectedPattern ) return 6;
 
   line            = "/* How about this #2 */ azerty /// querty ";
   expectedPattern = "ssccccccccccccccccccceettttttttsss++++++++";
   pattern.resize( line.length() );
   m_a.CreatePattern( line, pattern );
   if ( pattern != expectedPattern ) return 7;
 
   line            = "/* Will it see // this */ ";
   expectedPattern = "ssccccccccccccccccccccceet";
   pattern.resize( line.length() );
   m_a.CreatePattern( line, pattern );
   if ( pattern != expectedPattern ) return 8;
 
   line            = "// Will it see /* this */ (well both) ";
   expectedPattern = "ss++++++++++++++++++++++++++++++++++++";
   pattern.resize( line.length() );
   m_a.CreatePattern( line, pattern );
   if ( pattern != expectedPattern ) return 9;
   
   line             = " First  /*  comment 1  */  Second/* comment   2 ";
   expectedPattern  = "ttttttttssccccccccccccceettttttttssccccccccccccc";
   line            += "*/Third // Comment 3 ";
   expectedPattern += "eettttttss+++++++++++";
   pattern.resize( line.length() );
   m_a.CreatePattern( line, pattern );
   if ( pattern != expectedPattern ) return 10;
 
   line             = " First  /*  comment 1  *//* comment   2 */";
   expectedPattern  = "ttttttttssccccccccccccceesscccccccccccccee";
   line            += "Third // Comment 3 ";
   expectedPattern += "ttttttss+++++++++++";
   pattern.resize( line.length() );
   m_a.CreatePattern( line, pattern );
   if ( pattern != expectedPattern ) return 11;
 
   line             = " First  /*  comment 1  *//* comment   2 */";
   expectedPattern  = "ttttttttssccccccccccccceesscccccccccccccee";
   line            += "Third /* Comment 3 \n";
   expectedPattern += "ttttttsscccccccccccn";
   line            += " comment 3, la suite */ Four///Comment 4 ";
   expectedPattern += "ccccccccccccccccccccceetttttsss++++++++++";
   pattern.resize( line.length() );
   m_a.CreatePattern( line, pattern );
   if ( pattern != expectedPattern ) return 12;
 
   line             = " First // blah blah\n Blah Blah\n//\n // \nz/* zut */";
   expectedPattern  = "tttttttss++++++++++nttttttttttnssntss+ntsscccccee";
   pattern.resize( line.length() );
   m_a.CreatePattern( line, pattern );
   if ( pattern != expectedPattern ) return 13;

   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int analyserTest::TestStripComments()
{
   string expectedStrip;

   m_a.m_inputData   = "";
   m_a.m_pattern     = "";
   expectedStrip = "";
   m_a.StripComments();
   if ( m_a.m_stripData != expectedStrip ) return 1;

   m_a.m_inputData      = "  abc  /*    some text here  \n";
   m_a.m_pattern        = "tttttttssccccccccccccccccccccn";
   m_a.m_inputData     += "  which is continued on the next  \n";
   m_a.m_pattern       += "ccccccccccccccccccccccccccccccccccn";
   m_a.m_inputData     += "  line      */    ";
   m_a.m_pattern       += "cccccccccccceetttt";
   expectedStrip    = "  abc                        \n";
   expectedStrip   += "                                  \n";
   expectedStrip   += "                  ";
   m_a.StripComments();
   if ( m_a.m_stripData != expectedStrip ) return 2;

   m_a.m_inputData    = "  abc  /*    some text here  \n";
   m_a.m_pattern      = "tttttttssccccccccccccccccccccn";
   m_a.m_inputData   += "  which is continued on the next  \n";
   m_a.m_pattern     += "ccccccccccccccccccccccccccccccccccn";
   m_a.m_inputData   += "  line      */    www ";
   m_a.m_pattern     += "cccccccccccceetttttttt";
   expectedStrip  = "  abc                        \n";
   expectedStrip += "                                  \n";
   expectedStrip += "                  www ";
   m_a.StripComments();
   if ( m_a.m_stripData != expectedStrip ) return 3;
 
   m_a.m_inputData   = "Z/*\n  */";
   m_a.m_pattern     = "tssnccee";
   expectedStrip = "Z  \n    ";
   m_a.StripComments();
   if ( m_a.m_stripData != expectedStrip ) return 4;
 
   m_a.m_inputData    = "   /**    some text here  \n";
   m_a.m_pattern      = "tttsssccccccccccccccccccccn";
   m_a.m_inputData   += "  which is continued // on the next  \n";
   m_a.m_pattern     += "cccccccccccccccccccccccccccccccccccccn";
   m_a.m_inputData   += "  line      */hi there    ";
   m_a.m_pattern     += "cccccccccccceetttttttttttt";
   expectedStrip  = "                          \n";
   expectedStrip += "                                     \n";
   expectedStrip += "              hi there    ";
   m_a.StripComments();
//    cerr << pattern << endl;
//    cerr << expectedStrip << endl;
   if ( m_a.m_stripData != expectedStrip ) return 5;
 
   m_a.m_inputData   = "/*! How about this */";
   m_a.m_pattern     = "sssccccccccccccccccee";
   expectedStrip = "                     ";
   m_a.StripComments();
   if ( m_a.m_stripData != expectedStrip ) return 6;
 
   m_a.m_inputData   = "/* How about this #2 */ azerty /// querty ";
   m_a.m_pattern     = "ssccccccccccccccccccceettttttttsss++++++++";
   expectedStrip = "                        azerty            ";
   m_a.StripComments();
   if ( m_a.m_stripData != expectedStrip ) return 7;
 
   m_a.m_inputData   = "/* Will it see // this */ ";
   m_a.m_pattern     = "ssccccccccccccccccccccceet";
   expectedStrip = "                          ";
   m_a.StripComments();
   if ( m_a.m_stripData != expectedStrip ) return 8;
 
   m_a.m_inputData   = "// Will it see /* this */ (well both) ";
   m_a.m_pattern     = "ss++++++++++++++++++++++++++++++++++++";
   expectedStrip = "                                      ";
   m_a.StripComments();
   if ( m_a.m_stripData != expectedStrip ) return 9;
   
   m_a.m_inputData    = " First  /*  comment 1  */  Second/* comment   2 ";
   m_a.m_pattern      = "ttttttttssccccccccccccceettttttttssccccccccccccc";
   m_a.m_inputData   += "*/Third // Comment 3 ";
   m_a.m_pattern     += "eettttttss+++++++++++";
   expectedStrip  = " First                     Second               ";
   expectedStrip += "  Third              ";
   m_a.StripComments();
   if ( m_a.m_stripData != expectedStrip ) return 10;
 
   m_a.m_inputData    = " First  /*  comment 1  *//* comment   2 */";
   m_a.m_pattern      = "ttttttttssccccccccccccceesscccccccccccccee";
   m_a.m_inputData   += "Third // Comment 3 ";
   m_a.m_pattern     += "ttttttss+++++++++++";
   expectedStrip  = " First                                    ";
   expectedStrip += "Third              ";
   m_a.StripComments();
   if ( m_a.m_stripData != expectedStrip ) return 11;
 
   m_a.m_inputData    = " First  /*  comment 1  *//* comment   2 */";
   m_a.m_pattern      = "ttttttttssccccccccccccceesscccccccccccccee";
   m_a.m_inputData   += "Third /* Comment 3 \n";
   m_a.m_pattern     += "ttttttsscccccccccccn";
   m_a.m_inputData   += " comment 3, la suite */ Four///Comment 4 ";
   m_a.m_pattern     += "ccccccccccccccccccccceetttttsss++++++++++";
   expectedStrip  = " First                                    ";
   expectedStrip += "Third              \n";
   expectedStrip += "                        Four             ";
   m_a.StripComments();
   if ( m_a.m_stripData != expectedStrip ) return 12;
 
   m_a.m_inputData   = " First // blah blah\n Blah Blah\n//\n // \nz/* zut */";
   m_a.m_pattern     = "tttttttss++++++++++nttttttttttnssntss+ntsscccccee";
   expectedStrip = " First             \n Blah Blah\n  \n    \nz         ";
   m_a.StripComments();
   if ( m_a.m_stripData != expectedStrip ) return 13;

   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int analyserTest::TestGetToken()
{
   string token;
   string expectedToken;
   string::size_type location;
   string::size_type expectedLocation;
   
   m_a.m_stripData = " token ";
   expectedToken = "token";
   expectedLocation = 1;
   location = m_a.GetNextToken( 0, token );
   if ( location != expectedLocation ) return 1;
   if ( token != expectedToken ) return 2;
   
   m_a.m_stripData = "\n\ntoken aaa";
   expectedToken = "token";
   expectedLocation = 2;
   location = m_a.GetNextToken( 0, token );
   if ( location != expectedLocation ) return 3;
   if ( token != expectedToken ) return 4;

   m_a.m_stripData = "\n\n token\n aaa";
   expectedToken = "token";
   expectedLocation = 3;
   location = m_a.GetNextToken( 0, token );
   if ( location != expectedLocation ) return 5;
   if ( token != expectedToken ) return 6;

   m_a.m_stripData = "\t\t token\t aaa";
   expectedToken = "token";
   expectedLocation = 3;
   location = m_a.GetNextToken( 0, token );
   if ( location != expectedLocation ) return 7;
   if ( token != expectedToken ) return 8;

   m_a.m_stripData = "\t\t token\t aaa     \n  \n";
   expectedToken = "";
   expectedLocation = string::npos;
   location = m_a.GetNextToken( 15, token );
   if ( location != expectedLocation ) return 9;
   if ( token != expectedToken ) return 10;

   m_a.m_stripData = "\t\t token\t aaa     \n  \nZ";
   expectedToken = "";
   expectedLocation = string::npos;
   location = m_a.GetNextToken( m_a.m_stripData.length(), token );
   if ( location != expectedLocation ) return 11;
   if ( token != expectedToken ) return 12;

   m_a.m_stripData = "\t\t token\t aaa     \n  \nZ";
   expectedToken = "";
   expectedLocation = string::npos;
   location = m_a.GetNextToken( m_a.m_stripData.length()+1, token );
   if ( location != expectedLocation ) return 13;
   if ( token != expectedToken ) return 14;

   m_a.m_stripData = "\n\n token\n aaa";
   expectedToken = "aaa";
   expectedLocation = 10;
   location = m_a.GetNextToken( 8, token );
   if ( location != expectedLocation ) return 15;
   if ( token != expectedToken ) return 16;

   return 0;
}


// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int analyserTest::TestGetEnum()
{
   string::size_type location;
   bool coughtIt;

   m_a.m_stripData = "enum";
   try {
      location = m_a.GetEnumKeyword();
   }
   catch ( noEnumException e ) {
      return 1;
   }
   if ( location != 0 ) return 2;
   
   m_a.m_stripData = "  enum";
   try {
      location = m_a.GetEnumKeyword();
   }
   catch ( noEnumException e ) {
      return 3;
   }
   if ( location != 2 ) return 4;

   m_a.m_stripData = "  \nenum";
   try {
      location = m_a.GetEnumKeyword();
   }
   catch ( noEnumException e ) {
      return 5;
   }
   if ( location != 3 ) return 6;

   m_a.m_stripData = "  \nenum\n";
   try {
      location = m_a.GetEnumKeyword();
   }
   catch ( noEnumException e ) {
      return 7;
   }
   if ( location != 3 ) return 8;

   m_a.m_stripData = "  \nenum\n1 \n";
   try {
      location = m_a.GetEnumKeyword();
   }
   catch ( noEnumException e ) {
      return 9;
   }
   if ( location != 3 ) return 10;

   coughtIt = false;
   m_a.m_stripData = "";
   try {
      location = m_a.GetEnumKeyword();
   }
   catch ( noEnumException e ) {
      coughtIt = true;
   }
   if ( ! coughtIt ) return 11;
   
   coughtIt = false;
   m_a.m_stripData = " enum1";
   try {
      location = m_a.GetEnumKeyword();
   }
   catch ( noEnumException e ) {
      coughtIt = true;
   }
   if ( ! coughtIt ) return 12;
   
   coughtIt = false;
   m_a.m_stripData = " myenum ";
   try {
      location = m_a.GetEnumKeyword();
   }
   catch ( noEnumException e ) {
      coughtIt = true;
   }
   if ( ! coughtIt ) return 13;   

   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int analyserTest::TestProcessInput()
{
   bool coughtIt;
   coughtIt = false;

   m_a.m_inputData = "  \n  \n";
   m_a.m_stripData.reserve( m_a.m_inputData.length() );
   m_a.m_pattern.resize   ( m_a.m_inputData.length() );
   try {
      m_a.ProcessInput();
   }
   catch ( noEnumException e ) {
      coughtIt = true;
   }
   catch(...) {}   
   if ( ! coughtIt ) return 1;

   coughtIt = false;
   m_a.m_inputData = " enum \n \n \n";
   m_a.m_stripData.reserve( m_a.m_inputData.length() );
   m_a.m_pattern.resize   ( m_a.m_inputData.length() );
   try {
      m_a.ProcessInput();
   }
   catch ( noNameException e ) {
      coughtIt = true;
   }
   catch(...) {}
   if ( ! coughtIt ) return 2;
   
   coughtIt = false;
   m_a.m_inputData = " enum      my_errors\n";
   m_a.m_stripData.reserve( m_a.m_inputData.length() );
   m_a.m_pattern.resize   ( m_a.m_inputData.length() );
   try {
      m_a.ProcessInput();
   }
   catch ( noOpenCurlyException e ) {
      coughtIt = true;
   }
   catch(...) {}
   if ( ! coughtIt ) return 3;

   coughtIt = false;
   m_a.m_inputData = " enum \n \n my_errors \n";
   m_a.m_stripData.reserve( m_a.m_inputData.length() );
   m_a.m_pattern.resize   ( m_a.m_inputData.length() );
   try {
      m_a.ProcessInput();
   }
   catch ( noOpenCurlyException e ) {
      coughtIt = true;
   }
   catch(...) {}
   if ( ! coughtIt ) return 4;
   
   coughtIt = false;
   m_a.m_inputData = " enum \n \n my_errors \n \n \n \n{\n\n";
   m_a.m_stripData.reserve( m_a.m_inputData.length() );
   m_a.m_pattern.resize   ( m_a.m_inputData.length() );
   try {
      m_a.ProcessInput();
   }
   catch ( noCloseCurlyException e ) {
      coughtIt = true;
   }
   catch(...) {}
   if ( ! coughtIt ) return 5;

   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int analyserTest::TestIdentifyElements()
{
   bool coughtIt;
   string::size_type startTokenLocation;
   string::size_type endTokenLocation;
   string::size_type startCommentLocation;
   string::size_type endCommentLocation;   
   string::size_type next;   

   m_a.m_commentsBeforeCode = true;
   
   m_a.m_inputData = "  /* */ EEE = 4  \n";
   m_a.m_stripData = "        EEE = 4  \n";
   m_a.m_pattern   = "ttssceettttttttttn";
   try {
      next = m_a.IdentifyElements( 0, 
                                   m_a.m_inputData.length(), 
                                   startTokenLocation, 
                                   endTokenLocation, 
                                   startCommentLocation, 
                                   endCommentLocation );
   }
   catch(...) {
      return 1;
   }
   if ( startTokenLocation   != 8 )  return 2;
   if ( endTokenLocation     != 14 ) return 3;
   if ( startCommentLocation != 2 )  return 4;
   if ( endCommentLocation   != 6 )  return 5;
   if ( next != string::npos ) return 6;

   //

   m_a.m_inputData = "  /* */ EEE = 4, \n";
   m_a.m_stripData = "        EEE = 4, \n";
   m_a.m_pattern   = "ttssceettttttttttn";
   try {
      next = m_a.IdentifyElements( 0, 
                                   m_a.m_inputData.length(), 
                                   startTokenLocation, 
                                   endTokenLocation, 
                                   startCommentLocation, 
                                   endCommentLocation );
   }
   catch(...) {
      return 11;
   }
   if ( startTokenLocation   != 8 )  return 12;
   if ( endTokenLocation     != 14 ) return 13;
   if ( startCommentLocation != 2 )  return 14;
   if ( endCommentLocation   != 6 )  return 15;
   if ( next != 15 ) return 16;

   //

   m_a.m_commentsBeforeCode = false;
   m_a.m_inputData = "  /* */ EEE = 4, \n";
   m_a.m_stripData = "        EEE = 4, \n";
   m_a.m_pattern   = "ttssceettttttttttn";
   try {
      next = m_a.IdentifyElements( 0, 
                                   m_a.m_inputData.length(), 
                                   startTokenLocation, 
                                   endTokenLocation, 
                                   startCommentLocation,
                                   endCommentLocation );
   }
   catch(...) {
      return 21;
   }
   if ( startTokenLocation   != 8 )  return 22;
   if ( endTokenLocation     != 14 ) return 23;
   if ( startCommentLocation != string::npos )  return 24;
   if ( endCommentLocation   != string::npos )  return 25;
   if ( next != 15 ) return 26;

   //

   m_a.m_inputData = "   EEE = 4, // A comment \n FFF, ";
   m_a.m_stripData = "   EEE = 4,              \n FFF, ";
   m_a.m_pattern   = "ttttttttttttss+++++++++++ntttttt";
   try {
      next = m_a.IdentifyElements( 0, 
                                   m_a.m_inputData.length(), 
                                   startTokenLocation, 
                                   endTokenLocation, 
                                   startCommentLocation, 
                                   endCommentLocation );
   }
   catch(...) {
      return 31;
   }
//   cerr << startCommentLocation << endl;
   
   if ( startTokenLocation   != 3 )  return 32;
   if ( endTokenLocation     != 9 )  return 33;
   if ( startCommentLocation != 12 ) return 34;
   if ( endCommentLocation   != 24 ) return 35;
   if ( next != 10 ) return 36;

   //

   coughtIt = false;
   m_a.m_inputData = "  ";
   m_a.m_stripData = "  ";
   m_a.m_pattern   = "tt";
   try {
      next = m_a.IdentifyElements( 0, 
                                   m_a.m_inputData.length(), 
                                   startTokenLocation, 
                                   endTokenLocation, 
                                   startCommentLocation, 
                                   endCommentLocation );
   }
   catch(formatErrorException) {
      coughtIt = true;
   }
   catch(...) {
      return 41;
   }
   if ( ! coughtIt ) return 42;

   //

   m_a.m_inputData = "   EEE = 4 /* */, /* */\n";
   m_a.m_stripData = "   EEE = 4      ,      \n";
   m_a.m_pattern   = "tttttttttttssceettssceen";
   try {
      next = m_a.IdentifyElements( 0, 
                                   m_a.m_inputData.length(), 
                                   startTokenLocation, 
                                   endTokenLocation, 
                                   startCommentLocation, 
                                   endCommentLocation );
   }
   catch(...) {
      return 51;
   }
   
   if ( startTokenLocation   != 3 )  return 52;
   if ( endTokenLocation     != 9 )  return 53;
   if ( startCommentLocation != 11 ) return 54;
   if ( endCommentLocation   != 22 ) return 55;
   if ( next != 16 ) return 56;
   
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int analyserTest::Run()
{
   int errcode;
   
   errcode = TestCreatePattern();
   if ( errcode != 0 ) {
      fprintf( stderr, "Failed in TestCreatePattern(), test # %d\n", errcode );
      return -1;
   }

   errcode = TestStripComments();
   if ( errcode != 0 ) {
      fprintf( stderr, "Failed in StripComments(), test # %d\n", errcode );
      return -1;
   }

   errcode = TestGetToken();
   if ( errcode != 0 ) {
      fprintf( stderr, "Failed in GetNextToken(), test # %d\n", errcode );
      return -1;
   }
   
   errcode = TestGetEnum();
   if ( errcode != 0 ) {
      fprintf( stderr, "Failed in GetEnumKeyword(), test # %d\n", errcode );
      return -1;
   }

   errcode = TestProcessInput();
   if ( errcode != 0 ) {
      fprintf( stderr, "Failed in ProcessInput(), test # %d\n", errcode );
      return -1;
   }

   errcode = TestIdentifyElements();
   if ( errcode != 0 ) {
      fprintf( stderr, "Failed in IdentifyElements(), test # %d\n", errcode );
      return -1;
   }
   
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
