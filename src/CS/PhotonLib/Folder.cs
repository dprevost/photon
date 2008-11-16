using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace Photon
{
    public class Folder
    {
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

        private IntPtr handle;

        public Folder()
        {
            handle = (IntPtr)0;
        }
    }
}
