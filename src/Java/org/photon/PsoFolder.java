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

package org.photon;

class PsoFolderEntry {
   
   private int type;
   private String name;
   private int status;
   
   public int getType()    { return type; }
   public String getName() { return name; }
   public int getStatus()  { return status; }
}

/**
 * Folder class for the Photon library.
 */

class PsoFolder {

   /** To save the native pointer/handle. */
   private long handle = 0;
   private long sessionHandle = 0;

   /* Iterations
    * 
    * Usage:
    *
    * while ( folder.getNext() ) {
    *     type   = folder.entryType();
    *     name   = folder.entryName();
    *     status = folder.entryStatus();
    * }
    */
   private boolean endIteration = true;
   private int entryType;
   private String entryName;
   private int entryStatus;
   
   public int getEntryType()    { return entryType; }
   public String getEntryName() { return entryName; }
   public int getEntryStatus()  { return entryStatus; }
   
   public PsoFolder( PsoSession session, String name ) throws PsoException {

      handle = init( session.Handle(), name );
   }

   private native long fini( long h ) throws PsoException;
   private native long init( long h, String s ) throws PsoException;

   private native void folderCreateObject( long h, String objectName, 
      ObjectDefinition definition ) throws PsoException;
   private native void folderCreateObjectXML( long h, String xmlBuffer ) throws PsoException;
   private native void folderDestroyObject( long h, String objectName ) throws PsoException;

   private native boolean folderGetFirst( long h, int entryType, 
      String entryName, int entryStatus );
   private native boolean folderGetNext( long h, int entryType, 
      String entryName, int entryStatus );

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   protected void finalize() {
      fini( handle );
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public void Close() { fini(handle); }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public void createObject( String           objectName,
                             ObjectDefinition definition) {
      int rc;

      if ( sessionHandle == 0 || handle == 0 ) {
         throw new PsoException( PsoErrors.NULL_HANDLE );
      }

      folderCreateObject( handle, 
                          objectName, 
                          definition );
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public void createObjectXML(String xmlBuffer) {
      int rc;

      if ( sessionHandle == 0 || handle == 0 ) {
         throw new PsoException( PsoErrors.NULL_HANDLE );
      }

      folderCreateObjectXML( handle, xmlBuffer );
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public void destroyObject(String objectName) {
      int rc;

      if ( sessionHandle == 0 || handle == 0 ) {
         throw new PsoException( PsoErrors.NULL_HANDLE );
      }

      folderDestroyObject( handle, objectName );
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public boolean getNext() {
   
      boolean rc;

      if ( sessionHandle == 0 || handle == 0 ) {
         throw new PsoException( PsoErrors.NULL_HANDLE );
      }

      try {
         if ( endIteration ) {
            endIteration = false;
            rc = folderGetFirst( handle, entryType, entryName, entryStatus );
         } else {
            rc = folderGetNext( handle, entryType, entryName, entryStatus );
         }
         if ( ! rc ) { endIteration = true; }
      } catch (PsoException e) {
         endIteration = true;
         throw e;
      }
      
      return rc;
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public void Open( String folderName ) {
      int rc;

      if ( sessionHandle == 0 ) {
         throw new PsoException( PsoErrors.NULL_HANDLE );
      }

      handle = folderOpen( sessionHandle, folderName );
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        public void Status( ObjStatus status)
        {
            int rc;

            if (sessionHandle == (IntPtr)0 || handle == (IntPtr)0)
            {
                rc = (int)PsoErrors.NULL_HANDLE;
                throw new PsoException(PsoException.PrepareException("Folder.Status", rc), rc);
            }

            rc = psoFolderStatus(handle, status);
            if (rc != 0)
            {
                throw new PsoException(PsoException.PrepareException(sessionHandle, "Folder.Status"), rc);
            }
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--


}

