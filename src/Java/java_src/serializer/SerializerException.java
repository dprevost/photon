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

package org.photon.serializer;

/**
 * Exception class for the Photon library.
 */

public class SerializerException extends Exception {

   private SerializerErrors theErrorCode;
   
   /**
    * This can be used for debugging purposes or for doing unit tests.
    */
   public SerializerErrors getErrorCode() { return theErrorCode; }
        
   public SerializerException( SerializerErrors errcode ) { 
      super( errcode.getMessage() );

      theErrorCode = errcode; 
   }
   
   /**
    * This constructor encapsulates other exceptions.
    * <p>
    * The basic idea was to avoid declaring the interface PSOSerialize (and
    * the classes that implement it) throwing a generic Exception. This
    * way, any non-SerializerException is not expected and must be 
    * examined carefully.
    * <p>
    * For debugging purposes, the type of exception is captured in the
    * error code and can be examined using getErrorCode().
    */
   public SerializerException( Throwable cause, SerializerErrors errcode ) { 
      super( cause );

      theErrorCode = errcode; 
   }
}
