using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace Photon
{
    class Lifo: IDisposable
    {
        // Track whether Dispose has been called.
        private bool disposed = false;

        private IntPtr handle;
        private IntPtr sessionHandle;

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoLifoClose(IntPtr objectHandle);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoLifoDefinition(
            IntPtr               objectHandle, 
            ref ObjectDefinition definition );

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoLifoGetFirst(
            IntPtr     objectHandle,
            byte[]     buffer,
            IntPtr     bufferLength,
            ref IntPtr returnedLength );

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoLifoGetNext(
            IntPtr     objectHandle,
            byte[]     buffer,
            IntPtr     bufferLength,
            ref IntPtr returnedLength );

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoLifoOpen(  
            IntPtr     sessionHandle,
            string     queueName,
            IntPtr     nameLengthInBytes,
            ref IntPtr objectHandle );

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoLifoPop( 
            IntPtr     objectHandle,
            byte[]     buffer,
            IntPtr     bufferLength,
            ref IntPtr returnedLength );

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoLifoPush( 
            IntPtr objectHandle, 
            byte[] pItem, 
            IntPtr length );

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoLifoStatus( 
            IntPtr        objectHandle,
            ref ObjStatus pStatus );

        public Lifo(Session session)
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
                psoLifoClose(handle);
            }
            disposed = true;
        }

        ~Lifo()      
        {
            Dispose(false);
        }
    }
}
