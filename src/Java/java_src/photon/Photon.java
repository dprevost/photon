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

public class Photon {

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   static {
      System.loadLibrary("photon_jni");
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * This function initializes access to the shared-memory of Photon.
    * <p>
    * It takes a single argument, the address of the shared memory;
    * either the directory of the shared-memory backstore or the 
    * port number of Quasar, the Photon server.
    * 
    * @param psoAddress The address of Quasar. Currently a string with 
    *                   the port number (example "12345"). Or the name of 
    *                   the directory of the shared-memory backstore.
    * @exception PhotonException On an error with the Photon library.
    */
   public static void init( String psoAddress, String processName ) throws PhotonException {
      
      int errcode;
      
      errcode = psoInit( psoAddress, processName );
      if ( errcode != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(errcode) );
      }
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * This function terminates all accesses to Photon shared memory.
    * <p>
    * This function will also close all C sessions and terminate all 
    * accesses to the different C objects. 
    * <p>
    * This function takes no argument and always end successfully.
    */
   public static void exit(){
      psoFini();
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   private static native int psoInit( String psoAddress, String processName );

   private static native void psoFini();
}

