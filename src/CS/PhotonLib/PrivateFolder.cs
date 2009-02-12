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
    public partial class Folder
    {
        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        // Track whether Dispose has been called.
        private bool disposed = false;

        private IntPtr handle = (IntPtr)0;
        private IntPtr sessionHandle = (IntPtr)0;

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        [DllImport("photon.dll", EntryPoint = "psoFolderClose", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoFolderClose( IntPtr objectHandle );

        [DllImport("photon.dll", EntryPoint = "psoFolderCreateObject", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoFolderCreateObject( 
            IntPtr               folderHandle,
            string               objectName,
            UInt32               nameLengthInBytes,
            ref ObjectDefinition pDefinition,
            ref KeyDefinition    pKey,
            FieldDefinition []     pFields );

        [DllImport("photon.dll", EntryPoint = "psoFolderCreateObjectXML", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoFolderCreateObjectXML( 
            IntPtr folderHandle,
            string xmlBuffer,
            UInt32 lengthInBytes );

        [DllImport("photon.dll", EntryPoint = "psoFolderDestroyObject", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoFolderDestroyObject( 
            IntPtr folderHandle,
            string objectName,
            UInt32 nameLengthInBytes );

        [DllImport("photon.dll", EntryPoint = "psoFolderGetFirst", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoFolderGetFirst( 
            IntPtr          objectHandle,
            ref FolderEntry pEntry );

        [DllImport("photon.dll", EntryPoint = "psoFolderGetNext", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoFolderGetNext( 
            IntPtr          objectHandle,
            ref FolderEntry pEntry );

        [DllImport("photon.dll", EntryPoint = "psoFolderOpen", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoFolderOpen( 
            IntPtr     sessionHandle,
            string     folderName,
            UInt32     nameLengthInBytes,
            ref IntPtr objectHandle );

        [DllImport("photon.dll", EntryPoint = "psoFolderStatus", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoFolderStatus( 
            IntPtr        objectHandle,
            ref ObjStatus pStatus );

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        private void Dispose(bool disposing)
        {
            // Check to see if Dispose has already been called. 
            if (!this.disposed)
            {
                psoFolderClose(handle);
            }
            disposed = true;
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        ~Folder()      
        {
            Dispose(false);
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
    }
}
