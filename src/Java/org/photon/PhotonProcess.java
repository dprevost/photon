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

public class PhotonProcess {

   static {
      System.loadLibrary("photon_jni");
   }

   public PhotonProcess( String psoAddress ) throws PhotonException {
      
      init( psoAddress );
   }
   
   public void exit(){
      fini();
   }

   protected void finalize() {
      fini();
   }
   
   private native void init( String psoAddress ) throws PhotonException;

   private native void fini();
}

