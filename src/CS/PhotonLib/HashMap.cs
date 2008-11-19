using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace Photon
{
    public class HashMap : IDisposable
    {
        // Track whether Dispose has been called.
        private bool disposed = false;

        protected IntPtr handle;
        protected IntPtr sessionHandle;

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoHashMapClose(IntPtr objectHandle);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoHashMapDefinition(
            IntPtr objectHandle,
            ref ObjectDefinition definition);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        protected static extern int psoHashMapDelete(
            IntPtr objectHandle,
            IntPtr key,
            IntPtr keyLength);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoHashMapGet(
            IntPtr objectHandle,
            byte[] key,
            IntPtr keyLength,
            byte[] buffer,
            IntPtr bufferLength,
            ref IntPtr returnedLength);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoHashMapGetFirst(
            IntPtr objectHandle,
            byte[] key,
            IntPtr keyLength,
            byte[] buffer,
            IntPtr bufferLength,
            ref IntPtr retKeyLength,
            ref IntPtr retDataLength);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoHashMapGetNext(
            IntPtr objectHandle,
            byte[] key,
            IntPtr keyLength,
            byte[] buffer,
            IntPtr bufferLength,
            ref IntPtr retKeyLength,
            ref IntPtr retDataLength);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        protected static extern int psoHashMapInsert(
            IntPtr objectHandle,
            IntPtr key,
            IntPtr keyLength,
            IntPtr data,
            IntPtr dataLength);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoHashMapOpen(
            IntPtr sessionHandle,
            string hashMapName,
            IntPtr nameLengthInBytes,
            ref IntPtr objectHandle);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        protected static extern int psoHashMapReplace(
            IntPtr objectHandle,
            IntPtr key,
            IntPtr keyLength,
            IntPtr data,
            IntPtr dataLength);

        [DllImport("photon.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int psoHashMapStatus(
            IntPtr objectHandle,
            ref ObjStatus pStatus);

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

        private void Dispose(bool disposing)
        {
            // Check to see if Dispose has already been called.
            if (!this.disposed)
            {
                psoHashMapClose(handle);
                handle = (IntPtr)0;
            }
            disposed = true;
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        ~HashMap()
        {
            Dispose(false);
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

        /*
void HashMap::Delete( const void * key,
                      size_t       keyLength )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "HashMap::Delete", PSO_NULL_HANDLE );
   }

   rc = psoHashMapDelete( m_objectHandle,
                          key,
                          keyLength );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "HashMap::Delete" );
   }
}
         */

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
        /*
        void HashMap::Get( const void * key,
                           size_t       keyLength,
                           void       * buffer,
                           size_t       bufferLength,
                           size_t     & returnedLength )
        {
           int rc;
   
           if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
              throw pso::Exception( "HashMap::Get", PSO_NULL_HANDLE );
           }

           rc = psoHashMapGet( m_objectHandle,
                               key,
                               keyLength,
                               buffer,
                               bufferLength,
                               &returnedLength );
           if ( rc != 0 ) {
              throw pso::Exception( m_sessionHandle, "HashMap::Get" );
           }
        }
        */
        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
        /*
        int HashMap::GetFirst( void   * key,
                               size_t   keyLength,
                               void   * buffer,
                               size_t   bufferLength,
                               size_t & retKeyLength,
                               size_t & retDataLength )
        {
           int rc;
   
           if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
              throw pso::Exception( "HashMap::GetFirst", PSO_NULL_HANDLE );
           }

           rc = psoHashMapGetFirst( m_objectHandle,
                                    key,
                                    keyLength,
                                    buffer,
                                    bufferLength,
                                    &retKeyLength,
                                    &retDataLength );
           if ( rc != 0 && rc != PSO_IS_EMPTY ) {
              throw pso::Exception( m_sessionHandle, "HashMap::GetFirst" );
           }
   
           return rc;
        }
        */
        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
        /*
        int HashMap::GetNext( void   * key,
                              size_t   keyLength,
                              void   * buffer,
                              size_t   bufferLength,
                              size_t & retKeyLength,
                              size_t & retDataLength )
        {
           int rc;
   
           if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
              throw pso::Exception( "HashMap::GetNext", PSO_NULL_HANDLE );
           }

           rc = psoHashMapGetNext( m_objectHandle,
                                   key,
                                   keyLength,
                                   buffer,
                                   bufferLength,
                                   &retKeyLength,
                                   &retDataLength );
           if ( rc != 0 && rc != PSO_REACHED_THE_END ) {
              throw pso::Exception( m_sessionHandle, "HashMap::GetNext" );
           }

           return rc;
        }
        */
        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
        /*
        void HashMap::Insert( const void * key,
                              size_t       keyLength,
                              const void * data,
                              size_t       dataLength )
        {
           int rc;
   
           if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
              throw pso::Exception( "HashMap::Insert", PSO_NULL_HANDLE );
           }

           rc = psoHashMapInsert( m_objectHandle,
                                  key,
                                  keyLength,
                                  data,
                                  dataLength );
           if ( rc != 0 ) {
              throw pso::Exception( m_sessionHandle, "HashMap::Insert" );
           }
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
                                 (IntPtr)hashMapName.Length,
                                 ref handle);
            if (rc != 0)
            {
                throw new PhotonException(PhotonException.PrepareException(sessionHandle, "HashMap.Open"), rc);
            }
        }

        // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

        /*
        void HashMap::Replace( const void * key,
                               size_t       keyLength,
                               const void * data,
                               size_t       dataLength )
        {
           int rc;
   
           if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
              throw pso::Exception( "HashMap::Replace", PSO_NULL_HANDLE );
           }

           rc = psoHashMapReplace( m_objectHandle,
                                   key,
                                   keyLength,
                                   data,
                                   dataLength );
           if ( rc != 0 ) {
              throw pso::Exception( m_sessionHandle, "HashMap::Replace" );
           }
        }
        */
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
