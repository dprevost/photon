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

public class KeyODBC {

   /** The data type of the key. */
   private KeyType type;

   /** For fixed-length data types */
   private int length;

   /** For variable-length data types */
   private int minLength;

   /** For variable-length data types */
   private int maxLength;
 
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /** The data type of the key (integer, string, etc). */
   public KeyType getType() { return type; }
   
   /** The length of the key for fixed length data types. */
   public int     getLength()    { return length; }

   /** The minimum length of the key for variable length data types. */
   public int     getMinLength() { return minLength; }

   /** The maximum length of the key for variable length data types. */
   public int     getMaxLength() { return maxLength; }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   KeyODBC() {}

   public KeyODBC( KeyType type, int length, int minLength, int maxLength ) {
      this.type = type;
      this.length = length;
      this.minLength = minLength;
      this.maxLength = maxLength;
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   private static native void initIDs();

   static {
      initIDs();
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
}
