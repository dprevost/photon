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
