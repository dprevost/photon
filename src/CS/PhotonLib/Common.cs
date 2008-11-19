using System;
using System.Runtime.InteropServices;

namespace Photon
{
    /**
 * The object type as seen from the API
 */
    public enum ObjectType
    {
        FOLDER = 1,
        HASH_MAP = 2,
        LIFO = 3,  /* A LIFO queue aka a stack */
        FAST_MAP = 4,  /* A read-only hash map */
        QUEUE = 5,  /* A FIFO queue */
    }
    // Photon supported data types.

    enum psoFieldType
    {
        PSO_INTEGER = 1,
        PSO_BINARY,
        PSO_STRING,
        /** The decimal type should be mapped to an array of bytes of length
         *  precision + 2 (optional sign and the decimal separator).
         */
        PSO_DECIMAL,
        /** The boolean type should be mapped to a single byte in a C struct. */
        PSO_BOOLEAN,
        /** Only valid for the last field of the data definition */
        PSO_VAR_BINARY,
        /** Only valid for the last field of the data definition */
        PSO_VAR_STRING
    }

    /**
     * Photon supported data types for keys.
     */
    enum psoKeyType
    {
        PSO_KEY_INTEGER = 101,
        PSO_KEY_BINARY,
        PSO_KEY_STRING,
        /** Only valid for the last field of the data definition */
        PSO_KEY_VAR_BINARY,
        /** Only valid for the last field of the data definition */
        PSO_KEY_VAR_STRING
    }

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