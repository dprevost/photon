/*
 * Copyright (C) 2006 Daniel Prevost <dprevost@users.sourceforge.net>
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

/*
 * This program will take an input header file containing error codes
 * (an enum with all the errors) and will use that information to
 * generate a new header file that can be used by the module
 * vdscErrorHandler.
 *
 * C/C++ might not be the best choice for such a tool but it is likely
 * the most portable way of doing it.
 *
 * Usage: errorParser inputfile outputfile
 *
 * The "design"...
 *
 * The input file will be scan until the keyword enum is found. It will
 * assume that everything inside the following pair of curly brackets ({}) 
 * as either an enum value or a comment.
 *
 * All the processed data will be put in an array of structs with 2 elements, 
 * an int where the enum value will be stored and a message of max. 256 chars 
 * (the comment).
 *
 */

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#include "Common.h"
#include <iostream>
#include "inputHeader.h"
#include "outputFiles.h"
#include "analyser.h"
#include "analyserTest.h"
#include "Options.h"

using namespace std;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int main( int argc, char* argv[] )
{
   int errcode = 0;
   inputHeader  input;
   outputFiles output;
   analyser  parser( &input, &output );
   vdscOptionHandle handle;
   bool optionOK;
   char* inputFilename = NULL;
   char* outputRoot = NULL;
   char* prefix = NULL;
   
   struct vdscOptStruct opts[3] = 
      { 'i', "input",  0, "INPUT_HEADER_FILE", "The input header file",
        'o', "output", 0, "OUTPUT_ROOT_NAME", "The root name use for output files",
        'p', "prefix", 0, "PREFIX", "Prefix used for names (output files)"
      };

   errcode = vdscSetSupportedOptions( 3, opts, &handle );
   if ( errcode != 0 )
   {
      cerr << "Unexpected error in vdscSetSupportedOptions" << endl;
      return -1;
   }
   
   if ( argc == 2 )
   {
      if ( strcmp( argv[1], "--test" ) == 0 )
      {
         analyserTest tester( parser );
         errcode = tester.Run();
         if ( errcode == 0 )
            cerr << "Internal tests succeeded" << endl;
         else
            cerr << "Internal tests failed" << endl;
         return errcode;
      }
   }
   
   errcode = vdscValidateUserOptions( handle, argc, argv, 1 );
   if ( errcode != 0 )
   {
      vdscShowUsage( handle, "errorParser", "" );
      if ( errcode == 1 )
         return 0;
      return -1;
   }

   optionOK = vdscGetShortOptArgument( handle, 'i', &inputFilename );
   if ( ! optionOK )
   {
      cerr << " Problem in vdscGetShortOptArgument" << endl;
      goto the_exit;
   }
   optionOK = vdscGetShortOptArgument( handle, 'o', &outputRoot );
   if ( ! optionOK )
   {
      cerr << " Problem in vdscGetShortOptArgument" << endl;
      goto the_exit;
   }
   optionOK = vdscGetShortOptArgument( handle, 'p', &prefix );
   if ( ! optionOK )
   {
      cerr << " Problem in vdscGetShortOptArgument" << endl;
      goto the_exit;
   }
   
   try
   {
      input.OpenFile( inputFilename );
   }
   catch( fileException& ex )
   {
      cerr << ex.what() << endl;
      goto the_exit;
   }   
   catch( exception& ex )
   {
      cerr << "Error opening input file, error = " << ex.what() << endl;
      goto the_exit;
   }
   catch(...)
   {
      cerr << "Error opening input file, unhandled exception!" << endl;
      throw;
   }
   
   try
   {
      output.OpenFiles( outputRoot, prefix );
   }
   catch( exception& ex )
   {
      cerr << "Error opening one of the output files, error = " 
           << ex.what() << endl;
      goto the_exit;
   }
   catch(...)
   {
      cerr << "Error opening one of the output files, unhandled exception!" 
           << endl;
      throw;
   }

   try
   {
      parser.ReadInput();
   }
   catch( exception& ex )
   {
      cerr << "Error reading the input data, error = " << ex.what() << endl;
      goto the_exit;
   }
   catch(...)
   {
      cerr << "Error reading the input data, unhandled exception!" << endl;
      throw;
   }
   
   try
   {
      parser.ProcessInput();
   }
   catch( analyseException& ex )
   {
      cerr << "Error analysing the input data, error = " << ex.what() << endl;
      goto the_exit;
   }
   catch(...)
   {
      cerr << "Error analysing the input data, unhandled exception!" << endl;
      throw;
   }
   
   try
   {
      output.Write( inputFilename, prefix );
   }
   catch(...)
   {
      cerr << "Error writing the output, unhandled exception!" << endl;
      throw;
   }
   
   
 the_exit:

   if ( errcode != 0 )
      fprintf( stderr, "Exiting with an error\n" );

   return errcode;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
