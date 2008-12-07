/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@photonsoftware.org>
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
    public class Session: IDisposable
    {
        // Track whether Dispose has been called.
        private bool disposed = false;

        internal IntPtr handle;

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoCommit(IntPtr sessionHandle);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoCreateObject( 
            IntPtr           sessionHandle,
            string           objectName,
            IntPtr           nameLengthInBytes,
            ObjectDefinition pDefinition );

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoDestroyObject(
            IntPtr sessionHandle,
            string objectName,
            IntPtr nameLengthInBytes );

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoErrorMsg( 
            IntPtr sessionHandle,
            [MarshalAs(UnmanagedType.LPStr)] StringBuilder message,
            IntPtr msgLengthInBytes );

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoExitSession(IntPtr sessionHandle);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoGetDefinition( 
            IntPtr               sessionHandle,
            string               objectName,
            IntPtr               nameLengthInBytes,
            ref ObjectDefinition definition );

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoGetInfo( 
            IntPtr   sessionHandle,
            ref Info pInfo );

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoGetStatus( 
            IntPtr    sessionHandle,
            string    objectName,
            IntPtr    nameLengthInBytes,
            ObjStatus pStatus );

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoInitSession(ref IntPtr sessionHandle);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoLastError(IntPtr sessionHandle);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoRollback(IntPtr sessionHandle);

        public Session()
        {
            handle = (IntPtr)0;
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        private void Dispose(bool disposing)
        {
            // Check to see if Dispose has already been called.
            if (!this.disposed)
            {
                psoExitSession(handle);
            }
            disposed = true;
        }

        ~Session()      
        {
            Dispose(false);
        }

        public void CreateObject()
        {
        }
    }
}
