using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace Photon
{
    public class Process: IDisposable
    {
        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void psoExit();

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoInit( 
            string quasarAddress,
            int    protectionNeeded );

        // Track whether Dispose has been called.
        private bool disposed = false;

        public void Init(string address, bool protectionNeeded)
        {
            int rc;
            if (protectionNeeded) rc = psoInit(address, 1);
            else rc = psoInit(address, 0);
            if (rc != 0)
            {
                throw new PhotonException(PhotonException.PrepareException("Process::Init", rc), rc);
            }
        }

        // Implement IDisposable.
        public void Dispose()
        {
            Dispose(true);
            // This object will be cleaned up by the Dispose method.
            // We call GC.SupressFinalize to take this object off the 
            // finalization queue and prevent finalization code for this 
            // object from executing a second time.
            GC.SuppressFinalize(this);
        }

        // Dispose(bool disposing) executes in two distinct scenarios.
        // If disposing equals true, the method has been called directly
        // or indirectly by a user's code. Managed and unmanaged resources
        // can be disposed.
        // If disposing equals false, the method has been called by the 
        // runtime from inside the finalizer and you should not reference 
        // other objects. Only unmanaged resources can be disposed.
        private void Dispose(bool disposing)
        {
            // Check to see if Dispose has already been called. Although 
            // technically, calling psoExit twice won't hurt.
            if (!this.disposed)
            {
                psoExit();
            }
            disposed = true;
        }

        ~Process()      
        {
            Dispose(false);
        }
    }
}
