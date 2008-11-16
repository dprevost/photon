using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace Photon
{
    class Queue
    {
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
            byte       buffer,
            IntPtr     bufferLength,
            ref IntPtr returnedLength );

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoQueuePush( 
            IntPtr  objectHandle, 
            byte [] pItem, 
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

    }
}
