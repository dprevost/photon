/*
 * Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
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
 * Exception class for the Photon library.
 */

class PsoException extends Exception {

   private int theErrorCode;
   
   public int getErrorCode() { return theErrorCode; }
        
   public PsoException( String msg, int errcode ) { 
      super(msg);

      theErrorCode = errcode; 

      msg = getErrorMessage( errcode );
      
   }

   /*
    * This static function uses the Photon API to extract the error message
    * and pass it to the constructor.
    */
   public static String PrepareException( String functionName, int errcode ) {

      String str = getErrorMessage( errcode );

      if (str == null || str.length() == 0) {
         str = functionName
               + " exception: Cannot retrieve the error message - the error code is "
               + errcode;
      }
      return str;
   }

   private static native String getErrorMessage( int errcode );
}
