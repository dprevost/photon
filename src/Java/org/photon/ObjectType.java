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

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

public enum ObjectType {

   /*
    * Warning for maintainers: the jni must be updated if the enum values
    * are modified (for example if LIFO -> LIFO_QUEUE or new ones are 
    * added. Explanation: the values are hardcoded to generate weak 
    * references to the static instances of this class.
    */
   FOLDER(1) {
      public String getText() { return "Folder"; } },

   HASH_MAP(2) {
      public String getText() { return "Hash Map"; } },

   LIFO(3) {
      public String getText() { return "LIFO Queue"; } },

   FAST_MAP(4) {
      public String getText() { return "Read-only Hash Map"; } },

   QUEUE(5) {
      public String getText() { return "FIFO Queue"; } };

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   private final int type;

   ObjectType( int type ) { 
      this.type = type;
   }

   public int getType() { return type; }

   public abstract String getText();

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   private static final HashMap<Integer,ObjectType> reverseLookup 
                  = new HashMap<Integer,ObjectType>();

   private static native void initIDs();

   static {
      for ( ObjectType myType : ObjectType.values() ) {
         reverseLookup.put( myType.getType(), myType );
      }
      initIDs();
   }

   public static ObjectType getEnum(int type) {
      return reverseLookup.get(type);
   }

}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
