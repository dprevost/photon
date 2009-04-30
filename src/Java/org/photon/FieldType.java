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
   
   CHAR(101),

   VARCHAR(102),

   LONGVARCHAR(103),
   
   /** A one-byte integer. */
   TINYINT(104),
   
   /** A two-bytes integer. */
   SMALLINT(105),
   
   /** A four-bytes integer. */
   INTEGER(106),
   
   BIGINT(107),

   /** The decimal type should be mapped to the psoNumericStruct struct. */
   NUMERIC(108),
   
   REAL(109),

   DOUBLE(110),
   
   /** An opaque type of fixed length. */
   BINARY(111),

   /** A variable length opaque type with a maximum length. */
   VARBINARY(112),
   
   /**
    * A variable length opaque type with no maximum length constraint.
    *
    * The length of that field is constrained by the maximum length
    * of a data record.
    */
   /** Only valid for the last field of the data definition */
   LONGVARBINARY(113),
   
   DATE(114),

   TIME(115),

   TIMESTAMP(116);

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   private int type;
   
   FieldType( int type ) { this.type = type; }
   
   public int getType() { return type; }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   private static final HashMap<Integer,FieldType> reverseLookup 
                  = new HashMap<Integer,FieldType>();

   static {
      for ( FieldType myType : FieldType.values() ) {
         reverseLookup.put( myType.getType(), myType );
      }
   }

   public static FieldType getEnum(int type) {
      return reverseLookup.get(type);
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
}
