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

public class KeyDefBuilderODBC {
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /// The C struct array holding the definition of the fields
   private byte[] keyFields;

   /// Number of fields in the definition
   private int numKeyFields;

   /// Iterator
   private int currentKeyField = 0;

   /// true if the definition type is odbc simple, false otherwise.
   private boolean simpleDef;
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   static {
      initIDs();
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Construct an ODBC-like key definition.
    *
    * Use this constructor to build a key definition based on ODBC.
    *
    * @param numKeyFields The number of fields in the key.
    * @param simple Set this to true if the definition type is "simple"
    *               (fixed length fields) or false if not.
    *
    * @exception PhotonException An abnormal error with the Photon library.
    */
   public KeyDefBuilderODBC( int     numKeyFields,
                             boolean simple ) throws PhotonException {
       
      if ( numKeyFields <= 0 ) {
         throw new PhotonException( PhotonErrors.INVALID_NUM_FIELDS );
      }
      this.numKeyFields = numKeyFields;
      this.simpleDef    = simple;
       
      keyFields = new byte[numKeyFields*psoGetLength()];
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Add a key field to the definition. Key fields must be added in order.
    *
    * @param name The name of the key field.
    * @param type The data type of the key field. 
    * @param length The length of the key (needed for some data types).
    *
    * @exception PhotonException An abnormal error with the Photon library.
    */
   public void AddKeyField( String  name,
                            KeyType type,
                            int     length ) throws PhotonException {
   
      int errcode;
      
      if ( currentKeyField >= numKeyFields ) {
         throw new PhotonException( PhotonErrors.INVALID_NUM_FIELDS );
      }
      
      errcode = psoAddKeyField( name,
                                type.getType(),
                                length );
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

      if ( currentKeyField != numKeyFields ) {
         throw new PhotonException( PhotonErrors.INVALID_NUM_FIELDS );
      }
      
      return keyFields;
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   private static native void initIDs();

   private native int psoAddKeyField( String name,
                                      int    type,
                                      int    length );
   
   private native int psoGetLength();
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

}