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
public class Queue<T extends PSOSerialize> extends RawQueue implements Iterable<PSOSerialize>, Iterator<PSOSerialize> {

   /* For iterations */
   T dataBuffer;

   private boolean nextWasQueried = false;
   private boolean endIteration = true;

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public Queue() { super(); }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public Queue( Session session, String name ) throws PhotonException {
      
      super (session, name );
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

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

   public T next() {
      
      nextWasQueried = false;
      
      if ( nextWasQueried ) { return dataBuffer; }
      
      try {
         if ( getNextRecord() ) {
            return dataBuffer;
         }
      } catch (PhotonException e) {}

      throw new NoSuchElementException();
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   boolean getNextRecord() throws PhotonException {
      
      int errcode;
      byte[] buffer = null;
      
      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      if ( endIteration ) {
         endIteration = false;
         errcode = psoGetFirst( handle, buffer );
      } else {
         errcode = psoGetNext( handle, buffer );
      }
      if ( errcode == 0 ) {
         dataBuffer.unpackObject( buffer );
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

   /** This method implements the Iterable interface */
   public Iterator<PSOSerialize> iterator() {
      return this;
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public void pop( T record ) throws PhotonException {
      
      int errcode;
      byte [] buffer = null;
      
      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      errcode = psoPop( handle, buffer );
      record.unpackObject( buffer );
      
      if ( errcode == 0 ) return;

      throw new PhotonException( PhotonErrors.getEnum(errcode) );
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public void push( T record ) throws PhotonException {

      int errcode;
      
      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      errcode = psoPush( handle, record.packObject() );
      if ( errcode == 0 ) return;

      throw new PhotonException( PhotonErrors.getEnum(errcode) );
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public void pushNow( T record ) throws PhotonException {

      int errcode;
      
      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      errcode = psoPushNow( handle, record.packObject() );
      if ( errcode == 0 ) return;

      throw new PhotonException( PhotonErrors.getEnum(errcode) );
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public void remove() {
      throw new UnsupportedOperationException();
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

//   private static native void initIDs();

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
}

