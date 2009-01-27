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

namespace Photon
{
    public partial class Process: IDisposable
    {
        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        // protectionNeeded should usually be false except under these conditions:
        //  - you are sharing sessions between threads (not recommended!)
        //  - you call Thread.Abort() to terminate your threads (this may leave 
        //    unclosed Photon handles in the aborted thread which will require
        //    the cleanup code in the unmanaged code (posExit()) to use an internal lock).
        //  - your process terminates while threads are still running (same point as before).
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

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

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

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
    }
}
