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

/**
 * Container class for the Photon library. This class is the base
 * class of all Photon containers.
 */
// getDeclaredFields
//  [Lorg/photon/DataRecord$FieldDescription;


public class DataRecord {
   
   class FieldDescription {
      Object obj;
      int    conversionType;
      int    offset;
   }
   
   private FieldDescription[] fields;

   public Boolean getBool  ( int i ) { return (Boolean) fields[i].obj; }
   public byte[]  getBytes ( int i ) { return  (byte[]) fields[i].obj; }
   public Integer getInt   ( int i ) { return (Integer) fields[i].obj; }
   public Long    getLong  ( int i ) { return    (Long) fields[i].obj; }
   public Short   getShort ( int i ) { return   (Short) fields[i].obj; }
   public String  getString( int i ) { return  (String) fields[i].obj; }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   private static native void initIDs();

   static {
      initIDs();
   }
}