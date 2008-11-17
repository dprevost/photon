using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace Photon
{
    public class FastMap
    {
        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int psoFastMapClose(IntPtr objectHandle);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int psoFastMapDefinition( 
            IntPtr               objectHandle, 
            ref ObjectDefinition definition );

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int psoFastMapDelete( 
            IntPtr objectHandle,
            byte[] key,
            IntPtr keyLength );

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int psoFastMapEdit( 
            IntPtr     sessionHandle,
            string     hashMapName,
            IntPtr     nameLengthInBytes,
            ref IntPtr objectHandle );

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int psoFastMapEmpty(IntPtr objectHandle);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int psoFastMapGet( 
            IntPtr     objectHandle,
            byte[]     key,
            IntPtr     keyLength,
            byte[]     buffer,
            IntPtr     bufferLength,
            ref IntPtr returnedLength );

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int psoFastMapGetFirst( 
            IntPtr     objectHandle,
            byte[]     key,
            IntPtr     keyLength,
            byte[]     buffer,
            IntPtr     bufferLength,
            ref IntPtr retKeyLength,
            ref IntPtr retDataLength );

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int psoFastMapGetNext( 
            IntPtr     objectHandle,
            byte[]     key,
            IntPtr     keyLength,
            byte[]     buffer,
            IntPtr     bufferLength,
            ref IntPtr retKeyLength,
            ref IntPtr retDataLength );

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int psoFastMapInsert( 
            IntPtr objectHandle,
            byte[] key,
            IntPtr keyLength,
            byte[] data,
            IntPtr dataLength );

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int psoFastMapOpen( 
            IntPtr     sessionHandle,
            string     hashMapName,
            IntPtr     nameLengthInBytes,
            ref IntPtr objectHandle );

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int psoFastMapReplace( 
            IntPtr objectHandle,
            byte[] key,
            IntPtr keyLength,
            byte[] data,
            IntPtr dataLength );

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int psoFastMapStatus( 
            IntPtr        objectHandle,
            ref ObjStatus pStatus );

    }
}
