/*
 * Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined(WIN32)
#  define MSG_NOSIGNAL 0
#else
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#endif

#include "API/Connector.h"
#include <photon/psoErrors.h>

/** Send data on the socket. */
static int Send( psoaConnector    * pConnector,
                 void             * ptr, 
                 size_t             length,
                 psocErrorHandler * errorHandler);   

/** Receive data from the socket. */
static int Receive( psoaConnector    * pConnector,
                    void             * ptr, 
                    size_t             length,
                    psocErrorHandler * errorHandler );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaConnect( psoaConnector    * pConnector,
                 const char       * address,
                 struct qsrOutput * pAnswer,
                 psocErrorHandler * errorHandler )
{
   int errcode = 0;
   unsigned short port;
   struct sockaddr_in addr;
   long int dummy = 0;
#if defined (WIN32) 
   WORD versionRequested;
   WSADATA wsaData;
#endif
   struct qsrInput input;

   PSO_PRE_CONDITION( pConnector   != NULL );
   PSO_PRE_CONDITION( address      != NULL );
   PSO_PRE_CONDITION( pAnswer      != NULL );
   PSO_PRE_CONDITION( errorHandler != NULL );
   
   pConnector->socketFD = PSO_INVALID_SOCKET;
#if defined (WIN32)
   pConnector->cleanupNeeded = false;
#endif

   dummy = strtol( address, NULL, 10 );
   if ( dummy <= 0 || dummy > 65535 ) return PSO_INVALID_QUASAR_ADDRESS;
   port = (unsigned short) dummy;

#if defined (WIN32) 
   versionRequested = MAKEWORD( 2, 2 );
 
   errcode = WSAStartup( versionRequested, &wsaData );
   if ( errcode != 0 ) {
      psocSetError( errorHandler, PSOC_SOCKERR_HANDLE, WSAGetLastError() );
      return PSO_SOCKET_ERROR;
   }
   pConnector->cleanupNeeded = true;   
#endif

   pConnector->socketFD = socket( PF_INET, SOCK_STREAM, 0 );
   if ( pConnector->socketFD == PSO_INVALID_SOCKET ) {
#if defined (WIN32) 
      psocSetError( errorHandler, PSOC_SOCKERR_HANDLE, WSAGetLastError() );
#else
      psocSetError( errorHandler, PSOC_ERRNO_HANDLE, errno );
#endif
      return PSO_SOCKET_ERROR;
   }
   
   memset( &addr, 0, sizeof(struct sockaddr_in) );
   addr.sin_family = PF_INET;
   addr.sin_port = htons( port );
   addr.sin_addr.s_addr = inet_addr( "127.0.0.1" );

   errcode = connect( pConnector->socketFD, 
                      (const struct sockaddr *)&addr,
                      sizeof addr );
   if ( errcode != 0 ) {
#if defined (WIN32) 
      psocSetError( errorHandler, PSOC_SOCKERR_HANDLE, WSAGetLastError() );
#else
      psocSetError( errorHandler, PSOC_ERRNO_HANDLE, errno );
#endif
      return PSO_CONNECT_ERROR;
   }

   input.opcode = QSR_CONNECT;
   input.processId = getpid();

   errcode = Send( pConnector, &input, sizeof(struct qsrInput), errorHandler );
   if ( errcode != 0 ) return PSO_SEND_ERROR;

   errcode = Receive( pConnector, pAnswer, sizeof(struct qsrOutput), errorHandler );
   if ( errcode != 0 ) return PSO_RECEIVE_ERROR;

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psoaDisconnect( psoaConnector    * pConnector,
                     psocErrorHandler * errorHandler )
{
   int errcode = 0;

   struct qsrInput input;
   input.opcode = QSR_DISCONNECT;
   input.processId = getpid();

   PSO_PRE_CONDITION( pConnector   != NULL );
   PSO_PRE_CONDITION( errorHandler != NULL );

   if ( pConnector->socketFD != PSO_INVALID_SOCKET ) {

      errcode = Send( pConnector, &input, sizeof(struct qsrInput), errorHandler );
      /**
       * \todo Remove this debug info when we are sure it is ok (?)
       * or maybe replace it with an assert? 
       */
      if ( errcode != 0 ) {
         fprintf( stderr, "Error in Connector::Disconnect Send(): %d\n",
                  errcode );
      }
      
#if defined (WIN32)
      shutdown( pConnector->socketFD, SD_BOTH );
      closesocket( pConnector->socketFD );
#else
      shutdown( pConnector->socketFD, 2 );
      close( pConnector->socketFD );
#endif
      pConnector->socketFD = PSO_INVALID_SOCKET;
   }
   
#if defined (WIN32) 
   if ( pConnector->cleanupNeeded ) {
      WSACleanup ();
      /* To make sure we don't call WSACleanup twice */
      pConnector->cleanupNeeded = false;
   }
#endif

   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int Receive( psoaConnector    * pConnector,
             void             * ptr, 
             size_t             length,
             psocErrorHandler * errorHandler )
{
   int errcode = 0;

   PSO_PRE_CONDITION( pConnector   != NULL );
   PSO_PRE_CONDITION( ptr          != NULL );
   PSO_PRE_CONDITION( errorHandler != NULL );
   PSO_PRE_CONDITION( length > 0 );

   /*
    * The do while loop is useless on Win32 but it does not hurt either...
    */
   errno = 0;
   do {
      errcode = recv( pConnector->socketFD,
                      ptr,
                      length,
                      MSG_NOSIGNAL );
   } while ( errcode == -1 && errno == EINTR );
   
   if ( errcode != (int) length ) {
#if defined (WIN32)
      psocSetError( errorHandler, PSOC_SOCKERR_HANDLE, WSAGetLastError() );
      shutdown( pConnector->socketFD, SD_BOTH );      
      closesocket( pConnector->socketFD );
#else
      psocSetError( errorHandler, PSOC_ERRNO_HANDLE, errno );
      shutdown( pConnector->socketFD, 2 );      
      close( pConnector->socketFD );
#endif
      pConnector->socketFD = PSO_INVALID_SOCKET;
      return -1;
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int Send( psoaConnector    * pConnector,
          void             * ptr, 
          size_t             length,
          psocErrorHandler * errorHandler )
{
   int errcode = 0;
   
   PSO_PRE_CONDITION( pConnector   != NULL );
   PSO_PRE_CONDITION( ptr          != NULL );
   PSO_PRE_CONDITION( errorHandler != NULL );
   PSO_PRE_CONDITION( length > 0 );

   /*
    * The do while loop is useless on Win32 but it does not hurt either...
    */
   errno = 0;
   do {
      errcode = send( pConnector->socketFD, 
                      ptr, 
                      length,
                      MSG_NOSIGNAL );
   } while ( errcode == -1 && errno == EINTR );

   if ( errcode != (int) length ) {
#if defined (WIN32)
      psocSetError( errorHandler, PSOC_SOCKERR_HANDLE, WSAGetLastError() );
      shutdown( pConnector->socketFD, SD_BOTH );      
      closesocket( pConnector->socketFD );
#else
      psocSetError( errorHandler, PSOC_ERRNO_HANDLE, errno );
      shutdown( pConnector->socketFD, 2 );
      close( pConnector->socketFD );
#endif
      pConnector->socketFD = PSO_INVALID_SOCKET;
      return -1;
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

