/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@photonsoftware.org>
 *
 * This file is part of Photon (photonsoftware.org).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation and appearing in the file COPYING included in the
 * packaging of this library.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

package org.photon;

class psoFolderEntry {
   
   private int type;
   private String name;
   private int status;
}

/**
 * Folder class for the Photon library.
 */

class psoFolder {

   /** To save the native pointer/handle. */
   private long handle = 0;
   
   public psoFolder( psoSession session, String name ) throws psoException {

      handle = init( session.Handle(), name );
   }

   private native long fini( long h, String s ) throws psoException ;
   private native long init( long h, String s ) throws psoException ;
   private native psoFolderEntry getFirst( long h );
}

