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
   int type = 0;

   /**
    * Status of the object.
    * <p>
    * For example, created but not committed, etc.
    */
   int status = 0;

   /** The number of blocks allocated to this object. */
   long numBlocks = 0;

   /** The number of groups of blocks allocated to this object. */
   long numBlockGroup = 0;

   /** The number of data items in this object. */
   long numDataItem = 0;
   
   /** The amount of free space available in the blocks allocated to this object. */
   long freeBytes = 0;

   /** Maximum data length (in bytes). */
   int maxDataLength = 0;
   
   /** Maximum key length (in bytes) if keys are supported - zero otherwise */
   int maxKeyLength = 0;

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   static {
      initIDs();
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   ObjectStatus() {}

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /** The amount of free space available in the blocks allocated to this object. */
   public long getFreeBytes() { return freeBytes; }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /** Maximum data length (in bytes). */
   public int getMaxDataLength() { return maxDataLength; }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /** Maximum key length (in bytes) if keys are supported - zero otherwise */
   public int getMaxKeyLength() { return maxKeyLength; }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /** The number of groups of blocks allocated to this object. */
   public long getNumBlockGroup() { return numBlockGroup; }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /** The number of blocks allocated to this object. */
   public long getNumBlocks() { return numBlocks; }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /** The number of data items in this object. */
   public long getNumDataItem() { return numDataItem; }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Status of the object.
    * <p>
    * For example, created but not committed, etc.
    */
   public int getStatus() { return status; }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /** The object type. */
   public ObjectType getType() { return ObjectType.getEnum(type); }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   private static native void initIDs();

};

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

