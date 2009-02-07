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
   
   public PsoFolder( PsoSession session, String name ) throws PsoException {

      handle = init( session.Handle(), name );
   }

   private native long fini( long h ) throws PsoException ;
   private native long init( long h, String s ) throws PsoException ;
   private native PsoFolderEntry getFirst( long h );

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
         rc = (int)PhotonErrors.NULL_HANDLE;
         throw new PhotonException(PhotonException.PrepareException("Folder.CreateObject", rc), rc);
      }

      rc = folderCreateObject( handle, 
                               objectName, 
                               definition );
      if (rc != 0) {
         throw new PhotonException(PhotonException.PrepareException(sessionHandle, "Folder.CreateObject"), rc);
         
      }
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        public void CreateObjectXML(String xmlBuffer)
        {
            int rc;

            if (sessionHandle == (IntPtr)0 || handle == (IntPtr)0)
            {
                rc = (int)PhotonErrors.NULL_HANDLE;
                throw new PhotonException(PhotonException.PrepareException("Folder.CreateObjectXML", rc), rc);
            }

            rc = psoFolderCreateObjectXML(handle,
                                          xmlBuffer,
                                          (UInt32)xmlBuffer.Length);
            if (rc != 0)
            {
                throw new PhotonException(PhotonException.PrepareException(sessionHandle, "Folder.CreateObjectXML"), rc);
            }
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        public void DestroyObject(String objectName)
        {
            int rc;

            if (sessionHandle == (IntPtr)0 || handle == (IntPtr)0)
            {
                rc = (int)PhotonErrors.NULL_HANDLE;
                throw new PhotonException(PhotonException.PrepareException("Folder.DestroyObject", rc), rc);
            }

            rc = psoFolderDestroyObject(handle,
                                        objectName,
                                        (UInt32)objectName.Length );
            if (rc != 0)
            {
                throw new PhotonException(PhotonException.PrepareException(sessionHandle, "Folder.DestroyObject"), rc);
            }
        }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public int GetFirst( PsoFolderEntry entry) {
   
      int rc;

      if (sessionHandle == 0 || handle == 0 ) {
         rc = (int)PhotonErrors.NULL_HANDLE;
         throw new PhotonException(PhotonException.PrepareException("Folder.GetFirst", rc), rc);
      }

      rc = psoFolderGetFirst(handle, entry);
      if (rc != 0 && rc != (int)PhotonErrors.IS_EMPTY) {
         throw new PhotonException(PhotonException.PrepareException(sessionHandle, "Folder.GetFirst"), rc);
      }
      
      return rc;
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        public int GetNext( PsoFolderEntry entry )
        {
            int rc;

            if (sessionHandle == (IntPtr)0 || handle == (IntPtr)0)
            {
                rc = (int)PhotonErrors.NULL_HANDLE;
                throw new PhotonException(PhotonException.PrepareException("Folder.GetNext", rc), rc);
            }

            rc = psoFolderGetNext(handle, entry);
            if (rc != 0 && rc != (int)PhotonErrors.REACHED_THE_END)
            {
                throw new PhotonException(PhotonException.PrepareException(sessionHandle, "Folder.GetNext"), rc);
            }

            return rc;
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        public void Open(String folderName)
        {
            int rc;

            if (sessionHandle == (IntPtr)0)
            {
                rc = (int)PhotonErrors.NULL_HANDLE;
                throw new PhotonException(PhotonException.PrepareException("Folder.Open", rc), rc);
            }

            rc = psoFolderOpen(sessionHandle,
                               folderName,
                               (UInt32)folderName.Length,
                               handle);
            if (rc != 0)
            {
                throw new PhotonException(PhotonException.PrepareException(sessionHandle, "Folder.Open"), rc);
            }
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        public void Status( ObjStatus status)
        {
            int rc;

            if (sessionHandle == (IntPtr)0 || handle == (IntPtr)0)
            {
                rc = (int)PhotonErrors.NULL_HANDLE;
                throw new PhotonException(PhotonException.PrepareException("Folder.Status", rc), rc);
            }

            rc = psoFolderStatus(handle, status);
            if (rc != 0)
            {
                throw new PhotonException(PhotonException.PrepareException(sessionHandle, "Folder.Status"), rc);
            }
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--


}

