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

/**
 * Queue class for the Photon library.
 */
// getDeclaredFields

public class Queue<DataRecord> implements Iterable<DataRecord>, Iterator<DataRecord> {

   /* To save the native pointer/handle of the C struct. */
   private long handle = 0;
   private Session session;
   private Definition definition;
   
   /* For iterations */
   DataRecord record;

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
         nextWasQueried = getNextRecord();
      } catch (PhotonException e) {
         return false;
      }
      
      return nextWasQueried;
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public DataRecord next() {
      
      nextWasQueried = false;
      
      if ( nextWasQueried ) { return record; }
      
      try {
         if ( getNextRecord() ) {
            return record;
         }
      } catch (PhotonException e) {}

      throw new NoSuchElementException();
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public void remove() {
      throw new UnsupportedOperationException();
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /** This method implements the Iterable interface */
   public Iterator<DataRecord> iterator() {
      return this;
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public Queue( Session session, String name ) throws PhotonException {
      
      int errcode;
      /* Simplify the jni by preallocating some objects */
      ObjectDefinition objectDef = new ObjectDefinition();
      this.definition = new Definition();
      
      this.session = session;
      
      errcode = psoInit( session, name, definition, objectDef );
      if ( errcode != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(errcode) );
      }
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   private static native void initIDs();

   static {
      initIDs();
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   private native void psoFini( long handle );

   private native int psoGetFirst( long       handle,
                                   DataRecord record );

   private native int psoGetNext( long       handle,
                                  DataRecord record );

   private native int psoGetStatus( long         handle,
                                    ObjectStatus status );

   private native int psoInit( Session          session,
                               String           queueName,
                               Definition       def,
                               ObjectDefinition objDef );

   private native int psoPop( long handle, DataRecord record );

//   private native int psoPush( long handle, DataRecord record );

   private native int psoPush( long handle, 
                               int numFields, 
                               Object[] objects );

   private native int psoPushNow( long handle, DataRecord record );

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

   public void close() { 
      psoFini(handle);
      handle = 0;
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public Definition getDefinition() throws PhotonException {

      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      // We get the definition when accessing/opening the queue - no need
      // to get it again, evidently.
      return definition;
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   boolean getNextRecord() throws PhotonException {
      
      int errcode;
      
      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      if ( endIteration ) {
         endIteration = false;
         errcode = psoGetFirst( handle, record );
      } else {
         errcode = psoGetNext( handle, record );
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

   public ObjectStatus getStatus() throws PhotonException {
      
      int errcode;
      ObjectStatus status = new ObjectStatus();
      
      errcode = psoGetStatus( handle, status );
      if ( errcode == 0 ) return status;

      throw new PhotonException( PhotonErrors.getEnum(errcode) );
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public void pop( DataRecord record ) throws PhotonException {
      
      int errcode;
      
      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      errcode = psoPop( handle, record );
      if ( errcode == 0 ) return;

      throw new PhotonException( PhotonErrors.getEnum(errcode) );
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

//   public void push( DataRecord qrecord ) throws PhotonException {

//      int errcode;
      
 //     push( 45, "test", 333, "QQQ" );
//      java.lang.Object[] q = qrecord.QQQ();
//      if ( handle == 0 ) {
//         throw new PhotonException( PhotonErrors.NULL_HANDLE );
//      }

//      errcode = psoPush( handle, 
//                         definition.definition.numFields,
//                         q );
 //     if ( errcode == 0 ) return;

 //     throw new PhotonException( PhotonErrors.getEnum(errcode) );
//   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public void push( Object ... objects ) throws PhotonException {

      int errcode;
      
      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      errcode = psoPush( handle, 
                         definition.definition.numFields,
                         objects );
      if ( errcode == 0 ) return;

      throw new PhotonException( PhotonErrors.getEnum(errcode) );
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public void pushNow( DataRecord record ) throws PhotonException {

      int errcode;
      
      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      errcode = psoPushNow( handle, record );
      if ( errcode == 0 ) return;

      throw new PhotonException( PhotonErrors.getEnum(errcode) );
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
}

