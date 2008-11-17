using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace Photon
{
    class Session: IDisposable
    {
        // Track whether Dispose has been called.
        private bool disposed = false;

        private IntPtr handle;

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
    }
}
