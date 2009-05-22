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

package org.photon.Tests;

import org.photon.*;

public class Test {

   public static void main( String args[] ) {
      
      System.out.println("Total Memory "+Runtime.getRuntime().totalMemory());    
      System.out.println("Free Memory "+Runtime.getRuntime().freeMemory());
       
      Session session;

      System.gc();
      System.out.println("Free Memory "+Runtime.getRuntime().freeMemory());
      
      try {
         Photon.init( "10701", "Test-Java" );

         SessionTest.tests();
         
         session = new Session();

         FolderTest.test1( session );
         FolderTest.test2( session );
         FolderTest.createFolders( session );
         
         /* The GC might not call finalize() - safer to cleanup ourselves */
         Photon.exit();

      } catch ( PhotonException e ) {
         e.printStackTrace();
         Photon.exit();
//         System.exit(1);
      }
      System.out.println("Free Memory "+Runtime.getRuntime().freeMemory());
      System.gc();
      System.out.println("Free Memory "+Runtime.getRuntime().freeMemory());
//      System.runFinalization();
//      System.gc();
      System.out.println("Free Memory "+Runtime.getRuntime().freeMemory());
      
   }
}

