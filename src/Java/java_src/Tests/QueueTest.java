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

   class myStruct {
      Integer i;
      String  str;
      Object[] a;
      
      myStruct( int i, String s ) {
         this.i = i;
         this.str = s;
         this.a = new Object[2];
         a[0] = this.i;
         a[1] = this.str;
      }
      
      public byte[] packObject() {
         byte[]x = new byte[10];
         return x;
      }
   
      public Object unpackObject( byte[] buffer ) { return buffer; }
   }

public class QueueTest {

   
   public static void test1( org.photon.Session session ) throws PhotonException {
      
//      org.photon.Folder folder = new org.photon.Folder();
//      Queue<myStruct> queue = new Queue<myStruct>( session, "myQueue" );
      myStruct s = new myStruct( 123, "45678" );
      
//      queue.push( s.i, s.str );
//      queue.push( s.a );
//      queue.push( 234, "012345" );
      
//      folder.open( session, "/" );
//      folder.close();
   }

   public static void test2( org.photon.Session session ) throws PhotonException {
      
      org.photon.Folder folder = new org.photon.Folder( session, "/" );
   }
   
   public static void createFolders( org.photon.Session session ) throws PhotonException {

      org.photon.Folder folder = new org.photon.Folder( session, "/" );
      
//      folder.createObject( "java1",
//                           new ObjectDefinition(ObjectType.FOLDER, 0),
//                           null,
//                           null );
//      folder.createObject( "java2",
//                           new ObjectDefinition(ObjectType.FOLDER, 0),
//                           null,
//                           null );
//      folder.createObject( "java3",
//                           new ObjectDefinition(ObjectType.FOLDER, 0),
//                           null,
//                           null );
      for (FolderEntry entry : folder) {
         System.out.println( "Name: " + entry.getName() + ", Type: " 
            + entry.getType() + " or " + entry.getType().getText() );
      }
   }

}
