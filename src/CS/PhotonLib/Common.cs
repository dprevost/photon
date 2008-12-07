/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@photonsoftware.org>
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
    
    // Photon supported data types.
    public enum psoFieldType
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

    // Photon supported data types for keys.
    public enum psoKeyType
    {
        KEY_INTEGER = 101,
        KEY_BINARY,
        KEY_STRING,
        /* Only valid for the last field of the data definition */
        KEY_VAR_BINARY,
        /* Only valid for the last field of the data definition */
        KEY_VAR_STRING
    }

    [StructLayout(LayoutKind.Sequential)]
    internal struct FolderEntry
    {
        [MarshalAs(UnmanagedType.LPStr, SizeConst = 256)]
        String name;
        ObjectType type;
        int status;
        IntPtr nameLengthInBytes;
        
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct ObjStatus
    {
        /* The object type. */
        ObjectType type;

        /*
         * Status of the object. For example, created but not committed, etc.
         */
        int status;

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

    [StructLayout(LayoutKind.Sequential)]
    internal struct Info
    {
    }

}