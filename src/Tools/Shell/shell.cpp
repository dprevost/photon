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
      ls();
   }
   else if ( tokens[0] == s.assign("exit") ) {
      timeToExit = true;
   }
   else if ( tokens[0] == s.assign("ls") ) {
      ls();
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
   else
      throw( 0 );
   
   return timeToExit;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsShell::Parse( string & inStr )
{
   istringstream iss( inStr );
   string s;

   tokens.clear();
   do
   {
      getline( iss, s, ' ');
      tokens.push_back( s );
   } while ( ! iss.eof() );
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
      Parse( inStr );
      
      try {
         timeToExit = Dispatch();
      }
      catch ( int rc )
      {
         if ( rc == 0 )
            cerr << "vdssh: " << tokens[0] << ": command not found" << endl;
         else
            cerr << "vdssh: " << tokens[0] << ": invalid number of arguments" << endl;
      }
      if ( timeToExit )
         return;
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsShell::cd()
{
   string newLoc, folderName;
   vdsObjStatus status;

   if ( tokens.size() == 1 )
   {
      currentLocation = "/";
      return;
   }
   // Strip leading '/' and check for special case of "/"
   newLoc = tokens[1];
   if ( newLoc[0] == '/' )
      newLoc.erase( 0 );
   if ( newLoc.length() == 0 )
   {
      currentLocation = "/";
      return;
   }
   // Add trailing '/', if needed
   if ( newLoc[newLoc.length()-1] != '/' )
      newLoc += '/';
   
   folderName = currentLocation;
   folderName += newLoc;

   // Must check if folder exists
   try {
      session.GetStatus( folderName, &status );
   }
   catch ( int rc )
   {
      cerr << "vdsh: cd: " << folderName << ": Invalid folder name" << endl;
      return;
   }
   currentLocation = folderName;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsShell::ls()
{
   vdsFolder folder( session );
   int rc;
   vdsFolderEntry entry;
   const string folderName = currentLocation;
   
   try {
      folder.Open( currentLocation );
      rc = folder.GetFirst( &entry );
      while ( rc == 0 )
      {
         cout << entry.type << " " << entry.status << " " << entry.name << endl;
         rc = folder.GetNext( &entry );
      }
      folder.Close();
   }
   catch( int e )
   {
      cerr << "vdsh: ls: " << e << endl;
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsShell::mkdir()
{
   string folderName;
   
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
      session.Rollback();
      cerr << "vdsh: mkdir: " << rc << endl;
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
