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

class PhotonQueue<DataRecord> implements Iterable<DataRecord>, Iterator<DataRecord> {

   /** To save the native pointer/handle. */
   private long handle = 0;
   /** For iterations */
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

   // And this method implements Iterable   
   public Iterator<DataRecord> iterator() {
      return this;
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public PhotonQueue( PhotonSession session, String name ) throws PhotonException {
      
      int rc;
      
      rc = init( session, name );
      if ( rc != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(rc) );
      }
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   private static native void initIDs();

   static {
      initIDs();
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   private native int fini( long handle );
   private native int getDefinition( long              handle, 
                                     ObjectDefinition  definition,
                                     int               numFields,
                                     FieldDefinition[] fields );
   private native int getFirst( long       handle,
                                DataRecord record );

   private native int getNext( long       handle,
                               DataRecord record );

   private native int init( PhotonSession session, String queueName );

   private native int pop( long   handle,
                           byte[] buffer,
                           int    bufferLength,
                           int[]  returnedLength );

   private native int push( long   handle, 
                            byte[] pItem, 
                            int    length );

   private native int pushNow( long   handle, 
                               byte[] pItem, 
                               int    length );
   private native int getStatus( long      handle,
                                 ObjStatus status );

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   boolean getNextRecord() throws PhotonException {
      
      int rc;
      
      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      if ( endIteration ) {
         endIteration = false;
         rc = getFirst( handle, record );
      } else {
         rc = getNext( handle, record );
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

}
