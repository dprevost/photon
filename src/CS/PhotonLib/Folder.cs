/*
 * Copyright (C) 2008-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace Photon
{
    public partial class Folder: IDisposable
    {
        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        public Folder(Session session)
        {
            handle = (IntPtr)0;
            sessionHandle = session.handle;
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        public Folder(Session session, String folderName )
        {
            int rc;

            handle = (IntPtr)0;
            sessionHandle = session.handle;

            if (sessionHandle == (IntPtr)0)
            {
                rc = (int)PhotonErrors.NULL_HANDLE;
                throw new PhotonException(PhotonException.PrepareException("Folder.Folder", rc), rc);
            }

            rc = psoFolderOpen(sessionHandle,
                               folderName,
                               (UInt32)folderName.Length,
                               ref handle);
            if (rc != 0)
            {
                throw new PhotonException(PhotonException.PrepareException(sessionHandle, "Folder.Folder"), rc);
            }
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        public void Close()
        {
            int rc;

            if (handle == (IntPtr)0)
            {
                rc = (int)PhotonErrors.NULL_HANDLE;
                throw new PhotonException(PhotonException.PrepareException("Folder.CreateObject", rc), rc);
            }
            rc = psoFolderClose(handle);
            if (rc != 0)
            {
                throw new PhotonException(PhotonException.PrepareException(sessionHandle, "Folder.CreateObject"), rc);
            }
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        public void CreateObject(String               objectName,
                                 ref ObjectDefinition pDefinition,
                                 ref KeyDefinition    pKey,
                                 FieldDefinition []   pFields )
        {
            int rc;

            if (handle == (IntPtr)0)
            {
                rc = (int)PhotonErrors.NULL_HANDLE;
                throw new PhotonException(PhotonException.PrepareException("Folder.CreateObject", rc), rc);
            }

            rc = psoFolderCreateObject(handle, 
                                       objectName, 
                                       (UInt32)objectName.Length,
                                       ref pDefinition,
                                       ref pKey,
                                       pFields );
            if (rc != 0)
            {
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

        public int GetFirst(ref FolderEntry entry)
        {
            int rc;

            if (sessionHandle == (IntPtr)0 || handle == (IntPtr)0)
            {
                rc = (int)PhotonErrors.NULL_HANDLE;
                throw new PhotonException(PhotonException.PrepareException("Folder.GetFirst", rc), rc);
            }

            rc = psoFolderGetFirst(handle, ref entry);
            if (rc != 0 && rc != (int)PhotonErrors.IS_EMPTY)
            {
                throw new PhotonException(PhotonException.PrepareException(sessionHandle, "Folder.GetFirst"), rc);
            }

            return rc;
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        public int GetNext(ref FolderEntry entry )
        {
            int rc;

            if (sessionHandle == (IntPtr)0 || handle == (IntPtr)0)
            {
                rc = (int)PhotonErrors.NULL_HANDLE;
                throw new PhotonException(PhotonException.PrepareException("Folder.GetNext", rc), rc);
            }

            rc = psoFolderGetNext(handle, ref entry);
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
                               ref handle);
            if (rc != 0)
            {
                throw new PhotonException(PhotonException.PrepareException(sessionHandle, "Folder.Open"), rc);
            }
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        public void Status(ref ObjStatus status)
        {
            int rc;

            if (sessionHandle == (IntPtr)0 || handle == (IntPtr)0)
            {
                rc = (int)PhotonErrors.NULL_HANDLE;
                throw new PhotonException(PhotonException.PrepareException("Folder.Status", rc), rc);
            }

            rc = psoFolderStatus(handle, ref status);
            if (rc != 0)
            {
                throw new PhotonException(PhotonException.PrepareException(sessionHandle, "Folder.Status"), rc);
            }
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
    }
}
