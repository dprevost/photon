using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace Photon
{
    class Process
    {
        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void psoExit();

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoInit( 
            string quasarAddress,
            int    protectionNeeded );

        public void Init(string address, bool protectionNeeded)
        {
            int rc;
            if (protectionNeeded) rc = psoInit(address, 1);
            else rc = psoInit(address, 0);
            if (rc != 0)
            {
                throw new PhotonException(PhotonException.PrepareException("Process::Init", rc));
            }
        }
    }
}
