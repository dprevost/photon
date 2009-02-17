/*
 * Copyright (C) 2009 Daniel Prevost <dprevost@photonsoftware.org>
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
 * <p>
 * Folders allow the organization of the data containers in a tree-like
 * structure (similar to a filesystem).
 * <p>
 * If a flat structure is preferred, simply make all your data containers
 * children of the top folder ("/") - for example, "/MyQueue", "/MyMap", etc.
 */
public class Folder implements Iterable<FolderEntry>, Iterator<FolderEntry> {

   /* To save the native pointer/handle of the C struct. */
   private long handle = 0;
   private Session session;
   private FolderEntry entry;
   
   private boolean nextWasQueried = false;

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
      
      if ( nextWasQueried ) {
         nextWasQueried = false;
         return entry; 
      }
      
      try {
         if ( getNext() ) {
            return entry;
         }
      } catch (PhotonException e) {
         System.out.println( e.getMessage() );
      }

      throw new NoSuchElementException();
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public void remove() {
      throw new UnsupportedOperationException();
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /** This method implements the Iterable interface */
   public Iterator<FolderEntry> iterator() {
      return this;
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * A default constructor. You must use open() to access a folder in 
    * shared memory.
    */
   public Folder( Session session ) {

      this.session = session;
      
      entry = new FolderEntry();
   }

   /** Opens a Photon folder */
   public Folder( Session session, String name ) throws PhotonException {
      
      int errcode;
      
      this.session = session;
      
      errcode = psoInit( session, name );
      if ( errcode != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(errcode) );
      }

      entry = new FolderEntry();
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   private static native void initIDs();

   static {
      initIDs();
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   private native int psoCreateObject( long              h, 
                                       String            objectName, 
                                       ObjectDefinition  definition, 
                                       KeyDefinition     key,
                                       FieldDefinition[] fields );

   private native int psoCreateObjectXML( long h, String xmlBuffer );

   private native int psoDestroyObject( long h, String objectName );

   private native void psoFini( long h );

   private native int psoGetFirst( long h, FolderEntry e );

   private native int psoGetNext( long h, FolderEntry e );

   private native int psoInit( Session session, String s );

   private native int psoStatus( long h, ObjectStatus status );

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   protected void finalize() throws Throwable {     
      
      try {
         psoFini(handle);
      } finally {
         handle = 0;
         super.finalize();
      }
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Close a folder.
    *
    * This function terminates the current access to the folder in shared 
    * memory - the folder itself is untouched.
    *
    * @exception PhotonException On an error with the Photon library.
    */
   public void close() { 
      psoFini(handle);
      handle = 0;
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Create a new object in shared memory as a child of the current folder.
    * <p>
    * The creation of the object only becomes permanent after a call to 
    * Session.commit.
    * <p>
    * This function does not return a Java object linked to the newly 
    * created object. You must use org.photon.Queue and similar to 
    * access the newly created object.
    *
    * @param definition The object definition (its type, etc.)
    * @param key        The definition of the key or null for objects without 
    *                   keys (queues, etc.).
    * @param fields     An array of field definitions. It can be set to
    *                   null when creating a Folder.
    * @exception PhotonException On an error with the Photon library.
    */
   public void createObject( String            objectName,
                             ObjectDefinition  definition, 
                             KeyDefinition     key,
                             FieldDefinition[] fields ) throws PhotonException {
      int errcode;

      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      errcode = psoCreateObject( handle, 
                                 objectName, 
                                 definition,
                                 key,
                                 fields );
      if ( errcode != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(errcode) );
      }
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Create a new object in shared memory as a child of the current folder.
    * <p>
    * The creation of the object only becomes permanent after a call to 
    * Session.commit.
    * <p>
    * This function does not return a Java object linked to the newly 
    * created object. You must use org.photon.Queue and similar to 
    * access the newly created object.
    *
    * @param xmlBuffer  The XML string containing all the required
    *                   information. 
    * @exception PhotonException On an error with the Photon library.
    */
   public void createObjectXML(String xmlBuffer) throws PhotonException {

      int errcode;

      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      errcode = psoCreateObjectXML( handle, xmlBuffer );
      if ( errcode != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(errcode) );
      }
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Destroy an object, child of the current folder, in shared memory.
    * <p>
    * The destruction of the object only becomes permanent after a call to 
    * Session.commit.
    *
    * @param objectName          The name of the object. 
    * @exception PhotonException On an error with the Photon library.
    */
   public void destroyObject(String objectName) throws PhotonException {

      int errcode;

      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      errcode = psoDestroyObject( handle, objectName );
      if ( errcode != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(errcode) );
      }
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   private boolean getNext() throws PhotonException {
   
      int errcode;

      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      if ( endIteration ) {
         endIteration = false;
         errcode = psoGetFirst( handle, entry );
      } else {
         errcode = psoGetNext( handle, entry );
      }
      if ( errcode == 0 ) { 
         return true;
      }
      endIteration = true;
      if ( errcode == PhotonErrors.IS_EMPTY.getErrorNumber() || 
           errcode == PhotonErrors.REACHED_THE_END.getErrorNumber() ) {
         return false;
      }
      
      throw new PhotonException( PhotonErrors.getEnum(errcode) );
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /** 
    * Open an existing folder. 
    * <p>
    * If the folder is already open, you must close it first.
    *
    * @param folderName The fully qualified name of the folder. 
    * @exception PhotonException On an error with the Photon library.
    */
   public void open( String folderName ) throws PhotonException {

      int errcode;
      
      if ( handle != 0 ) {
         throw new PhotonException( PhotonErrors.ALREADY_OPEN );
      }

      errcode = psoInit( session, folderName );
      if ( errcode != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(errcode) );
      }
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Access the current status of the folder.
    *
    * @return A new status object.
    * @exception PhotonException On an error with the Photon library.
    */
   public ObjectStatus getStatus() throws PhotonException {

      int errcode;
      ObjectStatus status = new ObjectStatus();

      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      errcode = psoStatus( handle, status );
      if ( errcode != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(errcode) );
      }
      
      return status;
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
}

