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

class KeyDefinition {

   /** The data type of the key. */
   private int type;

   /** For fixed-length data types */
   private int length;

   /** For variable-length data types */
   private int minLength;

   /** For variable-length data types */
   private int maxLength;
 
   public int getType() { return type; }
   public int getLength() { return length; }
   public int getMinLength() { return minLength; }
   public int getMaxLength() { return maxLength; }

   KeyDefinition( int type, int length, int minLength, int maxLength ) {
      this.type = type;
      this.length = length;
      this.minLength = minLength;
      this.maxLength = maxLength;
   }
}