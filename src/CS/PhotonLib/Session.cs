using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace Photon
{
    class Session
    {
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
            string message,
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

        private IntPtr handle;

        public Session()
        {
            handle = (IntPtr)0;
        }

    }
}
