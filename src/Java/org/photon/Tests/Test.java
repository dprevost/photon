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

package org.photon.Tests;

import org.photon.*;

public class Test {

   public static void main( String args[] ) {
      
      PhotonProcess process;
      PhotonSession session;
      
      try {
         process = new PhotonProcess( "10701" );

         Session.tests();
         
         session = new PhotonSession();

         Folder.test1( session );
         Folder.test2( session );
         Folder.createFolders( session );
         
         /* The GC might not call finalize() - safer to cleanup ourselves */
         process.exit();

      } catch ( PhotonException e ) {
         e.printStackTrace();
         System.exit(1);
      }
      
   }
}

