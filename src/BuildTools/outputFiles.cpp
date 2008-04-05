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

#include "outputFiles.h"

using namespace std;

const char* outputFiles::m_programName = "errorParser";

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

outputFiles::outputFiles() throw()
   : m_longestString ( 0 )
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

outputFiles::~outputFiles() throw()
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void outputFiles::AddEntry( int          value, 
                            std::string& comment, 
                            std::string& enumName )
{
   entry e;
   
   e.value = value;
   e.comment = comment;
   if ( e.comment.length() > m_longestString ) {
      m_longestString = e.comment.length();
   }
   e.enumName = enumName;
   
   m_list.push_back( e );
   
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void outputFiles::OpenFiles( const char* filenamePrefix, 
                             const char* dataPrefix )
{
   string s;
   string::size_type offset;
   
   s = filenamePrefix;
   s += ".h";
   m_stream_h.open( s.c_str(), std::ios::trunc | std::ios::out );
   if ( ! m_stream_h.is_open() ) {
      throw m_exception;
   }
   
   s = filenamePrefix;
   s += ".c";
   m_stream_c.open( s.c_str(), std::ios::trunc | std::ios::out );
   if ( ! m_stream_c.is_open() ) {
      throw m_exception;
   }
   
   // As long as we have the name of the output header file, use it to
   // generate the standard "#ifndef XXX \n#define XXX ... #endif".
   m_ifdefname  = "";
   m_headerName = "";
   
   // Remove the directory name if present.
   s = filenamePrefix;
   offset = s.rfind( '/');
   if ( offset == string::npos ) offset = s.rfind( '\\'); // Win32
   if ( offset == string::npos ) {
      offset = 0;
   }
   else {
      offset++;
   }
   
   for ( unsigned int i = offset; i < strlen( filenamePrefix ); ++i ) {
      if ( isalnum(filenamePrefix[i]) ) {
         m_ifdefname += toupper(filenamePrefix[i]);
      }
      else {
         m_ifdefname += '_';
      }
      m_headerName += filenamePrefix[i];
   }
   m_ifdefname  += "_H";
   m_headerName += ".h";
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void outputFiles::Write( const char* inFilename, const char* prefix )
{
   size_t num = m_list.size();
   string upperPrefix;
   string stringPrefix = prefix;
   size_t i;
   
   WriteHeader( m_stream_h, inFilename );
   WriteHeader( m_stream_c, inFilename );
   
   for ( string::size_type j = 0; j < stringPrefix.length(); ++j ) {
      upperPrefix += toupper(stringPrefix[j]);
   }
   
   //
   // Write a few basic things to the generated header file
   //
   m_stream_h << "#ifndef " << m_ifdefname << endl;
   m_stream_h << "#define " << m_ifdefname << endl << endl;
   
   m_stream_h << "/* Just in case we get included by C++ code */" << endl;
   m_stream_h << "#ifdef __cplusplus" << endl;
   m_stream_h << "extern \"C\" {" << endl;
   m_stream_h << "#endif" << endl << endl;
   
   m_stream_h << "#define " << upperPrefix << "_MAX_ENTRIES  " << num << endl;
   m_stream_h << "#define " << upperPrefix << "_MAX_COMMENTS " << 
      m_longestString+1 << endl;
   m_stream_h << endl;

   m_stream_h << "/*" << endl;
   m_stream_h << " * Use this function to access the error messages " <<
      "(generated from the " << endl;
   m_stream_h << " * comments in the input header file)." << endl;
   m_stream_h << " * " << endl;
   m_stream_h << " * Parameters:" << endl;
   m_stream_h << " *   - errnum    The error number" << endl;
   m_stream_h << " *   - msg       User-supplied buffer where the message" <<
      " will be copied" << endl;
   m_stream_h << " *   - msgLength The length of the message." << endl;
   m_stream_h << " *" << endl;
   m_stream_h << " * [Tip: you can safely use " << upperPrefix << 
      "_MAX_COMMENTS to define the length of " << endl;
   m_stream_h << " * your buffer.]" << endl;
   m_stream_h << " *" << endl;
   m_stream_h << " * Return values:" << endl;
   m_stream_h << " *    0  if errnum is valid (exists)" << endl;
   m_stream_h << " *   -1  otherwise" << endl;
   m_stream_h << " */" << endl << endl;
   
   m_stream_h << "int " << stringPrefix << 
      "GetErrMessage( int errnum, char *msg, unsigned int msgLength );" << 
      endl;

   m_stream_h << endl<< "#ifdef __cplusplus" << endl;
   m_stream_h << "} /* End of extern C */" << endl;
   m_stream_h << "#endif" << endl << endl;

   m_stream_h << "#endif /* " << m_ifdefname << " */" << endl;

   //
   m_stream_c << "#include \"Common/Common.h\"" << endl;
   m_stream_c << "#include \"" << m_headerName << "\"" << endl << endl;
   
   m_stream_c << "struct " << stringPrefix << "ErrMsgStruct" << endl;
   m_stream_c << "{" << endl;
   m_stream_c << "   int  errorNumber;" << endl;
   m_stream_c << "   int  messageLength;" << endl;
   m_stream_c << "   char *message;" << endl;
//    m_stream_c << "   char message[" << upperPrefix << 
//       "_MAX_COMMENTS];" << endl;
   m_stream_c << "};" << endl << endl;
   m_stream_c << "typedef struct " << stringPrefix << "ErrMsgStruct " << 
      stringPrefix << "ErrMsgStruct;" << endl;
   m_stream_c << endl;
   
//    m_stream_c << stringPrefix << "ErrMsgStruct " << stringPrefix << 
//       "ErrMsg[" << upperPrefix << "_MAX_ENTRIES] = ";
//    m_stream_c << endl << "{" << endl;
   
//    entry e = m_list.front();
//    m_stream_c << "   {" << e.value << ", " << e.comment.length() <<
//       ", \"" << e.comment << "\"}";
//    m_list.pop_front();

//    while ( ! m_list.empty() )
//    {
//       m_stream_c << "," << endl;
//       entry e = m_list.front();
//       m_stream_c << "   {" << e.value << ", " << e.comment.length() << 
//          ", \"" << e.comment << "\"}";

//       m_list.pop_front();
//    }
   
//    m_stream_c << endl << "};" << endl;
   
   for ( i = 0; i < num; ++i ) {
      entry e = m_list.front();
      m_stream_c << "/* " << e.enumName << " */" << endl;
      m_stream_c << stringPrefix << "ErrMsgStruct " << stringPrefix << 
         "ErrMsg" << i << " =" << endl;
      m_stream_c << "   { " << e.value << ", " << e.comment.length() <<
         ", \"" << e.comment << "\" };" << endl;

      m_list.pop_front();
      m_stream_c << endl;
   }
   
//    while ( ! m_list.empty() )
//    {
//       i++;
//       entry e = m_list.front();
//       m_stream_c << stringPrefix << "ErrMsgStruct " << stringPrefix << 
//          "ErrMsg" << i << " =" << endl;
//       m_stream_c << "   { " << e.value << ", " << e.comment.length() <<
//          ", \"" << e.comment << "\" };" << endl;
//       m_list.pop_front();
//    }
   
   m_stream_c << stringPrefix << "ErrMsgStruct* " << stringPrefix << 
      "ErrMsg[" << upperPrefix << "_MAX_ENTRIES] = ";
   m_stream_c << endl << "{" << endl;

   for ( i = 0; i < num-1; ++i ) {
      m_stream_c << "   &" << stringPrefix << "ErrMsg" << i << "," << endl;
   }
   
   m_stream_c << "   &" << stringPrefix << "ErrMsg" << num-1 << endl;
   m_stream_c << endl << "};" << endl << endl;

   m_stream_c << "int " << stringPrefix << 
      "GetErrMessage( int errnum, char *msg, unsigned int msgLength )" << endl;
   m_stream_c << "{" << endl;
   m_stream_c << "   int i;" << endl << endl;
   m_stream_c << "   if ( msgLength > 0 ) msg[0] = '\\0';" << endl;
   m_stream_c << "   for ( i = 0; i < " << upperPrefix << 
      "_MAX_ENTRIES; ++i )" << endl;
   m_stream_c << "   {" << endl;
   m_stream_c << "      if ( errnum == " << stringPrefix << 
      "ErrMsg[i]->errorNumber )" << endl;
   m_stream_c << "      {" << endl;
   m_stream_c << "         if ( msgLength > 0 && " << stringPrefix << 
      "ErrMsg[i]->messageLength > 0 )" << endl;
   m_stream_c << "         {" << endl;
   m_stream_c << "            strncpy( msg, " << stringPrefix << 
      "ErrMsg[i]->message, msgLength-1 );" << endl;
   m_stream_c << "            msg[msgLength-1] = '\\0';" << endl;
   m_stream_c << "         }" << endl;
   m_stream_c << "         return 0;" << endl;
   m_stream_c << "      }" << endl;
   m_stream_c << "   }" << endl << endl;
   m_stream_c << "   return -1;" << endl;
   m_stream_c << "}" << endl;
}


// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void outputFiles::WriteHeader( ofstream& stream, const char* inFilename )
{
   stream << "/*" << endl;
   stream << " * This file was generated by the program ";
   stream << m_programName << endl;
   stream << " * using the input file " << inFilename << "." << endl;
   stream << " * (The code generator is copyrighted by Daniel ";
   stream << "Prevost " << endl;
   stream << " * <dprevost@users.sourceforge.net>)" << endl;
   stream << " *" << endl;

   stream << " * This file is free software; as a special exception";
   stream << " the author gives" << endl;
   stream << " * unlimited permission to copy and/or distribute it,";
   stream << " with or without" << endl;
   stream << " * modifications, as long as this notice is preserved.";
   stream << endl << " *" << endl;
   stream << " * This program is distributed in the hope that it will";
   stream << " be useful, but" << endl;
   stream << " * WITHOUT ANY WARRANTY, to the extent permitted by law;";
   stream << " without even the" << endl;
   stream << " * implied warranty of MERCHANTABILITY or FITNESS FOR A";
   stream << " PARTICULAR PURPOSE." << endl;
   stream << " */" << endl << endl;   
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

