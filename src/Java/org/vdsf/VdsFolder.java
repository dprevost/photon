/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of the vdsf (Virtual Data Space Framework) Library.
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

package org.vdsf;

class VdsFolderEntry {
   
   private int type;
   private String name;
   private int status;
}

/**
 * Folder class for the vdsf library.
 */

class VdsFolder {

   /** To save the native pointer/handle. */
   private long handle = 0;
   
   public VdsFolder( VdsSession session, String name ) throws VdsException {

      handle = init( session.Handle(), name );
   }

   private native long fini( long h, String s ) throws VdsException ;
   private native long init( long h, String s ) throws VdsException ;
   private native VdsFolderEntry getFirst( long h );
}

