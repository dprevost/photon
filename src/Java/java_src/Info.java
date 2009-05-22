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

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

public class Info {
   
   /** Total size of the virtual data space. */
   public long totalSizeInBytes;
   
   /** Total size of the allocated blocks. */
   public long allocatedSizeInBytes;
   
   /** Number of API objects in the pso (internal objects are not counted). */
   public long numObjects;
   
   /** Total number of groups of blocks. */
   public long numGroups;
   
   /** Number of calls to allocate groups of blocks. */
   public long numMallocs;
   
   /** Number of calls to free groups of blocks. */
   public long numFrees;
   
   /** Largest contiguous group of free blocks. */
   public long largestFreeInBytes;
   
   /** Shared-memory version. */
   public int memoryVersion;

   /** Endianess (0 for little endian, 1 for big endian). */
   public int bigEndian;
   
   /** Compiler name. */
   public String compiler;

   /** Compiler version (if available). */
   public String compilerVersion;
   
   /** Platform. */
   public String platform;
   
   /** Shared lib version. */
   public String dllVersion;
   
   /** The server version (of the pso creator). */
   public String quasarVersion;
   
   /** Timestamp of creation of the shared memory. */
   public String creationTime;

   Info() {}

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   private static native void initIDs();

   static {
      initIDs();
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
