/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of Photon (photonsoftware.org).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation and appearing in the file COPYING included in the
 * packaging of this library.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#include "Tools/Shell/shell.h"
#include "Nucleus/Engine.h" // For PSON_BLOCK_SIZE
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include "API/DataDefinition.h" // for psoaGetOffsets
#include "Tools/Shell/util.h"

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

psoShell::psoShell(psoSession & s)
   : currentLocation( "/" ),
     session        ( s )
{
}  

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

psoShell::~psoShell()
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

bool psoShell::Dispatch()
{
   bool timeToExit = false;
   string s;
   
   if ( tokens[0] == s.assign("cat") ) {
      if ( tokens.size() != 2 ) throw(1);
      Cat();
   }
   else if ( tokens[0] == s.assign("cd") ) {
      if ( tokens.size() > 2 ) throw(1);
      Cd();
   }
   else if ( tokens[0] == s.assign("cp") ) {
      if ( tokens.size() != 3 ) throw(1);
      Cp();
   }
   else if ( tokens[0] == s.assign("del") ) {
      if ( tokens.size() != 2 ) throw(1);
      Rm();
   }
   else if ( tokens[0] == s.assign("dir") ) {
      if ( tokens.size() > 2 ) throw(1);
      Ls();
   }
   else if ( tokens[0] == s.assign("echo") ) {
      cerr << tokens[2] << " - " << tokens.size() << endl;
      if ( tokens.size() != 4 ) throw(1);
      if ( tokens[2] != ">" ) throw(2);
      Echo();
   }
   else if ( tokens[0] == s.assign("exit") ) {
      timeToExit = true;
   }
   else if ( tokens[0] == s.assign("free") ) {
      Free();
   }
   else if ( tokens[0] == s.assign("help") ) {
      Man();
   }
   else if ( tokens[0] == s.assign("ls") ) {
      Ls();
   }
   else if ( tokens[0] == s.assign("man") ) {
      Man();
   }
   else if ( tokens[0] == s.assign("mkdir") ) {
      if ( tokens.size() != 2 ) throw(1);
      Mkdir();
   }
   else if ( tokens[0] == s.assign("pwd") ) {
      cerr << currentLocation << endl;
   }
   else if ( tokens[0] == s.assign("quit") ) {
      timeToExit = true;
   }
   else if ( tokens[0] == s.assign("rm") ) {
      if ( tokens.size() != 2 ) throw(1);
      Rm();
   }
   else if ( tokens[0] == s.assign("rmdir") ) {
      if ( tokens.size() != 2 ) throw(1);
      Rmdir();
   }
   else if ( tokens[0] == s.assign("stat") ) {
      if ( tokens.size() != 2 ) throw(1);
      Stat();
   }
   else if ( tokens[0] == s.assign("touch") ) {
      if ( tokens.size() != 3 ) throw(1);
      Touch();
   }
   else {
      throw(0);
   }
   
   return timeToExit;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
   
void psoShell::DisplayData( psoObjectDefinition * pDefinition,
                            size_t              * offsets,
                            unsigned char       * buffer,
                            size_t                length )
{
   size_t i, j;
//   vector<string> segments;
   string s;
   
   for ( i = 0; i < pDefinition->numFields; ++i ) {
//      ostringstream oss;
      switch( pDefinition->fields[i].type ) {

      case PSO_BINARY:
         cout << "0x" << hex;
         for ( j = 0; j < pDefinition->fields[i].length; ++j )
            cout << (int) buffer[offsets[i]+j];
         cout << dec;
         break;
      case PSO_STRING:
         for ( j = 0; j < pDefinition->fields[i].length; ++j )
            cout << (char) buffer[offsets[i]+j];
      case PSO_INTEGER:
//         ptr->fields[i].length = pInternalDef[i].length1;
         
         break;

      case PSO_DECIMAL:
//         ptr->fields[i].precision = pInternalDef[i].length1;
//         ptr->fields[i].scale     = pInternalDef[i].length2;

         break;

      case PSO_BOOLEAN:

         break;

      case PSO_VAR_BINARY:
      case PSO_VAR_STRING:

//         ptr->fields[i].minLength = pInternalDef[i].length1;
  //       ptr->fields[i].maxLength = pInternalDef[i].length2;

         break;
      }
      cout << " ";
//      cout << oss;
   }

   cout << endl;
//   cout << buffer << endl;

}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
   
void psoShell::DisplayKey()
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoShell::Parse( string & inStr )
{
   istringstream iss( inStr );
   string s;
   string::size_type len;
   int count = 0;
   
   tokens.clear();

   // Cannot start with a "
   if ( inStr[0] == '\'' ) throw(2);

   len = inStr.find('\'');
   if ( len == string::npos ) {
      // Simpler case, no quotes
      do {
         getline( iss, s, ' ');
         tokens.push_back( s );
      } while ( ! iss.eof() );
      
      return;
   }
   
   // We have to handle multiple formats, for example:
   //   touch -q "My QUeue"
   //   touch "My Queue" -q
   //   cp "My Q 1" "My Q 2"
   //
   // The trick we can use: when we break the string using the '"' delimiter,
   // odd segments are what is quoted and even segments are what must be
   // further break down using the ' ' delimiter.
   
   do {
      getline( iss, s, '\'');
      Trim(s);
      if ( s.length() > 0 ) {
         if ( (count % 2) == 0 ) {
            istringstream iss2( s );
            string s2;

            do {
               getline( iss2, s2, ' ');
               tokens.push_back( s2 );
            } while ( ! iss2.eof() );
         }
         else {
            tokens.push_back( s );
         }
      }
      else {
         // Error - a pair of " with no data
         if ( (count % 2) == 1 ) throw(2);
      }
      
      count++;
   } while ( ! iss.eof() );
   
   if ( (count % 2) == 0 ) throw(2);
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoShell::Run()
{
   string inStr;
   bool timeToExit = false;
   
   while ( true ) {
      
      cout << "pso$ "; // << endl;
      getline( cin, inStr );
      if ( cin.eof() ) { // Control-d was used on a unix shell
         cin.clear();
         cin.ignore();
         cout << endl;
         return;
      }
      if ( inStr.length() == 0 ) continue;
      
      try {
         Parse( Trim(inStr) );
         timeToExit = Dispatch();
      }
      catch ( int rc ) {
         if ( rc == 0 ) {
            cerr << "psosh: " << tokens[0] << ": command not found" << endl;
         }
         else if ( rc == 1 ) {
            cerr << "psosh: " << tokens[0] << ": invalid number of arguments" << endl;
         }
         else {
            cerr << "psosh: " << "Malformed arguments (missing quote?)" << endl;
         }
      }
      if ( timeToExit ) return;
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

string & psoShell::Trim( string & s )
{
   unsigned i;

   for ( i = 0; i < s.length(); ++i ) {
      if ( s[i] != ' ' ) break;
   }
   
   s = s.assign( s, i, s.length()-i );

   while ( s.length() > 0 ) {
      if ( s[s.length()-1] == ' ' ) {
         s.erase( s.length()-1 );
      }
      else break;
   }
   return s;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

// The "list" of currently implemented pseudo-shell commands starts here.

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoShell::Cat()
{
   string objectName;
   psoObjStatus status;
   unsigned char * key, * buffer;
   int rc;
   size_t keyLength, dataLength;
   psoObjectDefinition * pDefinition = NULL;
   size_t * offsets;
   
   if ( tokens[1][0] == '/' ) {
      // Absolute path
      objectName = tokens[1];
   }
   else {
      objectName = currentLocation + tokens[1];
   }
   
   // Must check if object exists (and its type)
   try {
      session.GetStatus( objectName, &status );
   }
   catch ( psoException exc ) {
      exc = exc; // Avoid a warning
      cerr << "psosh: cat: " << objectName << ": Invalid object name" << endl;
      return;
   }
   if ( status.type == PSO_FOLDER ) {
      cerr << "psosh: cat: " << objectName << ": Is a directory/folder" << endl;
      return;
   }
   
   try {
      session.GetDefinition( objectName, &pDefinition );
   }
   catch ( psoException exc ) {
      cerr << "psosh: cat: " << exc.Message() << endl;
      return;
   }

   try {
      offsets = new size_t[pDefinition->numFields];
   }
   catch ( ... ) {
      cerr << "psosh: cat: Not enough memory " << endl;
      cerr << "Number of fields in data definition = " << pDefinition->numFields << endl;
      return;
   }
   
   psoaGetOffsets( pDefinition, offsets );
   for ( size_t i = 0; i < pDefinition->numFields; ++i ) {
      cout << "offsets[" << i << "] = " << offsets[i] << endl;
   }
   
   try {
      // Do we have some data to copy?
      if ( status.numDataItem > 0 ) {
         
         buffer = new unsigned char[status.maxDataLength+1];

         if ( status.type == PSO_HASH_MAP ) {
         
            psoHashMap hashMap(session);
         
            key = new unsigned char[status.maxKeyLength+1];
         
            hashMap.Open( objectName );
         
            memset( key, 0, status.maxKeyLength+1 );
            memset( buffer, 0, status.maxDataLength+1 );
            rc = hashMap.GetFirst( key, status.maxKeyLength, 
                                   buffer, status.maxDataLength,
                                   &keyLength, &dataLength );
            while ( rc == 0 ) {
               cout << "key : " << key << endl;
               cout << "data: " << buffer << endl;
               memset( key, 0, status.maxKeyLength+1 );
               memset( buffer, 0, status.maxDataLength+1 );
               rc = hashMap.GetNext( key, status.maxKeyLength, 
                                     buffer, status.maxDataLength,
                                     &keyLength, &dataLength );
            }
            hashMap.Close();
         }
         else if ( status.type == PSO_QUEUE ) {

            psoQueue queue(session);
         
            queue.Open( objectName );
         
            memset( buffer, 0, status.maxDataLength+1 );
            rc = queue.GetFirst( buffer, status.maxDataLength, &dataLength );
            while ( rc == 0 ) {
               DisplayData( pDefinition, offsets, buffer, dataLength );
               memset( buffer, 0, status.maxDataLength+1 );
               rc = queue.GetNext( buffer, status.maxDataLength, &dataLength );
            }
            queue.Close();
         }
      }
   }
   catch ( psoException exc ) {
      if ( pDefinition != NULL ) free(pDefinition);
      cerr << "psosh: cat: " << exc.Message() << endl;
      return;
   }

   if ( pDefinition != NULL ) free(pDefinition);
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoShell::Cd()
{
   string newLoc;
   psoObjStatus status;

   if ( tokens.size() == 1 ) {
      currentLocation = "/";
      return;
   }

   if ( tokens[1][0] == '/' ) {
      // Absolute path
      newLoc = tokens[1];
   }
   else {
      newLoc = currentLocation + tokens[1];
   }
   
   // Add trailing '/', if needed
   if ( newLoc[newLoc.length()-1] != '/' ) newLoc += '/';
   
   // Must check if folder exists
   try {
      session.GetStatus( newLoc, &status );
   }
   catch ( psoException exc ) {
      exc = exc; // Avoid a warning
      cerr << "psosh: cd: " << newLoc << ": Invalid folder name" << endl;
      return;
   }
   if ( status.type != PSO_FOLDER ) {
      cerr << "psosh: cd: " << newLoc << ": Invalid folder name" << endl;
      return;
   }
   currentLocation = newLoc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoShell::Cp()
{
   string srcName, destName;
   psoObjStatus status;
   unsigned char * key, * buffer;
   int rc;
   size_t keyLength, dataLength;
   psoObjectDefinition definition;
   
   if ( tokens[1][0] == '/' ) {
      // Absolute path
      srcName = tokens[1];
   }
   else {
      srcName = currentLocation + tokens[1];
   }
   
   if ( tokens[2][0] == '/' ) {
      // Absolute path
      destName = tokens[2];
   }
   else {
      destName = currentLocation + tokens[2];
   }
   
   // Must check if source object exists
   try {
      session.GetStatus( srcName, &status );
   }
   catch ( psoException exc ) {
      exc = exc; // Avoid a warning
      cerr << "psosh: cp: " << srcName << ": Invalid object name" << endl;
      return;
   }
   if ( status.type == PSO_FOLDER ) {
      cerr << "psosh: cp: " << srcName << ": Is a directory/folder" << endl;
      return;
   }
   
   try {
      definition.type = status.type;
      session.CreateObject( destName, &definition );
      // Do we have some data to copy?
      if ( status.numDataItem > 0 ) {
         
         buffer = new unsigned char[status.maxDataLength];

         if ( status.type == PSO_HASH_MAP ) {
         
            psoHashMap srcHash(session), destHash(session);
         
            key = new unsigned char[status.maxKeyLength];
         
            srcHash.Open( srcName );
            destHash.Open( destName );
         
            rc = srcHash.GetFirst( key, status.maxKeyLength, 
                                   buffer, status.maxDataLength,
                                   &keyLength, &dataLength );
            while ( rc == 0 ) {
               destHash.Insert( key, keyLength, buffer, dataLength );
               rc = srcHash.GetNext( key, status.maxKeyLength, 
                                     buffer, status.maxDataLength,
                                     &keyLength, &dataLength );
            }
            srcHash.Close();
            destHash.Close();
         }
         else if ( status.type == PSO_QUEUE ) {

            psoQueue srcQueue(session), destQueue(session);
         
            srcQueue.Open( srcName );
            destQueue.Open( destName );
         
            rc = srcQueue.GetFirst( buffer, status.maxDataLength, &dataLength );
            while ( rc == 0 ) {               
               destQueue.Push( buffer, dataLength );
               rc = srcQueue.GetNext( buffer, status.maxDataLength, &dataLength );
            }
            srcQueue.Close();
            destQueue.Close();
         }
      }
      session.Commit();
   }
   catch ( psoException exc ) {
      session.Rollback();  // just in case it's the Commit that fails
      cerr << "psosh: cp: " << exc.Message() << endl;
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoShell::Echo()
{
   string objectName;
   psoObjStatus status;
   unsigned char * key = NULL, * buffer = NULL;
   size_t keyLength, dataLength;
   psoObjectDefinition * pDefinition = NULL;
   
   if ( tokens[3][0] == '/' ) {
      // Absolute path
      objectName = tokens[3];
   }
   else {
      objectName = currentLocation + tokens[3];
   }
   
   // Must check if object exists (and its type)
   try {
      session.GetStatus( objectName, &status );
   }
   catch ( psoException exc ) {
      exc = exc; // Avoid a warning
      cerr << "psosh: echo: " << objectName << ": Invalid object name" << endl;
      return;
   }
   if ( status.type == PSO_FOLDER ) {
      cerr << "psosh: echo: " << objectName << ": Is a directory/folder" << endl;
      return;
   }
   
   try {
      session.GetDefinition( objectName, &pDefinition );
   }
   catch ( psoException exc ) {
      cerr << "psosh: echo: " << exc.Message() << endl;
      return;
   }

   try {
      if ( status.type == PSO_HASH_MAP ) {
         key = psotsInToKey( tokens[1], pDefinition, keyLength );
      }
      buffer = psotsInToBuff( tokens[1], pDefinition, dataLength );
   }   
   catch ( exception exc ) {
      cerr << "psosh: echo: Not enough memory " << endl;
      cerr << "Number of fields in data definition = " << pDefinition->numFields << endl;
      return;
   }
   catch( unsigned int count ) {
      if ( count == 0 ) { 
         cerr << "psosh: echo: Invalid key" << endl;
      }
      else {
         cerr << "psosh: echo: Invalid input field #" << count << endl;
      }
   }
   
   try {
      if ( status.type == PSO_HASH_MAP ) {
   
         psoHashMap hashMap(session);
         
         hashMap.Open( objectName );
         
         try {
            hashMap.Insert( key, keyLength, buffer, dataLength );
         }
         catch( ... ) {
            // we try a replace instead
         }
         hashMap.Close();
      }
      else if ( status.type == PSO_QUEUE ) {
         psoQueue queue(session);
         
         queue.Open( objectName );
         
         queue.Close();
      }
   }
   catch ( psoException exc ) {
      if ( pDefinition != NULL ) free(pDefinition);
      cerr << "psosh: cat: " << exc.Message() << endl;
      return;
   }

   if ( pDefinition != NULL ) free(pDefinition);
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoShell::Free()
{
   psoInfo info;
   
   try {
      session.GetInfo( &info );
   }
   catch( psoException exc ) {
      cerr << "psosh: free: " << exc.Message() << endl;
      return;
   }

   cout << "shared-mem total size : " << constants.Bytes(info.totalSizeInBytes) << endl;
   cout << "allocated memory      : " << constants.Bytes(info.allocatedSizeInBytes) << endl;
   cout << "number of objects     : " << info.numObjects << endl;
   cout << "number of groups      : " << info.numGroups << endl;
   cout << "number of mallocs     : " << info.numMallocs << endl;
   cout << "number of frees       : " << info.numFrees << endl;
   cout << "largest free space    : " << constants.Bytes(info.largestFreeInBytes) << endl;

}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoShell::Ls()
{
   psoFolder folder( session );
   int rc;
   psoFolderEntry entry;
   string folderName = currentLocation;
   
   if ( tokens.size() == 2 ) {
      if ( tokens[1][0] == '/' ) {
         // Absolute path
         folderName = tokens[1];
      }
      else {
         folderName = currentLocation + tokens[1];
      }
   }
   
   try {
      folder.Open( folderName );
      
      rc = folder.GetFirst( &entry );
      cout << constants.TypeHeader() << " " << constants.StatusHeader() << " Name" << endl;
      while ( rc == 0 ) {
         cout << constants.Type(entry.type) << " " << constants.Status(entry.status) << " " << entry.name << endl;
         rc = folder.GetNext( &entry );
      }
      folder.Close();
   }
   catch( psoException exc ) {
      if ( exc.ErrorCode() == PSO_NO_SUCH_OBJECT || 
         exc.ErrorCode() == PSO_NO_SUCH_FOLDER ) {
         cerr << "psosh: " << tokens[0] << ": " << "No such file or directory" << endl;
      }
      else {
         cerr << "psosh: " << tokens[0] << ": " << exc.Message() << endl;
      }
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoShell::Man()
{
   cout << "List of available commands: " << endl << endl;
   cout << "cat object_name" << endl;
   cout << "    Display the content of object named object_name." << endl;
   cout << "    Warning: print the output as strings." << endl;
   cout << "cd [folder_name]" << endl;
   cout << "    Without a name, change the current folder to \"/\"." << endl;
   cout << "    Change the current folder to folder_name." << endl;
   cout << "cp source_name dest_name" << endl;
   cout << "    Copy the source_name object to the dest_name object." << endl;
   cout << "    Warning: dest_name is not overwritten if it already exists." << endl;
   cout << "del/rm object_name" << endl;
   cout << "    Delete the data container named object_name" << endl;
   cout << "dir/ls [folder_name]" << endl;
   cout << "    Without a name, list the objects of the current folder." << endl;
   cout << "    List the objects of the folder folder_name." << endl;
   cout << "exit/quit" << endl;
   cout << "    exit the program." << endl;
   cout << "free" << endl;
   cout << "    display amount of used/free memory (main allocator)" <<endl;
   cout << "help/man" << endl;
   cout << "    The current list." << endl;
   cout << "mkdir folder_name" << endl;
   cout << "    Create a new folder" << endl;
   cout << "pwd" << endl;
   cout << "    Display the current folder." << endl;
   cout << "rmdir folder_name" << endl;
   cout << "    Delete the folder folder_name" << endl;
   cout << "stat object_name" << endl;
   cout << "    Show the status of the object object_name" << endl;
   cout << "touch -h|-q object_name" << endl;
   cout << "    Create a new object named object_name of the specified type:" << endl;
   cout << "    -h or --hashmap: a hash map" << endl;
   cout << "    -q or --queue  : a FIFO queue" << endl;
   cout << endl;
   cout << "Note: use a pair of \" to include white spaces in names." << endl;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoShell::Mkdir()
{
   string folderName;
   psoObjectDefinition definition;

   if ( tokens[1][0] == '/' ) {
      // Absolute path
      folderName = tokens[1];
   }
   else {
      folderName = currentLocation + tokens[1];
   }
   
   try {
      definition.type = PSO_FOLDER;
      session.CreateObject( folderName, &definition );
      session.Commit();
   }
   catch ( psoException exc ) {
      session.Rollback();  // just in case it's the Commit that fails
      cerr << "psosh: mkdir: " << exc.Message() << endl;
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoShell::Rm()
{
   string objectName;
   psoObjStatus status;
   
   if ( tokens[1][0] == '/' ) {
      // Absolute path
      objectName = tokens[1];
   }
   else {
      objectName = currentLocation + tokens[1];
   }
   
   // Must check if object exists
   try {
      session.GetStatus( objectName, &status );
   }
   catch ( psoException exc ) {
      exc = exc; // Avoid a warning
      cerr << "psosh: rm: " << objectName << ": Invalid object name" << endl;
      return;
   }
   if ( status.type == PSO_FOLDER ) {
      cerr << "psosh: rm: " << objectName << ": Is a directory/folder" << endl;
      return;
   }
   
   try {
      session.DestroyObject( objectName );
      session.Commit();
   }
   catch ( psoException exc ) {
      session.Rollback();  // just in case it's the Commit that fails
      cerr << "psosh: rm: " << exc.Message() << endl;
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoShell::Rmdir()
{
   string folderName;
   psoObjStatus status;
   
   if ( tokens[1][0] == '/' ) {
      // Absolute path
      folderName = tokens[1];
   }
   else {
      folderName = currentLocation + tokens[1];
   }
   
   // Must check if folder exists
   try {
      session.GetStatus( folderName, &status );
   }
   catch ( psoException exc ) {
      exc = exc; // Avoid a warning
      cerr << "psosh: rmdir: " << folderName << ": Invalid folder name" << endl;
      return;
   }
   if ( status.type != PSO_FOLDER ) {
      cerr << "psosh: rmdir: " << folderName << ": Not a directory/folder" << endl;
      return;
   }
   
   try {
      session.DestroyObject( folderName );
      session.Commit();
   }
   catch ( psoException exc ) {
      session.Rollback();  // just in case it's the Commit that fails
      cerr << "psosh: rmdir: " << exc.Message() << endl;
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoShell::Stat()
{
   string objectName;
   psoObjStatus status;
   
   if ( tokens[1][0] == '/' ) {
      // Absolute path
      objectName = tokens[1];
   }
   else {
      objectName = currentLocation + tokens[1];
   }
   
   try {
      session.GetStatus( objectName, &status );
   }
   catch ( psoException exc ) {
      cerr << "psosh: stat: " << objectName << ": " << exc.Message() << endl;
      return;
   }
   cout << "Object full name     : " << objectName << endl;
   cout << "Object type          : " << constants.Type(status.type)     << endl;
   cout << "Status               : " << constants.Status(status.status) << endl;
   cout << "Total size           : " << constants.Bytes(status.numBlocks*PSON_BLOCK_SIZE) << endl;
   cout << "Number of blocks     : " << status.numBlocks << endl;
   cout << "# of group of blocks : " << status.numBlockGroup << endl;
   cout << "Number of data items : " << status.numDataItem << endl;
   cout << "Free space           : " << constants.Bytes(status.freeBytes) << endl;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoShell::Touch()
{
   string objectName;
   string option, filename;
   psoObjectDefinition definition;
   bool useXML = false;
   psoFolder folder( session );
   
   definition.type = PSO_LAST_OBJECT_TYPE;
   
   if ( tokens[1][0] == '-' ) {
      option   = tokens[1];
      filename = tokens[2];
   }
   else if ( tokens[2][0] == '-' ) {
      option   = tokens[2];
      filename = tokens[1];
   }
   else {
      cerr << "psosh: touch: " << "Usage: touch -h|-q object_name" << endl;
      return;
   }
   
   if ( option == "-q" || option == "--queue" ) {
      definition.type = PSO_QUEUE;
   }
   else if ( option == "-h" || option == "--hashmap" ) {
      definition.type = PSO_HASH_MAP;
   }
   else if ( option == "-x" || option == "--xml" ) {
      useXML = true;
   }
   else {
      cerr << "psosh: touch: " << "invalid option (" << option << ")" << endl;
      return;
   }

   if ( useXML ) {
      try {
         cerr << filename << endl;
         folder.Open( currentLocation );
         folder.CreateObjectXML( filename );
         folder.Close();
         session.Commit();
      }
      catch ( psoException exc ) {
         session.Rollback();  // just in case it's the Commit that fails
         cerr << "psosh: touch: " << exc.Message() << endl;
      }
   }
   else
   {
      if ( filename[0] == '/' ) {
         // Absolute path
         objectName = filename;
      }
      else {
         objectName = currentLocation + filename;
      }
   
      try {
         session.CreateObject( objectName, &definition );
         session.Commit();
      }
      catch ( psoException exc ) {
         session.Rollback();  // just in case it's the Commit that fails
         cerr << "psosh: touch: " << exc.Message() << endl;
      }
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--


