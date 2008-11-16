using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace Photon
{
    class HashMap
    {
        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoHashMapClose(IntPtr objectHandle);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoHashMapDefinition( 
            IntPtr               objectHandle, 
            ref ObjectDefinition definition );

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoHashMapDelete( 
            IntPtr objectHandle,
            byte[] key,
            IntPtr keyLength );

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoHashMapGet( 
            IntPtr     objectHandle,
            byte[]     key,
            IntPtr     keyLength,
            byte[]     buffer,
            IntPtr     bufferLength,
            ref IntPtr returnedLength );

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoHashMapGetFirst( 
            IntPtr     objectHandle,
            byte[]     key,
            IntPtr     keyLength,
            byte[]     buffer,
            IntPtr     bufferLength,
            ref IntPtr retKeyLength,
            ref IntPtr retDataLength );

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoHashMapGetNext( 
            IntPtr     objectHandle,
            byte[]     key,
            IntPtr     keyLength,
            byte[]     buffer,
            IntPtr     bufferLength,
            ref IntPtr retKeyLength,
            ref IntPtr retDataLength );

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoHashMapInsert( 
            IntPtr  objectHandle,
            byte[]  key,
            IntPtr  keyLength,
            byte[]  data,
            IntPtr  dataLength );

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoHashMapOpen( 
            IntPtr     sessionHandle,
            byte[]     hashMapName,
            IntPtr     nameLengthInBytes,
            ref IntPtr objectHandle );

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoHashMapReplace( 
            IntPtr  objectHandle,
            byte[]  key,
            IntPtr  keyLength,
            byte[]  data,
            IntPtr  dataLength );

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoHashMapStatus( 
            IntPtr        objectHandle,
            ref ObjStatus pStatus );

    }
}
