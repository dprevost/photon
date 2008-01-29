/*
 * Copyright (C) 2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

/**
 * 
 */

class VdsSession {

   /** To save the native pointer/handle. */
   private long handle;
   
   public VdsSession() throws VdsException {

      handle = initSession();
   }

   private native long initSession() throws VdsException ;
   
   public long Handle() {
      return handle;
   }
}
