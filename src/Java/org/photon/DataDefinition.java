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

public class DataDefinition implements Iterable<String>, Iterator<String> {
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /** To save the native pointer/handle of the C struct. */
   long handle = 0;

   /** The name of the data definition. */
   private String name;
   
   /** The session we belong to. */
   private Session session;

   /** The definition type of the data definition. */
   private int type;

   /** Pointer to the actual data definition. */
   private byte[] dataDef;
   
   /** Iterator */
   private int currentLength = 0;

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   static {
      initIDs();
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Default constructor.
    * <p>
    * You must use open() or create to access a data definition in 
    * shared memory.
    */
   public DataDefinition() {}
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Creates a new data definition in shared memory.
    *
    * @param session The session we belong to.
    * @param name The name of the definition.
    * @param type The type of definition (ODBC, user defined, etc.)
    * @param dataDef The data definition (as an opaque type)
    *
    * @exception PhotonException An abnormal error with the Photon library.
    */
   public DataDefinition( Session        session,
                          String         name,
                          DefinitionType type,
                          byte[]         dataDef ) throws PhotonException {
   
      int errcode;
      
      errcode = psoCreate( session.handle,
                           name,
                           type.getType(),
                           dataDef,
                           dataDef.length );
      if ( errcode != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(errcode) );
      }

      this.name    = name;
      this.session = session;
      this.dataDef = dataDef;
      this.type    = type.getType();
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Opens an existing data definition in shared memory.
    *
    * @param session The session we belong to.
    * @param name The name of the definition.
    *
    * @exception PhotonException An abnormal error with the Photon library.
    */
   public DataDefinition( Session session,
                          String  name ) throws PhotonException {
   
      int errcode;
      
      errcode = psoOpen( session.handle, name );
      if ( errcode != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(errcode) );
      }

      this.name = name;
      this.session = session;
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Closes our access to the data definition in shared memory.
    *
    * @exception PhotonException An abnormal error with the Photon library.
    */
   public void close() throws PhotonException {
      
      int errcode;
      
      errcode = psoClose( handle );
      if ( errcode != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(errcode) );
      }
      
      handle = 0;
      name = "";
      currentLength = 0;
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Creates a new data definition in shared memory.
    *
    * @param name The name of the definition.
    * @param type The type of definition (ODBC, user defined, etc.)
    * @param dataDef The data definition (as an opaque type)
    *
    * @exception PhotonException An abnormal error with the Photon library.
    */
   public void create( String         name,
                       DefinitionType type,
                       byte[]         dataDef ) throws PhotonException {
   
      int errcode;
      
      if ( handle != 0 ) {
         throw new PhotonException( PhotonErrors.ALREADY_OPEN );
      }
      
      errcode = psoCreate( session.handle,
                           name,
                           type.getType(),
                           dataDef,
                           dataDef.length );
      if ( errcode != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(errcode) );
      }

      this.name    = name;
      this.dataDef = dataDef;
      this.type    = type.getType();
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

   public byte[] getDefinition() throws PhotonException {

      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }
      return dataDef;
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public int getLength() throws PhotonException {

      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }
      return dataDef.length;
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public String getName() throws PhotonException {

      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }
      return name;
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public DefinitionType getType() throws PhotonException {

      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }
      return DefinitionType.getEnum(type);
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Implement the Iterator interface.
    * <p>
    * The three methods, hasNext, next and remove implement Iterator.
    */
   public boolean hasNext() {

      if ( currentLength < dataDef.length )
         return true;
      
      return false;
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /** This method implements the Iterable interface */
   public Iterator<String> iterator() {
      return this;
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Implement the Iterator interface.
    * <p>
    * The three methods, hasNext, next and remove implement Iterator.
    */
   public String next() {
      
      if ( currentLength >= dataDef.length ) {
         currentLength = 0;
         throw new NoSuchElementException();
      }
      
      return psoGetNext();
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Opens an existing data definition in shared memory.
    *
    * @param session The session we belong to.
    * @param name The name of the definition.
    *
    * @exception PhotonException An abnormal error with the Photon library.
    */
   public void open( Session session, String name ) throws PhotonException {
   
      int errcode;
      
      errcode = psoOpen( session.handle, name );
      if ( errcode != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(errcode) );
      }

      this.name = name;
      this.session = session;
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Implement the Iterator interface.
    * <p>
    * The three methods, hasNext, next and remove implement Iterator.
    * <p>
    * Note: remove() will throw an UnsupportedOperation Exception
    * since this operation is not supported.
    */
   public void remove() {
      throw new UnsupportedOperationException();
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   private static native void initIDs();

   private native int psoClose( long h );

   private native int psoCreate( long    hSession,
                                 String  name,
                                 int     type,
                                 byte [] dataDef,
                                 int     dataDefLength );

   private native String psoGetNext();
   
   private native int psoOpen( long hSession, String name );

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
}

