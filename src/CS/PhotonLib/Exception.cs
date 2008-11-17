using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace Photon
{
    public class PhotonException : System.Exception
    {
        public PhotonException(string message) : base(message) { }

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern string pson_ErrorMessage(int errcode);
       
        internal static string PrepareException(string functionName, int theErrorCode)
        {
            string str = pson_ErrorMessage( theErrorCode );

            if (str == null || str.Length == 0)
            {
                str = "Cannot retrieve the error message - the error code is ";
                str += theErrorCode;
            }
            return str;
        }

/*
 *      internal static string PrepareException( IntPtr sessionHandle, string functionName )
        {
            char s[1024];
            int rc = 1;
            ostringstream oss;
   
            rc = psoErrorMsg( sessionHandle, s, 1024 );
            errcode = psoLastError( sessionHandle );
            msg = functionName;
            msg += " exception: ";
            msg += s;
   
            if ( rc != 0 ) {
                // We build our own message
                oss << functionName << " exception: ";
                oss << "Cannot retrieve the error message ";
                msg = oss.str();
            }
        }
*/
    }

}
