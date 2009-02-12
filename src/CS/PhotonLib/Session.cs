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
    public partial class Session: IDisposable
    {
        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        public Session()
        {
            int rc;

            handle = (IntPtr)0;

            rc = psoInitSession(ref handle);
            if (rc != 0)
            {
                throw new PhotonException(PhotonException.PrepareException("Session.Session", rc), rc);
            }
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
        
        public void Commit()
        {
            int rc;

            if (handle == (IntPtr)0)
            {
                rc = (int)PhotonErrors.NULL_HANDLE;
                throw new PhotonException(PhotonException.PrepareException("Session.Commit", rc), rc);
            }

            rc = psoCommit(handle );
            if (rc != 0)
            {
                throw new PhotonException(PhotonException.PrepareException(handle, "Session.Commit"), rc);
            }
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        public void CreateObject( String           objectName,
                                  ObjectDefinition definition )
        {
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        public void CreateObject( String               objectName,
                                  ref ObjectDefinition definition,
                                  ref KeyDefinition    key,
                                  FieldDefinition[]    fields )
        {
            int rc;

            if (handle == (IntPtr)0)
            {
                rc = (int)PhotonErrors.NULL_HANDLE;
                throw new PhotonException(PhotonException.PrepareException("Session.DestroyObject", rc), rc);
            }
            
            rc = psoCreateObject(handle,
                                 objectName,
                                 (UInt32)objectName.Length,
                                 ref definition,
                                 ref key,
                                 fields );
            if (rc != 0)
            {
                throw new PhotonException(PhotonException.PrepareException(handle, "Session.DestroyObject"), rc);
            }
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        public void DestroyObject( String objectName )
        {
            int rc;

            if (handle == (IntPtr)0)
            {
                rc = (int)PhotonErrors.NULL_HANDLE;
                throw new PhotonException(PhotonException.PrepareException("Session.DestroyObject", rc), rc);
            }

            rc = psoDestroyObject(handle,
                                  objectName,
                                  (UInt32)objectName.Length);
            if (rc != 0)
            {
                throw new PhotonException(PhotonException.PrepareException(handle, "Session.DestroyObject"), rc);
            }
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        public String ErrorMsg()
        {
            int rc;
            StringBuilder msg = new StringBuilder();

            if (handle == (IntPtr)0)
            {
                rc = (int)PhotonErrors.NULL_HANDLE;
                throw new PhotonException(PhotonException.PrepareException("Session.ErrorMsg", rc), rc);
            }

            msg.Capacity = 1024;
            rc = psoErrorMsg(handle, msg, 1024 );
            if ( rc != 0 ) 
            {
                throw new PhotonException(PhotonException.PrepareException(handle, "Session.ErrorMsg"), rc);
            }

            return msg.ToString();
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        public void Exit()
        {
            Dispose();
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        public void GetDefinition(String objectName,
                                  ObjectDefinition definition)
        {
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        public void GetInfo( ref Info info )
        {
            int rc;

            if (handle == (IntPtr)0)
            {
                rc = (int)PhotonErrors.NULL_HANDLE;
                throw new PhotonException(PhotonException.PrepareException("Session.GetInfo", rc), rc);
            }

            rc = psoGetInfo(handle, ref info);
            if (rc != 0)
            {
                throw new PhotonException(PhotonException.PrepareException(handle, "Session.GetInfo"), rc);
            }
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        public void GetStatus( String        objectName,
                               ref ObjStatus status )
        {
            int rc;

            if (handle == (IntPtr)0)
            {
                rc = (int)PhotonErrors.NULL_HANDLE;
                throw new PhotonException(PhotonException.PrepareException("Session.GetStatus", rc), rc);
            }

            rc = psoGetStatus(handle, 
                              objectName,
                              (UInt32)objectName.Length,
                              ref status);
            if (rc != 0)
            {
                throw new PhotonException(PhotonException.PrepareException(handle, "Session.GetStatus"), rc);
            }
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        public int LastError()
        {
            return psoLastError(handle);
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        public void Rollback()
        {
            int rc;

            if (handle == (IntPtr)0)
            {
                rc = (int)PhotonErrors.NULL_HANDLE;
                throw new PhotonException(PhotonException.PrepareException("Session.Rollback", rc), rc);
            }

            rc = psoRollback(handle);
            if (rc != 0)
            {
                throw new PhotonException(PhotonException.PrepareException(handle, "Session.Rollback"), rc);
            }
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
    }
}
