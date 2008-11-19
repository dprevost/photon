using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace Photon
{
    public partial class Folder: IDisposable
    {
        public Folder(Session session)
        {
            handle = (IntPtr)0;
            sessionHandle = session.handle;
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

    }
}
