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

package org.photon.serializer;

import org.photon.*;

public class StringSerializer implements PSOSerialize<String> {
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /** Pointer to the data definition. */
   private byte[] dataDef;

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   static {
      initIDs();
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
   
   public StringSerializer( byte[] dataDef ) throws SerializerException {
      
      int errcode = psoValidate( dataDef );
      if ( errcode != 0 ) {
         throw new SerializerException( SerializerErrors.getEnum(errcode) );
      }
      
      this.dataDef = dataDef;
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public byte[] packObject( String obj ) throws SerializerException {

      byte [] data = psoPackObject( obj );
      
      return data;
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public void setDefinition( byte[] dataDef ) throws SerializerException { 
      
      int errcode = psoValidate( dataDef );
      if ( errcode != 0 ) {
         throw new SerializerException( SerializerErrors.getEnum(errcode) );
      }
      
      this.dataDef = dataDef;
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public String unpackObject( byte[] buffer ) throws SerializerException {

      String obj = psoUnpackObject( buffer );
      
      
      return obj;
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   private static native void initIDs();

   private native  byte[] psoPackObject( String obj ) throws SerializerException;
   
   private native String psoUnpackObject( byte[] buffer ) throws SerializerException;

   private native int psoValidate( byte[] dataDef );
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
}