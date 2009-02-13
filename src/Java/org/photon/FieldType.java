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

import java.util.*;

public enum FieldType {
   
   INTEGER(1),
   BINARY(2),
   STRING(3),
   /** The decimal type should be mapped to an array of bytes of length
    *  precision + 2 (optional sign and the decimal separator).
    */
   DECIMAL(4),
   /** The boolean type should be mapped to a single byte in a C struct. */
   BOOLEAN(5),
   /** Only valid for the last field of the data definition */
   VAR_BINARY(6),
   /** Only valid for the last field of the data definition */
   VAR_STRING(7);
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   private int type;
   
   FieldType( int type ) { this.type = type; }
   
   public int getType() { return type; }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   private static final HashMap<Integer,FieldType> reverseLookup 
                  = new HashMap<Integer,FieldType>();

   private static native void initIDs();

   static {
      for ( FieldType myType : FieldType.values() ) {
         reverseLookup.put( myType.getType(), myType );
      }
      initIDs();
   }

   public static FieldType getEnum(int type) {
      return reverseLookup.get(type);
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
}
