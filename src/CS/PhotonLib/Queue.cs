using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace Photon
{
    class Queue: IDisposable
    {
        // Track whether Dispose has been called.
        private bool disposed = false;

        private IntPtr handle;
        private IntPtr sessionHandle;

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoQueueClose(IntPtr objectHandle);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoQueueDefinition( 
            IntPtr               objectHandle, 
            ref ObjectDefinition definition );

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoQueueGetFirst( 
            IntPtr     objectHandle,
            byte[]     buffer,
            IntPtr     bufferLength,
            ref IntPtr returnedLength );

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoQueueGetNext( 
            IntPtr     objectHandle,
            byte[]     buffer,
            IntPtr     bufferLength,
            ref IntPtr returnedLength );

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoQueueOpen( 
            IntPtr     sessionHandle,
            string     queueName,
            IntPtr     nameLengthInBytes,
            ref IntPtr objectHandle );

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoQueuePop( 
            IntPtr     objectHandle,
            byte[]     buffer,
            IntPtr     bufferLength,
            ref IntPtr returnedLength );

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoQueuePush( 
            IntPtr  objectHandle, 
            byte[]  pItem, 
            IntPtr  length );

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoQueuePushNow( 
            IntPtr  objectHandle, 
            byte [] pItem, 
            IntPtr  length );

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoQueueStatus( 
            IntPtr        objectHandle,
            ref ObjStatus pStatus );

        public Queue(Session session)
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
                psoQueueClose(handle);
            }
            disposed = true;
        }

        ~Queue()      
        {
            Dispose(false);
        }
    }
}
