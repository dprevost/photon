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
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace Photon
{
    public class PhotonException : System.Exception
    {
        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        /*
         * Declarations to access the photon C API.
         */
        [DllImport("photonNucleus.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern string pson_ErrorMessage(int errcode);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoErrorMsg(
            IntPtr sessionHandle,
            [MarshalAs(UnmanagedType.LPStr)] StringBuilder message,
            IntPtr msgLengthInBytes);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoLastError(IntPtr sessionHandle);

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        private int theErrorCode;
        public int ErrorCode() { return theErrorCode; }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        /*
         * Contructor. 
         */
        public PhotonException(string message, int errcode) : base(message) { theErrorCode = errcode; }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        /*
         * This static function uses the Photon API to extract the error message
         * and pass it to the constructor.
         */
        public static string PrepareException(string functionName, int errcode)
        {
            string str = pson_ErrorMessage( errcode );

            if (str == null || str.Length == 0)
            {
                str = functionName
                    + " exception: Cannot retrieve the error message - the error code is "
                    + errcode;
            }
            return str;
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        /*
         * This static function uses the Photon API to extract the error message
         * and pass it to the constructor.
         */
        public static string PrepareException(IntPtr sessionHandle, string functionName)
        {
            StringBuilder s = new StringBuilder(1024);
            int rc = 1, errcode;
            string msg = functionName + " exception: ";

            rc = psoErrorMsg(sessionHandle, s, (IntPtr)1024);
            if (rc == 0)
            {
                msg += " exception: ";
                msg += s.ToString();
            }
            else
            {
                errcode = psoLastError(sessionHandle);
                msg = functionName
                    + " exception: Cannot retrieve the error message - the error code is "
                    + errcode;
            }
            
            return msg;
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
    }
}
