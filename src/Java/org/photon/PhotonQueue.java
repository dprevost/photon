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

/**
 * Queue class for the Photon library.
 */
// getDeclaredFields

class PhotonQueue {

   /** To save the native pointer/handle. */
   private long handle = 0;

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public PhotonQueue( PhotonSession session, String name ) throws PhotonException {
      
      int rc;
      
      rc = init( session, name );
      if ( rc != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(rc) );
      }
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   private static native void initIDs();

   static {
      initIDs();
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   private native int fini( long handle );
   private native int getDefinition( long              handle, 
                                     ObjectDefinition  definition,
                                     int               numFields,
                                     FieldDefinition[] fields );
   private native int getFirst( long   handle,
                                byte[] buffer,
                                int    bufferLength,
                                int[]  returnedLength );

   private native int getNext( long   handle,
                               byte[] buffer,
                               int    bufferLength,
                               int[]  returnedLength );

   private native int init( PhotonSession session, String queueName );

   private native int pop( long   handle,
                           byte[] buffer,
                           int    bufferLength,
                           int[]  returnedLength );

   private native int push( long   handle, 
                            byte[] pItem, 
                            int    length );

   private native int pushNow( long   handle, 
                               byte[] pItem, 
                               int    length );
   private native int getStatus( long      handle,
                                 ObjStatus status );

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

}
