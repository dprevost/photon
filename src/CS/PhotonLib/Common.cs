using System;
using System.Runtime.InteropServices;

namespace Photon
{
    [StructLayout(LayoutKind.Sequential)]
    internal struct FolderEntry
    {
        int type;
        int status;
        IntPtr nameLengthInBytes;
        [MarshalAs(UnmanagedType.LPStr, SizeConst=256)]String name;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct ObjStatus
    {
        /** The object type. */
        int type;

        /**
         * Status of the object.
         *
         * For example, created but not committed, etc.
         */
        int status;

        /** The number of blocks allocated to this object. */
        IntPtr numBlocks;

        /** The number of groups of blocks allocated to this object. */
        IntPtr numBlockGroup;

        /** The number of data items in thisa object. */
        IntPtr numDataItem;

        /** The amount of free space available in the blocks allocated to this object. */
        IntPtr freeBytes;

        /** Maximum data length (in bytes). */
        IntPtr maxDataLength;

        /** Maximum key length (in bytes) if keys are supported - zero otherwise */
        IntPtr maxKeyLength;
    }

    [StructLayout(LayoutKind.Sequential)]
    internal struct Info
    {
    }

}