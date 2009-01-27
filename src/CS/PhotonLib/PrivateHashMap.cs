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
    public partial class HashMap
    {
        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        // Track whether Dispose has been called.
        private bool disposed = false;

        protected IntPtr handle;
        protected IntPtr sessionHandle;

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoHashMapClose(IntPtr objectHandle);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoHashMapDefinition(
            IntPtr objectHandle,
            ref ObjectDefinition definition);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        protected static extern int psoHashMapDelete(
            IntPtr objectHandle,
            IntPtr key,
            UInt32 keyLength);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoHashMapGet(
            IntPtr objectHandle,
            byte[] key,
            UInt32 keyLength,
            byte[] buffer,
            UInt32 bufferLength,
            ref UInt32 returnedLength);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoHashMapGetFirst(
            IntPtr objectHandle,
            byte[] key,
            UInt32 keyLength,
            byte[] buffer,
            UInt32 bufferLength,
            ref UInt32 retKeyLength,
            ref UInt32 retDataLength);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoHashMapGetNext(
            IntPtr objectHandle,
            byte[] key,
            UInt32 keyLength,
            byte[] buffer,
            UInt32 bufferLength,
            ref UInt32 retKeyLength,
            ref UInt32 retDataLength);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        protected static extern int psoHashMapInsert(
            IntPtr objectHandle,
            IntPtr key,
            UInt32 keyLength,
            IntPtr data,
            UInt32 dataLength);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoHashMapOpen(
            IntPtr sessionHandle,
            string hashMapName,
            UInt32 nameLengthInBytes,
            ref IntPtr objectHandle);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        protected static extern int psoHashMapReplace(
            IntPtr objectHandle,
            IntPtr key,
            UInt32 keyLength,
            IntPtr data,
            UInt32 dataLength);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoHashMapStatus(
            IntPtr objectHandle,
            ref ObjStatus pStatus);

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        private void Dispose(bool disposing)
        {
            // Check to see if Dispose has already been called.
            if (!this.disposed)
            {
                psoHashMapClose(handle);
                handle = (IntPtr)0;
            }
            disposed = true;
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        ~HashMap()
        {
            Dispose(false);
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
    }
}
