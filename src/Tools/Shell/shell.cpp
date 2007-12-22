/*
 * Copyright (C) 2007 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of vdsf (Virtual Data Space Framework).
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
#include <string>
#include <sstream>
#include <iostream>
#include <vector>

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

vdsShell::vdsShell(vdsSession & s)
   : currentLocation( "/" ),
     session        ( s )
{
}  

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

vdsShell::~vdsShell()
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

bool vdsShell::Dispatch()
{
   bool timeToExit = false;
   string s;
   
   if ( tokens[0] == s.assign("cd") ) {
      if ( tokens.size() > 2 ) throw( 1 );
      cd();
   }
   else if ( tokens[0] == s.assign("dir") ) {
      if ( tokens.size() > 2 ) throw( 1 );
      ls();
   }
   else if ( tokens[0] == s.assign("exit") ) {
      timeToExit = true;
   }
   else if ( tokens[0] == s.assign("free") ) {
      free();
   }
   else if ( tokens[0] == s.assign("help") ) {
      man();
   }
   else if ( tokens[0] == s.assign("ls") ) {
      ls();
   }
   else if ( tokens[0] == s.assign("man") ) {
      man();
   }
   else if ( tokens[0] == s.assign("mkdir") ) {
      if ( tokens.size() != 2 ) throw( 1 );
      mkdir();
   }
   else if ( tokens[0] == s.assign("pwd") ) {
      cerr << currentLocation << endl;
   }
   else if ( tokens[0] == s.assign("quit") ) {
      timeToExit = true;
   }
   else if ( tokens[0] == s.assign("rmdir") ) {
      if ( tokens.size() != 2 ) throw( 1 );
      rmdir();
   }
   else if ( tokens[0] == s.assign("stat") ) {
      if ( tokens.size() != 2 ) throw( 1 );
      stat();
   }
   else
      throw( 0 );
   
   return timeToExit;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsShell::Parse( string & inStr )
{
   istringstream iss( inStr );
   string s;
   string::size_type len1, len2;

   tokens.clear();

   len1 = inStr.find('"');
   if ( len1 == string::npos )
   {
      do
      {
         getline( iss, s, ' ');
         tokens.push_back( s );
      } while ( ! iss.eof() );
   }
   else
   {
      len2 = inStr.find('"',len1+1);
      if ( len2 == string::npos || (len2-len1) == 1 )
         throw(2);
      do
      {
         // Incomplete - options are not taken into account (but we don't
         // support options yet...)
         getline( iss, s, '"');
         Trim(s);
         if ( s.length() > 0 )
            tokens.push_back( s );
      } while ( ! iss.eof() );
   }
   if ( tokens.size() == 0 )
      throw(2);
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsShell::Run()
{
   string inStr;
   bool timeToExit = false;
   
   while ( true )
   {
      cout << "vds$ "; // << endl;
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
      catch ( int rc )
      {
         if ( rc == 0 )
            cerr << "vdssh: " << tokens[0] << ": command not found" << endl;
         else if ( rc == 1 )
            cerr << "vdssh: " << tokens[0] << ": invalid number of arguments" << endl;
         else
            cerr << "vdssh: " << "Malformed arguments (missing quote?)" << endl;
      }
      if ( timeToExit )
         return;
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

string & vdsShell::Trim( string & s )
{
//   string s = inStr;
   unsigned i;

   for ( i = 0; i < s.length(); ++i )
      if ( s[i] != ' ' ) break;
   
   s = s.assign( s, i, s.length()-i );

   while ( s.length() > 0 ) {
      if ( s[s.length()-1] == ' ' )
         s.erase( s.length()-1 );
      else
         break;
   }
   return s;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

// The "list" of currently implemented pseudo-shell commands starts here.

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsShell::cd()
{
   string newLoc;
   vdsObjStatus status;

   if ( tokens.size() == 1 )
   {
      currentLocation = "/";
      return;
   }

   if ( tokens[1][0] == '/' )
      // Absolute path
      newLoc = tokens[1];
   else
      newLoc = currentLocation + tokens[1];

   // Add trailing '/', if needed
   if ( newLoc[newLoc.length()-1] != '/' )
      newLoc += '/';
   
   // Must check if folder exists
   try {
      session.GetStatus( newLoc, &status );
   }
   catch ( int rc )
   {
      cerr << "vdssh: cd: " << newLoc << ": Invalid folder name" << endl;
      return;
   }
   if ( status.type != VDS_FOLDER )
   {
      cerr << "vdssh: cd: " << newLoc << ": Invalid folder name" << endl;
      return;
   }
   currentLocation = newLoc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsShell::free()
{
   vdsInfo info;
   string msg;
   
   try {
      session.GetInfo( &info );
   }
   catch( int rc )
   {
      session.ErrorMsg( msg );
      cerr << "vdssh: free: " << msg << endl;
      return;
   }

   cout << "vds total size     : " << constants.Bytes(info.totalSizeInBytes) << endl;
   cout << "allocated memory   : " << constants.Bytes(info.allocatedSizeInBytes) << endl;
   cout << "number of objects  : " << info.numObjects << endl;
   cout << "number of groups   : " << info.numGroups << endl;
   cout << "number of mallocs  : " << info.numMallocs << endl;
   cout << "number of frees    : " << info.numFrees << endl;
   cout << "largest free space : " << constants.Bytes(info.largestFreeInBytes) << endl;

}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsShell::ls()
{
   vdsFolder folder( session );
   int rc;
   vdsFolderEntry entry;
   string folderName = currentLocation;
   string msg;
   
   if ( tokens.size() == 2 )
   {
      if ( tokens[1][0] == '/' )
         // Absolute path
         folderName = tokens[1];
      else
         folderName = currentLocation + tokens[1];
   }
   
   try {
      folder.Open( folderName );
      rc = folder.GetFirst( &entry );
      while ( rc == 0 )
      {
         cout << constants.Type(entry.type) << " " << constants.Status(entry.status) << " " << entry.name << endl;
         rc = folder.GetNext( &entry );
      }
      folder.Close();
   }
   catch( int e )
   {
      session.ErrorMsg( msg );
      cerr << "vdssh: " << tokens[0] << ": " << msg << endl;
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsShell::man()
{
   cout << "List of available commands: " << endl << endl;
   cout << "cd [folder_name]" << endl;
   cout << "    Without a name, change the current folder to \"/\"." << endl;
   cout << "    Change the current folder to folder_name." << endl;
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
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsShell::mkdir()
{
   string folderName;
   string msg;

   if ( tokens[1][0] == '/' )
      // Absolute path
      folderName = tokens[1];
   else
      folderName = currentLocation + tokens[1];

   try {
      session.CreateObject( folderName, VDS_FOLDER );
      session.Commit();
   }
   catch ( int rc ) {
      session.ErrorMsg( msg );
      session.Rollback();  // just in case it's the Commit that fails
      cerr << "vdssh: mkdir: " << msg << endl;
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsShell::rmdir()
{
   string folderName;
   vdsObjStatus status;
   string msg;
   
   if ( tokens[1][0] == '/' )
      // Absolute path
      folderName = tokens[1];
   else
      folderName = currentLocation + tokens[1];

   // Must check if folder exists
   try {
      session.GetStatus( folderName, &status );
   }
   catch ( int rc )
   {
      cerr << "vdssh: cd: " << folderName << ": Invalid folder name" << endl;
      return;
   }
   if ( status.type != VDS_FOLDER )
   {
      cerr << "vdssh: cd: " << folderName << ": Invalid folder name" << endl;
      return;
   }
   
   try {
      session.DestroyObject( folderName );
      session.Commit();
   }
   catch ( int rc ) {
      session.ErrorMsg( msg );
      session.Rollback();  // just in case it's the Commit that fails
      cerr << "vdssh: mkdir: " << msg << endl;
   }
}
// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsShell::stat()
{
   string objectName;
   vdsObjStatus status;
   string msg;
   
   if ( tokens[1][0] == '/' )
      // Absolute path
      objectName = tokens[1];
   else
      objectName = currentLocation + tokens[1];

   try {
      session.GetStatus( objectName, &status );
   }
   catch ( int rc ) {
      session.ErrorMsg( msg );
      cerr << "vdssh: cd: " << objectName << ": " << msg << endl;
      return;
   }
   cout << "Object full name     : " << objectName << endl;
   cout << "Object type          : " << constants.Type(status.type)     << endl;
   cout << "Status               : " << constants.Status(status.status) << endl;
   cout << "Total size           : " << constants.Bytes(status.numBlocks*VDSE_BLOCK_SIZE) << endl;
   cout << "Number of blocks     : " << status.numBlocks << endl;
   cout << "# of group of blocks : " << status.numBlockGroup << endl;
   cout << "Number of data items : " << status.numDataItem << endl;
   cout << "Free space           : " << constants.Bytes(status.freeBytes) << endl;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

