using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace Photon
{
    public partial class HashMap
    {
        // Track whether Dispose has been called.
        private bool disposed = false;

        protected IntPtr handle;
        protected IntPtr sessionHandle;

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoHashMapClose(IntPtr objectHandle);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoHashMapDefinition(
            IntPtr objectHandle,
            ref ObjectDefinition definition);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        protected static extern int psoHashMapDelete(
            IntPtr objectHandle,
            IntPtr key,
            IntPtr keyLength);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoHashMapGet(
            IntPtr objectHandle,
            byte[] key,
            IntPtr keyLength,
            byte[] buffer,
            IntPtr bufferLength,
            ref IntPtr returnedLength);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoHashMapGetFirst(
            IntPtr objectHandle,
            byte[] key,
            IntPtr keyLength,
            byte[] buffer,
            IntPtr bufferLength,
            ref IntPtr retKeyLength,
            ref IntPtr retDataLength);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoHashMapGetNext(
            IntPtr objectHandle,
            byte[] key,
            IntPtr keyLength,
            byte[] buffer,
            IntPtr bufferLength,
            ref IntPtr retKeyLength,
            ref IntPtr retDataLength);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        protected static extern int psoHashMapInsert(
            IntPtr objectHandle,
            IntPtr key,
            IntPtr keyLength,
            IntPtr data,
            IntPtr dataLength);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoHashMapOpen(
            IntPtr sessionHandle,
            string hashMapName,
            IntPtr nameLengthInBytes,
            ref IntPtr objectHandle);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        protected static extern int psoHashMapReplace(
            IntPtr objectHandle,
            IntPtr key,
            IntPtr keyLength,
            IntPtr data,
            IntPtr dataLength);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoHashMapStatus(
            IntPtr objectHandle,
            ref ObjStatus pStatus);

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        private void Dispose(bool disposing)
        {
            // Check to see if Dispose has already been called.
            if (!this.disposed)
            {
                psoHashMapClose(handle);
                handle = (IntPtr)0;
            }
            disposed = true;
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        ~HashMap()
        {
            Dispose(false);
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

    }
}
