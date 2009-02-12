/*
 * Copyright (C) 2008-2009 Daniel Prevost <dprevost@photonsoftware.org>
 *
 * This file is part of Photon (photonsoftware.org).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 or version 3 as published by the 
 * Free Software Foundation and appearing in the file COPYING.GPL2 and 
 * COPYING.GPL3 included in the packaging of this software.
 *
 * Licensees holding a valid Photon Commercial license can use this file 
 * in accordance with the terms of their license.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

using System;
using System.Runtime.InteropServices;

namespace Photon
{
    // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

    /*
     * enums in C# are ints, 4 bytes long.  
     */
    public enum ObjectType 
    {
        FOLDER   = 1,
        HASH_MAP = 2,
        LIFO     = 3,  // A LIFO queue aka a stack 
        FAST_MAP = 4,  // A read-only hash map 
        QUEUE    = 5,  // A FIFO queue 
    }

    // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

    // Photon supported data types.
    public enum FieldType
    {
        INTEGER = 1,
        BINARY,
        STRING,
        /* The decimal type should be mapped to an array of bytes of length
         *  precision + 2 (optional sign and the decimal separator).
         */
        DECIMAL,
        /** The boolean type should be mapped to a single byte in a C struct. */
        BOOLEAN,
        /* Only valid for the last field of the data definition */
        VAR_BINARY,
        /* Only valid for the last field of the data definition */
        VAR_STRING
    }

    // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

    // Photon supported data types for keys.
    public enum KeyType
    {
        KEY_INTEGER = 101,
        KEY_BINARY,
        KEY_STRING,
        /* Only valid for the last field of the data definition */
        KEY_VAR_BINARY,
        /* Only valid for the last field of the data definition */
        KEY_VAR_STRING
    }

    // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

    [StructLayout(LayoutKind.Sequential)]
    public struct FolderEntry
    {
        [MarshalAs(UnmanagedType.LPStr, SizeConst = 256)]
        String     name;
        ObjectType type;
        UInt32     status;
        UInt32     nameLengthInBytes;
        
    }

    // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

    [StructLayout(LayoutKind.Sequential)]
    public struct ObjStatus
    {
        /* The object type. */
        ObjectType type;

        /*
         * Status of the object. For example, created but not committed, etc.
         */
        UInt32 status;

        /* The number of blocks allocated to this object. */
        IntPtr numBlocks;

        /* The number of groups of blocks allocated to this object. */
        IntPtr numBlockGroup;

        /* The number of data items in this a object. */
        IntPtr numDataItem;

        /* The amount of free space available in the blocks allocated to this object. */
        IntPtr freeBytes;

        /* Maximum data length (in bytes). */
        UInt32 maxDataLength;

        /* Maximum key length (in bytes) if keys are supported - zero otherwise */
        UInt32 maxKeyLength;
    }

    // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

    [StructLayout(LayoutKind.Sequential)]
    public struct Info
    {
    }

    // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

    /*
     * Description of the structure of the hash map key.
     */
    [StructLayout(LayoutKind.Sequential)]
    public struct KeyDefinition
    {
        /* The data type of the key. */
        public KeyType type;

        /** For fixed-length data types */
        public UInt32 length;

        /** For variable-length data types */
        public UInt32 minLength;

        /** For variable-length data types */
        public UInt32 maxLength;
    }

    // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

    /*
     * Description of the structure of the fields.
     */
    [StructLayout(LayoutKind.Sequential)]
    public struct FieldDefinition
    {
        /** The name of the field. */
        [MarshalAs(UnmanagedType.LPStr, SizeConst = 32)]
        String name;

        /** The data type of the field/ */
        FieldType type;

        /** For fixed-length data types */
        UInt32 length;

        /** For variable-length data types */
        UInt32 minLength;

        /** For variable-length data types */
        UInt32 maxLength;

        /** Total number of digits in the decimal field. */
        UInt32 precision;

        /** Number of digits following the decimal separator. */
        UInt32 scale;
    }

    // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

    /*
     * 
     */
    [StructLayout(LayoutKind.Sequential)]
    public struct ObjectDefinition
    {
        /** The object type. */
        public ObjectType type;

        /** The number of fields in the definition. */
        public UInt32 numFields;

    }

}