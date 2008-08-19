/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of vdsf (Virtual Data Space Framework).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation and appearing in the file COPYING included in the
 * packaging of this library.
 *
 * This file is distributed in the hope that it will be useful,
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
#include <vdsf/vdsErrors.h>

/** Send data on the socket. */
static int Send( psaConnector    * pConnector,
                 void            * ptr, 
                 size_t            length,
                 pscErrorHandler * errorHandler);   

/** Receive data from the socket. */
static int Receive( psaConnector    * pConnector,
                    void            * ptr, 
                    size_t            length,
                    pscErrorHandler * errorHandler );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psaConnect( psaConnector    * pConnector,
                const char      * address,
                struct WDOutput * pAnswer,
                pscErrorHandler * errorHandler )
{
   int errcode = 0;
   unsigned short port;
   struct sockaddr_in addr;
   long int dummy = 0;
#if defined (WIN32) 
   WORD versionRequested;
   WSADATA wsaData;
#endif
   struct WDInput input;

   VDS_PRE_CONDITION( pConnector   != NULL );
   VDS_PRE_CONDITION( address      != NULL );
   VDS_PRE_CONDITION( pAnswer      != NULL );
   VDS_PRE_CONDITION( errorHandler != NULL );
   
   pConnector->socketFD = VDS_INVALID_SOCKET;
#if defined (WIN32)
   pConnector->cleanupNeeded = false;
#endif

   dummy = strtol( address, NULL, 10 );
   if ( dummy <= 0 || dummy > 65535 ) return VDS_INVALID_WATCHDOG_ADDRESS;
   port = (unsigned short) dummy;

#if defined (WIN32) 
   versionRequested = MAKEWORD( 2, 2 );
 
   errcode = WSAStartup( versionRequested, &wsaData );
   if ( errcode != 0 ) {
      pscSetError( errorHandler, PSC_SOCKERR_HANDLE, WSAGetLastError() );
      return VDS_SOCKET_ERROR;
   }
   pConnector->cleanupNeeded = true;   
#endif

   pConnector->socketFD = socket( PF_INET, SOCK_STREAM, 0 );
   if ( pConnector->socketFD == VDS_INVALID_SOCKET ) {
#if defined (WIN32) 
      pscSetError( errorHandler, PSC_SOCKERR_HANDLE, WSAGetLastError() );
#else
      pscSetError( errorHandler, PSC_ERRNO_HANDLE, errno );
#endif
      return VDS_SOCKET_ERROR;
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
      pscSetError( errorHandler, PSC_SOCKERR_HANDLE, WSAGetLastError() );
#else
      pscSetError( errorHandler, PSC_ERRNO_HANDLE, errno );
#endif
      return VDS_CONNECT_ERROR;
   }

   input.opcode = WD_CONNECT;
   input.processId = getpid();

   errcode = Send( pConnector, &input, sizeof(struct WDInput), errorHandler );
   if ( errcode != 0 ) return VDS_SEND_ERROR;

   errcode = Receive( pConnector, pAnswer, sizeof(struct WDOutput), errorHandler );
   if ( errcode != 0 ) return VDS_RECEIVE_ERROR;

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psaDisconnect( psaConnector    * pConnector,
                    pscErrorHandler * errorHandler )
{
   int errcode = 0;

   struct WDInput input;
   input.opcode = WD_DISCONNECT;
   input.processId = getpid();

   VDS_PRE_CONDITION( pConnector   != NULL );
   VDS_PRE_CONDITION( errorHandler != NULL );

   if ( pConnector->socketFD != VDS_INVALID_SOCKET ) {

      errcode = Send( pConnector, &input, sizeof(struct WDInput), errorHandler );
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
      pConnector->socketFD = VDS_INVALID_SOCKET;
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

int Receive( psaConnector    * pConnector,
             void            * ptr, 
             size_t            length,
             pscErrorHandler * errorHandler )
{
   int errcode = 0;

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
      pscSetError( errorHandler, PSC_SOCKERR_HANDLE, WSAGetLastError() );
      shutdown( pConnector->socketFD, SD_BOTH );      
      closesocket( pConnector->socketFD );
#else
      pscSetError( errorHandler, PSC_ERRNO_HANDLE, errno );
      shutdown( pConnector->socketFD, 2 );      
      close( pConnector->socketFD );
#endif
      pConnector->socketFD = VDS_INVALID_SOCKET;
      return -1;
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int Send( psaConnector    * pConnector,
          void            * ptr, 
          size_t            length,
          pscErrorHandler * errorHandler )
{
   int errcode = 0;
   
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
      pscSetError( errorHandler, PSC_SOCKERR_HANDLE, WSAGetLastError() );
      shutdown( pConnector->socketFD, SD_BOTH );      
      closesocket( pConnector->socketFD );
#else
      pscSetError( errorHandler, PSC_ERRNO_HANDLE, errno );
      shutdown( pConnector->socketFD, 2 );
      close( pConnector->socketFD );
#endif
      pConnector->socketFD = VDS_INVALID_SOCKET;
      return -1;
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

