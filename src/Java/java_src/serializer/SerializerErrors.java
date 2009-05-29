/*
 * Copyright (C) 2009 Daniel Prevost
 *
 * This file is part of Photon (photonsoftware.org).
 *
 * This file may be distributed and/or modified under the terms of the 
 * GNU General Public License version 2 or version 3 as published by 
 * the Free Software Foundation and appearing in the file COPYING.GPL2 
 * and COPYING.GPL3 included in the packaging of this software.
 *
 * Licensees holding a valid Photon Commercial license can use this 
 * file in accordance with the terms of their license.
 *
 * This software is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

package org.photon.serializer;

import java.util.*;

public enum SerializerErrors {

    // No error...
   OK(0) {
      String getMessage() { return "No error..."; } },

   IO_EXCEPTION(1) {
      String getMessage() { return "The exception IOException was thrown."; } },

   CLASS_NOT_FOUND_EXCEPTION(2) {
      String getMessage() { return "The exception ClassNotFoundException was thrown."; } },

   INVALID_NUM_FIELDS_IN_DATA_DEF(3) {
      String getMessage() { return "The number of fields in the data definition is incorrect."; } };

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   private final int errcode;

   SerializerErrors(int errcode) { 
      this.errcode = errcode;
   }

   public int getErrorNumber() { return errcode; }

   abstract String getMessage();

   private static final HashMap<Integer,SerializerErrors> reverseLookup = new HashMap<Integer,SerializerErrors>();

   static {
      for ( SerializerErrors err : SerializerErrors.values() ) {
         reverseLookup.put( err.getErrorNumber(), err );
      }
   }

   public static SerializerErrors getEnum(int errcode) {
      return reverseLookup.get(errcode);
  }

}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

