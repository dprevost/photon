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
 * This class/data structure is used to retrieve the status of objects.
 */
public class ObjectStatus {

   /** The object type. */
   public ObjectType type;

   /**
    * Status of the object.
    * <p>
    * For example, created but not committed, etc.
    */
   public int status;

   /** The number of blocks allocated to this object. */
   public long numBlocks;

   /** The number of groups of blocks allocated to this object. */
   public long numBlockGroup;

   /** The number of data items in this object. */
   public long numDataItem;
   
   /** The amount of free space available in the blocks allocated to this object. */
   public long freeBytes;

   /** Maximum data length (in bytes). */
   public int maxDataLength;
   
   /** Maximum key length (in bytes) if keys are supported - zero otherwise */
   public int maxKeyLength;

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   ObjectStatus() {}

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   private static native void initIDs();

   static {
      initIDs();
   }

};

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

