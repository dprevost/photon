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

public class Session {

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public static void Commit() throws PhotonException {

      PhotonSession session = new PhotonSession();
      
      session.commit();
      session.close();

      try {
         session.commit();
      } catch ( PhotonException e ) {
         System.out.println( e.getMessage() );
         if ( e.getErrorCode() != PhotonErrors.NULL_HANDLE ) {
            throw e;
         }
      }
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public static void Info(PhotonSession session) throws PhotonException {
      
      Info info;
      
      info = session.getInfo();
      System.out.println( "Compiler: " + info.compiler );
      System.out.println( "Compiler Version: " + info.compilerVersion );
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public static void Status(PhotonSession session) throws PhotonException {
      
      ObjStatus status;
      
      try {
         status = session.getStatus( "/SessionTest1" );
      } catch ( PhotonException e ) {
         System.out.println( e.getMessage() );
         if ( e.getErrorCode() != PhotonErrors.NO_SUCH_OBJECT ) {
            throw e;
         }
      }
      
      session.createObject( "/SessionTest1",
                            new ObjectDefinition(ObjectType.FOLDER, 0),
                            null,
                            null );
      status = session.getStatus( "/SessionTest1" );
      System.out.println( "Object Type: " + status.type );
      
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
   
   public static void tests() throws PhotonException {
      
      PhotonSession session;
      
      /* Starts with tests on the creation/use of sessions */
      Commit();
      
      /* Tests for the other session methods */
      session = new PhotonSession();
      Info(session);
      Status(session);
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

}
