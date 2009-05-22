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

public class DataDefBuilderUser {
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /// The C struct array holding the definition of the fields
   private byte[] fields;

   private int numFields;
   
   /// Iterator
   private int currentField = 0;

   /// Current length of the buffer
   private int currentLength = 0;
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   static {
      initIDs();
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Construct a data-container definition.
    *
    * Use this constructor to build a definition to create new Photon objects.
    */
   public DataDefBuilderUser( int numFields ) throws PhotonException {

      if ( numFields <= 0 ) {
         throw new PhotonException( PhotonErrors.INVALID_NUM_FIELDS );
      }
      this.numFields = numFields;
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Add a field to the definition. Fields must be added in order.
    *
    * @param fieldDescription Your description/definition of the field.
    *
    * @exception PhotonException An abnormal error with the Photon library.
    */
   public void AddField( String fieldDescription ) throws PhotonException {
   
      int errcode;
            
      if ( currentField >= numFields ) {
         throw new PhotonException( PhotonErrors.INVALID_NUM_FIELDS );
      }

      if ( fieldDescription.length() == 0 ) {
         throw new PhotonException( PhotonErrors.INVALID_LENGTH );
      }

      errcode = psoAddField( fieldDescription );
      if ( errcode != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(errcode) );
      }
      currentField++;
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /** 
    * Retrieve the fields definition.
    *
    * @return The definition of the fields as an array of bytes.
    */
   public byte[] GetDefinition() throws PhotonException {

      if ( currentField != numFields ) {
         throw new PhotonException( PhotonErrors.INVALID_NUM_FIELDS );
      }
      
      return fields;
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   private static native void initIDs();

   private native int psoAddField( String fieldDescription );
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

}