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

public enum DefinitionType {
   
   USER_DEFINED(3),
   /**
    * A simplified version of ODBC. 
    * 
    * The fields must all have a fixed length except for the last one. 
    * This condition makes it easy to map the data record with a C struct.
    */
   PHOTON_ODBC_SIMPLE(4);

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   private int type;
   
   DefinitionType( int type ) { this.type = type; }
   
   public int getType() { return type; }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   private static final HashMap<Integer,DefinitionType> reverseLookup 
                  = new HashMap<Integer,DefinitionType>();

   private static native void initIDs();

   static {
      for ( DefinitionType myType : DefinitionType.values() ) {
         reverseLookup.put( myType.getType(), myType );
      }
      initIDs();
   }

   public static DefinitionType getEnum(int type) {
      return reverseLookup.get(type);
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
}
