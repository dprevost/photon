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

public class FolderTest {

   
   public static void test1( org.photon.Session session ) throws PhotonException {
      
      org.photon.Folder folder = new org.photon.Folder();

      folder.open( session, "/" );
      folder.close();
   }

   public static void test2( org.photon.Session session ) throws PhotonException {
      
      org.photon.Folder folder = new org.photon.Folder( session, "/" );
   }
   
   public static void createFolders( org.photon.Session session ) throws PhotonException {

      org.photon.Folder folder = new org.photon.Folder( session, "/" );
      
      folder.createFolder( "java1" );
      folder.createFolder( "java2" );
      folder.createFolder( "java3" );

      for (FolderEntry entry : folder) {
         System.out.println( "Name: " + entry.getName() + ", Type: " 
            + entry.getType() + " or " + entry.getType().getText() );
      }
   }

}
