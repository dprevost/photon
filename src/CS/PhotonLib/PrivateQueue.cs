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
    public partial class Queue
    {
        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        // Track whether Dispose has been called.
        private bool disposed = false;

        private IntPtr handle;
        private IntPtr sessionHandle;

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoQueueClose(IntPtr objectHandle);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoQueueDefinition(
            IntPtr objectHandle,
            ref ObjectDefinition definition);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoQueueGetFirst(
            IntPtr objectHandle,
            byte[] buffer,
            UInt32 bufferLength,
            ref UInt32 returnedLength);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoQueueGetNext(
            IntPtr objectHandle,
            byte[] buffer,
            UInt32 bufferLength,
            ref UInt32 returnedLength);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoQueueOpen(
            IntPtr sessionHandle,
            string queueName,
            UInt32 nameLengthInBytes,
            ref IntPtr objectHandle);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoQueuePop(
            IntPtr objectHandle,
            byte[] buffer,
            UInt32 bufferLength,
            ref UInt32 returnedLength);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoQueuePush(
            IntPtr objectHandle,
            byte[] pItem,
            UInt32 length);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoQueuePushNow(
            IntPtr objectHandle,
            byte[] pItem,
            UInt32 length);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoQueueStatus(
            IntPtr objectHandle,
            ref ObjStatus pStatus);

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        private void Dispose(bool disposing)
        {
            // Check to see if Dispose has already been called.
            if (!this.disposed)
            {
                psoQueueClose(handle);
            }
            disposed = true;
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        ~Queue()
        {
            Dispose(false);
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
    }
}
