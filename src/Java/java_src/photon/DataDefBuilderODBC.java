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

public class DataDefBuilderODBC {
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /// The C struct array holding the definition of the fields
   private byte[] fields;

   /// Number of fields in the definition
   private int numFields;

   /// Iterator
   private int currentField = 0;

   /// true if the definition type is odbc simple, false otherwise.
   private boolean simpleDef;
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   static {
      initIDs();
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Construct a data-container definition.
    *
    * Use this constructor to build a definition to create new Photon objects.
    *
    * @param numFields The number of data fields.
    * @param simple Set this to true if the definition type is "simple"
    *               (fixed length fields) or false if not.
    *
    * @exception PhotonException An abnormal error with the Photon library.
    */
   public DataDefBuilderODBC( int     numFields,
                              boolean simple ) throws PhotonException {
       
      if ( numFields <= 0 ) {
         throw new PhotonException( PhotonErrors.INVALID_NUM_FIELDS );
      }
      this.numFields = numFields;
      this.simpleDef = simple;
       
      fields = new byte[numFields*psoGetLength()];
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Add a field to the definition. Fields must be added in order.
    *
    * @param name The name of the field
    * @param type The data type of the field. 
    * @param length The length of the key (needed for some data types).
    * @param precision The number of digits of a numeric field (not 
    *              counting the separator or the sign).
    * @param scale The number of digits after the separator of a numeric
    *              field.
    *
    * @exception PhotonException An abnormal error with the Photon library.
    */
   public void AddField( String    name,
                         FieldType type,
                         int       length,
                         int       precision,
                         int       scale ) throws PhotonException {
   
      int errcode;
      
      if ( currentField >= numFields ) {
         throw new PhotonException( PhotonErrors.INVALID_NUM_FIELDS );
      }
      
      errcode = psoAddField( name,
                             type.getType(),
                             length,
                             precision,
                             scale );
      if ( errcode != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(errcode) );
      }
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

   private native int psoAddField( String name,
                                   int    type,
                                   int    length,
                                   int    precision,
                                   int    scale );
   
   private native int psoGetLength();
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

}