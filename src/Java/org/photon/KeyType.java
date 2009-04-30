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

public enum KeyType {
   
   KEY_CHAR(1001),
   
   KEY_VARCHAR(1002),

   KEY_LONGVARCHAR(1003),
   
   /** A four-bytes integer. */
   KEY_INTEGER(1004),
   
   KEY_BIGINT(1005),
   
   /** An opaque type of fixed length. */
   KEY_BINARY(1006),

   /** A variable length opaque type with a maximum length. */
   KEY_VARBINARY(1007),
   
   /**
    * A variable length opaque type with no maximum length constraint.
    *
    * The length of that field is constrained by the maximum length
    * of a data record.
    */
   /** Only valid for the last field of the data definition */
   KEY_LONGVARBINARY(1008),
   
   KEY_DATE(1009),

   KEY_TIME(1010),

   KEY_TIMESTAMP(1011);

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   private int type;
   
   KeyType( int type ) { this.type = type; }
   
   public int getType() { return type; }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   private static final HashMap<Integer,KeyType> reverseLookup 
                  = new HashMap<Integer,KeyType>();

   private static native void initIDs();

   static {
      for ( KeyType myType : KeyType.values() ) {
         reverseLookup.put( myType.getType(), myType );
      }
      initIDs();
   }

   public static KeyType getEnum(int type) {
      return reverseLookup.get(type);
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
}

