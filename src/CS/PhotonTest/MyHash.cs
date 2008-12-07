using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using Photon;

namespace PhotonTest
{
    public class MyHash : HashMap
    {
        public MyHash(Session session)
            : base(session)
        {
            dataPtr = Marshal.AllocHGlobal(Marshal.SizeOf(my));
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        [StructLayout(LayoutKind.Sequential)]
        protected struct MyStruct
        {
            public int type;
            public int status;
            [MarshalAs(UnmanagedType.LPStr, SizeConst = 256)]
            public String name;
        }

        protected MyStruct my;

        private IntPtr dataPtr;

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        public void Delete(
            String key,
            IntPtr keyLength)
        { 
            int rc;
            IntPtr keyPtr;

            keyPtr = Marshal.StringToHGlobalAnsi(key);

            if (sessionHandle == (IntPtr)0 || handle == (IntPtr)0)
            {
                rc = (int)PhotonErrors.NULL_HANDLE;
                throw new PhotonException(PhotonException.PrepareException("HashMap.Delete", rc), rc);
            }

            rc = psoHashMapDelete(handle,
                                  keyPtr,
                                  (UInt32)Marshal.SizeOf(key));
            Marshal.FreeHGlobal(keyPtr);

            if (rc != 0)
            {
                throw new PhotonException(PhotonException.PrepareException(sessionHandle, "HashMap.Insert"), rc);
            }
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        public void Insert(string key,
                           int    type,
                           int    status,
                           String name)
        {
            int rc;
            IntPtr keyPtr;

            my.type = type;
            my.status = status;
            my.name = name;
            keyPtr = Marshal.StringToHGlobalAnsi(key);
            Marshal.StructureToPtr(my, dataPtr, true);

            if (sessionHandle == (IntPtr)0 || handle == (IntPtr)0)
            {
                rc = (int)PhotonErrors.NULL_HANDLE;
                throw new PhotonException(PhotonException.PrepareException("HashMap.Insert", rc), rc);
            }

            rc = psoHashMapInsert(handle,
                                   keyPtr,
                                   (UInt32)Marshal.SizeOf(key),
                                   dataPtr, //ref my,
                                   (UInt32)Marshal.SizeOf(my));
            Marshal.FreeHGlobal(keyPtr);

            if (rc != 0)
            {
                throw new PhotonException(PhotonException.PrepareException(sessionHandle, "HashMap.Insert"), rc);
            }
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        public void Replace(
            string key,
            int    type,
            int    status,
            String name)
        {
            int rc;
            IntPtr keyPtr;

            my.type = type;
            my.status = status;
            my.name = name;
            keyPtr = Marshal.StringToHGlobalAnsi(key);
            Marshal.StructureToPtr(my, dataPtr, true);

            if (sessionHandle == (IntPtr)0 || handle == (IntPtr)0)
            {
                rc = (int)PhotonErrors.NULL_HANDLE;
                throw new PhotonException(PhotonException.PrepareException("HashMap.Insert", rc), rc);
            }

            rc = psoHashMapReplace(handle,
                                   keyPtr,
                                   (UInt32)Marshal.SizeOf(key),
                                   dataPtr, //ref my,
                                   (UInt32)Marshal.SizeOf(my));
            Marshal.FreeHGlobal(keyPtr);

            if (rc != 0)
            {
                throw new PhotonException(PhotonException.PrepareException(sessionHandle, "HashMap.Insert"), rc);
            }
        }
    }
}
