/*
 * Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
 *
 * This file is part of Photon (photonsoftware.org).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 or version 3 as published by the 
 * Free Software Foundation and appearing in the file COPYING.GPL2 and 
 * COPYING.GPL3 included in the packaging of this software.
 *
 * Licensees holding a valid Photon Commercial license can use this file 
 * in accordance with the terms of their license.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

package org.photon;

import java.lang.*;
import java.util.*;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

/**
 * Folder class for the Photon library.
 */

class PhotonFolder implements Iterable<FolderEntry>, Iterator<FolderEntry> {

   /** To save the native pointer/handle. */
   private long handle = 0;
   private long sessionHandle = 0;
   private FolderEntry entry;
   
   private boolean nextWasQueried = false;
   /* Iterations
    * 
    * Usage:
    *
    * while ( folder.getNext() ) {
    *     type   = folder.entryType();
    *     name   = folder.entryName();
    *     status = folder.entryStatus();
    * }
    */
   private boolean endIteration = true;

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   // The next three methods implement Iterator.

   // We cannot implement hasNext directly. What we do instead is to
   // get the next item into this.entry and set nextWasQueried to true
   public boolean hasNext() {
      // In case hasNext is called twice without a call to next()
      if ( nextWasQueried ) { return true; }
      
      try {
         nextWasQueried = getNext();
      } catch (PhotonException e) {
         return false;
      }
      
      return nextWasQueried;
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public FolderEntry next() {
      
      nextWasQueried = false;
      
      if ( nextWasQueried ) { return entry; }
      
      try {
         if ( getNext() ) {
            return entry;
         }
      } catch (PhotonException e) {}

      throw new NoSuchElementException();
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public void remove() {
      throw new UnsupportedOperationException();
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   // And this method implements Iterable   
   public Iterator<FolderEntry> iterator() {
      return this;
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public PhotonFolder( PhotonSession session ) {
      sessionHandle = session.Handle();
      
      entry = new FolderEntry();
   }

   public PhotonFolder( PhotonSession session, String name ) throws PhotonException {
      
      int rc;
      
      sessionHandle = session.Handle();
      
      rc = folderInit( sessionHandle, name );
      if ( rc != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(rc) );
      }

      entry = new FolderEntry();
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   private static native void initIDs();

   static {
      initIDs();
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   private native int  folderCreateObject( long              h, 
                                           String            objectName, 
                                           ObjectDefinition  definition, 
                                           KeyDefinition     key,
                                           FieldDefinition[] fields );
   private native int  folderCreateObjectXML( long h, String xmlBuffer );
   private native int  folderDestroyObject( long h, String objectName );
   private native void folderFini( long h );
   private native int  folderGetFirst( long h, FolderEntry e );
   private native int  folderGetNext( long h, FolderEntry e );
   private native int  folderInit( long h, String s );
   private native int  folderStatus( long h, ObjStatus status );

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   protected void finalize() throws Throwable {     
      
      try {
         folderFini(handle);
      } finally {
         super.finalize();
      }
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public void close() { folderFini(handle); }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public void createObject( String            objectName,
                             ObjectDefinition  definition, 
                             KeyDefinition     key,
                             FieldDefinition[] fields ) throws PhotonException {
      int rc;

      if ( sessionHandle == 0 || handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      rc = folderCreateObject( handle, 
                               objectName, 
                               definition,
                               key,
                               fields );
      if ( rc != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(rc) );
      }
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public void createObjectXML(String xmlBuffer) throws PhotonException {

      int rc;

      if ( sessionHandle == 0 || handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      rc = folderCreateObjectXML( handle, xmlBuffer );
      if ( rc != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(rc) );
      }
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public void destroyObject(String objectName) throws PhotonException {

      int rc;

      if ( sessionHandle == 0 || handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      rc = folderDestroyObject( handle, objectName );
      if ( rc != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(rc) );
      }
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public boolean getNext() throws PhotonException {
   
      int rc;

      if ( sessionHandle == 0 || handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      if ( endIteration ) {
         endIteration = false;
         rc = folderGetFirst( handle, entry );
      } else {
         rc = folderGetNext( handle, entry );
      }
      if ( rc == 0 ) { 
         return true;
      }
      endIteration = true;
      if ( rc == PhotonErrors.IS_EMPTY.getErrorNumber() || 
           rc == PhotonErrors.REACHED_THE_END.getErrorNumber() ) {
         return false;
      }
      
      throw new PhotonException( PhotonErrors.getEnum(rc) );
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public void open( String folderName ) throws PhotonException {

      int rc;
      
      if ( sessionHandle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }
   
      rc = folderInit( sessionHandle, folderName );
      if ( rc != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(rc) );
      }
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public void status( ObjStatus status ) throws PhotonException {

      int rc;

      if ( sessionHandle == 0 || handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      rc = folderStatus( handle, status );
      if ( rc != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(rc) );
      }
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
}

