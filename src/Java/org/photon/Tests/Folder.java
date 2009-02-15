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

public class Folder {

   
   public static void test1( PhotonSession session ) throws PhotonException {
      
      PhotonFolder folder = new PhotonFolder( session );

      folder.open( "/" );
      folder.close();
   }

   public static void test2( PhotonSession session ) throws PhotonException {
      
      PhotonFolder folder = new PhotonFolder( session, "/" );
   }
   
   public static void createFolders( PhotonSession session ) throws PhotonException {

      PhotonFolder folder = new PhotonFolder( session, "/" );
      
      folder.createObject( "java1",
                           new ObjectDefinition(ObjectType.FOLDER, 0),
                           null,
                           null );
      folder.createObject( "java2",
                           new ObjectDefinition(ObjectType.FOLDER, 0),
                           null,
                           null );
      folder.createObject( "java3",
                           new ObjectDefinition(ObjectType.FOLDER, 0),
                           null,
                           null );
      for (FolderEntry entry : folder) {
         System.out.println( "Name: " + entry.getName() + ", Type: " 
            + entry.getType() + " or " + entry.getType().getText() );
      }
   }

}
