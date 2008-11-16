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

    }
}
