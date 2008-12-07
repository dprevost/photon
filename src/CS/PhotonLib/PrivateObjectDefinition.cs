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
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace Photon
{
    public partial class ObjectDefinition
    {
        /*
         * Description of the structure of the hash map key.
         */
        [StructLayout(LayoutKind.Sequential)]
        private struct psoKeyDefinition
        {
            /* The data type of the key. */
            public psoKeyType type;

            /** For fixed-length data types */
            public IntPtr length;

            /** For variable-length data types */
            public IntPtr minLength;

            /** For variable-length data types */
            public IntPtr maxLength;
        }

        /*
         * Description of the structure of the data (if any).
         *
         * This structure is aligned in such a way that you can do:
         *
         *    malloc( offsetof(psoObjectDefinition, fields) +
         *            numFields * sizeof(psoFieldDefinition) );
         *
         */
        [StructLayout(LayoutKind.Sequential)]
        private struct psoFieldDefinition
        {
            /** The name of the field. */
            [MarshalAs(UnmanagedType.LPStr, SizeConst=32)]String name;
   
            /** The data type of the field/ */
            psoFieldType type;
   
            /** For fixed-length data types */
            IntPtr length;

            /** For variable-length data types */
            IntPtr minLength;

            /** For variable-length data types */
            IntPtr maxLength;

            /** Total number of digits in the decimal field. */
            IntPtr precision;

            /** Number of digits following the decimal separator. */
            IntPtr scale;
        }

        /*
         * This struct has a variable length.
         */
        [StructLayout(LayoutKind.Sequential)]
        struct psoObjectDefinition
        {
            /** The object type. */
            ObjectType type;

            /** The number of fields in the definition. */
            UInt32 numFields;
   
            /** The data definition of the key (hash map/fast map only) */
            psoKeyDefinition key;

            /** The data definition of the fields */
            psoFieldDefinition[] fields;
        }


    }
}
