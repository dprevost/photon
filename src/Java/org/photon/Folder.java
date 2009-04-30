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

   /** To save the native pointer/handle of the C struct. */
   private long handle = 0;
   
   /** The session we belong to. */
   private Session session;
   
   /** Use to cache the next entry for hasNext(). */
   private FolderEntry entry;
   
   /**
    * A flag used by hasNext() and next(). Set to true if the next entry 
    * is already cached.
    */
   private boolean nextWasQueried = false;

   /**
    * A flag to indicate if we are starting the iteration or if we are in 
    * the middle of it.
    * <p>
    * Iterations are started with GetFirst().
    */
   private boolean endIteration = true;

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Initialize some global values for the jni when the class is first loaded.
    */
   static {
      initIDs();
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * A default constructor. You must use open() to access a folder in 
    * shared memory.
    */
   public Folder() {

      entry = new FolderEntry();
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /** 
    * Opens a Photon folder 
    *
    * @param session The session we belong to.
    * @param name    The fully qualified name of the folder. 
    *
    * @exception PhotonException On an error with the Photon library.
    */
   public Folder( Session session, String name ) throws PhotonException {
      
      int errcode;
      
      this.session = session;
      
      errcode = psoOpen( session, name );
      if ( errcode != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(errcode) );
      }

      entry = new FolderEntry();
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
      psoClose(handle);
      handle = 0;
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Create a new folder in shared memory as a child of the current folder.
    * <p>
    * The creation of the object only becomes permanent after a call to 
    * Session.commit.
    * <p>
    * This function does not return a Java object linked to the newly 
    * created object. You must use org.photon.Folder.open() to 
    * access the newly created object.
    *
    * @param folderName The name of the new folder.
    *
    * @exception PhotonException On an error with the Photon library.
    */
   public void createFolder( String folderName ) throws PhotonException {
      int errcode;

      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      errcode = psoCreateFolder( handle, folderName ); 
      if ( errcode != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(errcode) );
      }
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Create a new object in shared memory as a child of the current folder.
    * <p>
    * This overloaded method should be used for objects not requiring
    * a key definition (queues, etc.).
    * <p>
    * The creation of the object only becomes permanent after a call to 
    * Session.commit.
    * <p>
    * This function does not return a Java object linked to the newly 
    * created object. You must use org.photon.Queue.open and similar to 
    * access the newly created object.
    *
    * @param objectName The name of the newly created object.
    * @param definition The object definition (its type, etc.).
    * @param dataDef    The definition of the data fields.
    * @exception PhotonException On an error with the Photon library.
    */
   public void createObject( String           objectName,
                             ObjectDefinition definition, 
                             DataDefinition   dataDef ) throws PhotonException {
      int errcode;

      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      errcode = psoCreateObject( handle, 
                                 objectName, 
                                 definition,
                                 dataDef );
      if ( errcode != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(errcode) );
      }
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Create a new object in shared memory as a child of the current folder.
    * <p>
    * This overloaded method should be used for objects not requiring
    * a key definition (queues, etc.). It also uses the name of an
    * existing data definition instead of requiring a DataDefinition
    * object.
    * <p>
    * The creation of the object only becomes permanent after a call to 
    * Session.commit.
    * <p>
    * This function does not return a Java object linked to the newly 
    * created object. You must use org.photon.Queue.open and similar to 
    * access the newly created object.
    *
    * @param objectName  The name of the newly created object.
    * @param definition  The object definition (its type, etc.).
    * @param dataDefName The name of the definition of the data fields.
    * @exception PhotonException On an error with the Photon library.
    */
   public void createObject( String           objectName,
                             ObjectDefinition definition, 
                             String           dataDefName ) throws PhotonException {
      int errcode;

      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      errcode = psoCreateObjectEx( handle,
                                   session.handle,
                                   objectName, 
                                   definition,
                                   dataDefName );
      if ( errcode != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(errcode) );
      }
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Create a new object in shared memory as a child of the current folder.
    * <p>
    * This overloaded method should only be used for objects requiring
    * a key definition (maps, etc.).
    * <p>
    * The creation of the object only becomes permanent after a call to 
    * Session.commit.
    * <p>
    * This function does not return a Java object linked to the newly 
    * created object. You must use org.photon.Hashmap and similar to 
    * access the newly created object.
    *
    * @param objectName The name of the newly created object.
    * @param definition The object definition (its type, etc.)
    * @param keyDef     The definition of the key.
    * @param dataDef    The definition of the data fields.
    * @exception PhotonException On an error with the Photon library.
    */
   public void createObject( String           objectName,
                             ObjectDefinition definition, 
                             KeyDefinition    keyDef,
                             DataDefinition   dataDef ) throws PhotonException {
      int errcode;

      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      errcode = psoCreateKeyedObject( handle, 
                                      objectName, 
                                      definition,
                                      keyDef,
                                      dataDef );
      if ( errcode != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(errcode) );
      }
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Create a new object in shared memory as a child of the current folder.
    * <p>
    * This overloaded method should only be used for objects requiring
    * a key definition (maps, etc.). It also uses the names of 
    * both an existing data definition and a key definition instead 
    * of requiring a DataDefinition object and a KeyDefinition object.
    * <p>
    * The creation of the object only becomes permanent after a call to 
    * Session.commit.
    * <p>
    * This function does not return a Java object linked to the newly 
    * created object. You must use org.photon.Hashmap and similar to 
    * access the newly created object.
    *
    * @param objectName  The name of the newly created object.
    * @param definition  The object definition (its type, etc.)
    * @param keyDefName  The definition of the key.
    * @param dataDefName The definition of the data fields.
    * @exception PhotonException On an error with the Photon library.
    */
   public void createObject( String           objectName,
                             ObjectDefinition definition, 
                             String           keyDefName,
                             String           dataDefName ) throws PhotonException {
      int errcode;

      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      errcode = psoCreateKeyedObjectEx( handle, 
                                        session.handle,
                                        objectName, 
                                        definition,
                                        keyDefName,
                                        dataDefName );
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
    *
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

   /**
    * Cleanup the object before GC.
    */
   protected void finalize() throws Throwable {     
      
      try {
         psoClose(handle);
      } finally {
         handle = 0;
         super.finalize();
      }
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public DataDefinition getDataDefinition( String objectName ) throws PhotonException {

      int errcode;
      DataDefinition definition;
      
      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }
      definition = new DataDefinition();
      
      errcode = psoDataDefinition( handle, objectName, definition );
      if ( errcode != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(errcode) );
      }
      
      return definition;
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public ObjectDefinition getDefinition( String objectName ) throws PhotonException {

      int errcode;
      ObjectDefinition definition;
      
      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }
      definition = new ObjectDefinition();
      
      errcode = psoDefinition( handle, objectName, definition );
      if ( errcode != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(errcode) );
      }
      
      return definition;
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public KeyDefinition getKeyDefinition( String objectName ) throws PhotonException {

      int errcode;
      KeyDefinition definition;
      
      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }
      definition = new KeyDefinition();
      
      errcode = psoKeyDefinition( handle, objectName, definition );
      if ( errcode != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(errcode) );
      }
      
      return definition;
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

   /**
    * Implement the Iterator interface.
    * <p>
    * The three methods, hasNext, next and remove implement Iterator.
    */
   public boolean hasNext() {

      /*
       * Note: We cannot implement hasNext directly. Technically, we could 
       * but since the content of a Folder is shared, the next item might 
       * be removed by the time we call next(). What we do instead is to 
       * get the next item into this.entry and set nextWasQueried to true. 
       * In other words, we cache it.
       */

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

   /** This method implements the Iterable interface */
   public Iterator<FolderEntry> iterator() {
      return this;
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Implement the Iterator interface.
    * <p>
    * The three methods, hasNext, next and remove implement Iterator.
    */
   public FolderEntry next() {
      
      /*
       * hasNext() may cache the next item. See hasNext() for more details.
       */
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

   /** 
    * Open an existing folder. 
    * <p>
    * If the folder is already open, you must close it first.
    *
    * @param session The session we belong to.
    * @param folderName The fully qualified name of the folder. 
    * @exception PhotonException On an error with the Photon library.
    */
   public void open( Session session, String folderName ) throws PhotonException {

      int errcode;
      
      if ( handle != 0 ) {
         throw new PhotonException( PhotonErrors.ALREADY_OPEN );
      }

      errcode = psoOpen( session, folderName );
      if ( errcode != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(errcode) );
      }
      this.session = session;
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Implement the Iterator interface.
    * <p>
    * The three methods, hasNext, next and remove implement Iterator.
    * <p>
    * Note: remove() will throw an UnsupportedOperation Exception
    * since this operation is not currently supported.
    */
   public void remove() {
      throw new UnsupportedOperationException();
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   private static native void initIDs();

   private native int psoCreateFolder( long   handle,
                                       String objectName );

   private native int psoCreateObject( long             handle,
                                       String           objectName, 
                                       ObjectDefinition definition, 
                                       DataDefinition   dataDef );

   private native int psoCreateObjectEx( long             handle,
                                         long             sessionHandle,
                                         String           objectName, 
                                         ObjectDefinition definition, 
                                         String           dataDefName );

   private native int psoCreateKeyedObject( long             handle,
                                            String           objectName, 
                                            ObjectDefinition definition, 
                                            KeyDefinition    keyDef,
                                            DataDefinition   dataDef );

   private native int psoCreateKeyedObjectEx( long             handle,
                                              long             sessionHandle,
                                              String           objectName, 
                                              ObjectDefinition definition, 
                                              String           keyDefName,
                                              String           dataDefName );

   private native int psoDataDefinition( long           handle, 
                                         String         objectName,
                                         DataDefinition definition );

   private native int psoDefinition( long             handle,
                                     String           objectName,
                                     ObjectDefinition definition );
   
   private native int psoDestroyObject( long handle, String objectName );

   private native void psoClose( long handle );

   private native int psoGetFirst( long handle, FolderEntry e );

   private native int psoGetNext( long handle, FolderEntry e );

   private native int psoKeyDefinition( long          handle,
                                        String        objectName,
                                        KeyDefinition definition );

   private native int psoOpen( Session session, String name );

   private native int psoStatus( long handle, ObjectStatus status );

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
}

