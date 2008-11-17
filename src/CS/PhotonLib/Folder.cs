using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace Photon
{
    public class Folder: IDisposable
    {
        // Track whether Dispose has been called.
        private bool disposed = false;

        private IntPtr handle = (IntPtr)0;
        private IntPtr sessionHandle = (IntPtr)0;

        [DllImport("photon.dll", EntryPoint = "psoFolderClose", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoFolderClose( IntPtr objectHandle );

        [DllImport("photon.dll", EntryPoint = "psoFolderCreateObject", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoFolderCreateObject( 
            IntPtr           folderHandle,
            string           objectName,
            IntPtr           nameLengthInBytes,
            ObjectDefinition pDefinition);

        [DllImport("photon.dll", EntryPoint = "psoFolderCreateObjectXML", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoFolderCreateObjectXML( 
            IntPtr folderHandle,
            string xmlBuffer,
            IntPtr lengthInBytes );

        [DllImport("photon.dll", EntryPoint = "psoFolderDestroyObject", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoFolderDestroyObject( 
            IntPtr folderHandle,
            string objectName,
            IntPtr nameLengthInBytes );

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
            IntPtr     nameLengthInBytes,
            ref IntPtr objectHandle );

        [DllImport("photon.dll", EntryPoint = "psoFolderStatus", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoFolderStatus( 
            IntPtr        objectHandle,
            ref ObjStatus pStatus );

        public Folder(Session session)
        {
            handle = (IntPtr)0;
            sessionHandle = session.handle;
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
                psoFolderClose(handle);
            }
            disposed = true;
        }

        ~Folder()      
        {
            Dispose(false);
        }
    }
}
