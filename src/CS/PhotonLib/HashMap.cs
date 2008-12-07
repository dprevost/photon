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
    public partial class HashMap : IDisposable
    {
        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        public HashMap(Session session)
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

        public void Close() { Dispose(); }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        /*
        void Definition( ObjDefinition & definition )
{
   int rc;
   psoObjectDefinition * def = NULL;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "HashMap::Definition", PSO_NULL_HANDLE );
   }

   rc = psoHashMapDefinition( m_objectHandle, &def );   
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "HashMap::Definition" );
   }
   
   // We catch and rethrow the exception to avoid a memory leak
   try {
      definition.Reset( *def );
   }
   catch( pso::Exception exc ) {
      free( def );
      throw exc;
   }
   
   free( def );
}
*/



        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        void Open(string hashMapName)
        {
            int rc;

            if (sessionHandle == (IntPtr)0)
            {
                rc = (int)PhotonErrors.NULL_HANDLE;
                throw new PhotonException(PhotonException.PrepareException("HashMap.Open", rc), rc);
            }

            rc = psoHashMapOpen(sessionHandle,
                                 hashMapName,
                                 (UInt32)hashMapName.Length,
                                 ref handle);
            if (rc != 0)
            {
                throw new PhotonException(PhotonException.PrepareException(sessionHandle, "HashMap.Open"), rc);
            }
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        public void Status(ref ObjStatus status)
        {
            int rc;

            if (sessionHandle == (IntPtr)0 || handle == (IntPtr)0)
            {
                rc = (int)PhotonErrors.NULL_HANDLE;
                throw new PhotonException(PhotonException.PrepareException("HashMap.Status", rc), rc);
            }

            rc = psoHashMapStatus(handle, ref status);
            if (rc != 0)
            {
                throw new PhotonException(PhotonException.PrepareException(sessionHandle, "HashMap.Status"), rc);
            }
        }
    }
}
