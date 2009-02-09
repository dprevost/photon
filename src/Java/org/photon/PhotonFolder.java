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

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

/**
 * Folder class for the Photon library.
 */

class PhotonFolder {

   /** To save the native pointer/handle. */
   private long handle = 0;
   private long sessionHandle = 0;

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
   private int entryType;
   private String entryName;
   private int entryStatus;
   
   public int getEntryType()    { return entryType; }
   public String getEntryName() { return entryName; }
   public int getEntryStatus()  { return entryStatus; }
   
   public PhotonFolder( PhotonSession session ) throws PhotonException {
      sessionHandle = session.Handle();
   }

   public PhotonFolder( PhotonSession session, String name ) throws PhotonException {
      sessionHandle = session.Handle();
      handle = folderInit( sessionHandle, name );
   }

   private native void folderCreateObject( long h, String objectName, 
      ObjectDefinition definition ) throws PhotonException;
   private native void folderCreateObjectXML( long h, String xmlBuffer ) throws PhotonException;
   private native void folderDestroyObject( long h, String objectName ) throws PhotonException;
   private native void folderFini( long h );
   private native boolean folderGetFirst( long h, int entryType, 
      String entryName, int entryStatus ) throws PhotonException;
   private native boolean folderGetNext( long h, int entryType, 
      String entryName, int entryStatus ) throws PhotonException;
   private native long folderInit( long h, String s ) throws PhotonException;
   private native void folderStatus( long h, ObjStatus status ) throws PhotonException;

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   protected void finalize() {
      folderFini( handle );
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public void close() { folderFini(handle); }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public void createObject( String           objectName,
                             ObjectDefinition definition) throws PhotonException {
      int rc;

      if ( sessionHandle == 0 || handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      folderCreateObject( handle, 
                          objectName, 
                          definition );
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public void createObjectXML(String xmlBuffer) throws PhotonException {
      int rc;

      if ( sessionHandle == 0 || handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      folderCreateObjectXML( handle, xmlBuffer );
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public void destroyObject(String objectName) throws PhotonException {
      int rc;

      if ( sessionHandle == 0 || handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      folderDestroyObject( handle, objectName );
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public boolean getNext() throws PhotonException {
   
      boolean rc;

      if ( sessionHandle == 0 || handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      try {
         if ( endIteration ) {
            endIteration = false;
            rc = folderGetFirst( handle, entryType, entryName, entryStatus );
         } else {
            rc = folderGetNext( handle, entryType, entryName, entryStatus );
         }
         if ( ! rc ) { endIteration = true; }
      } catch (PhotonException e) {
         endIteration = true;
         throw e;
      }
      
      return rc;
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public void open( String folderName ) throws PhotonException {
      int rc;

      if ( sessionHandle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }
   
      handle = folderInit( sessionHandle, folderName );
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public void status( ObjStatus status) throws PhotonException {
            int rc;

      if ( sessionHandle == 0 || handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      folderStatus( handle, status );
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
}

