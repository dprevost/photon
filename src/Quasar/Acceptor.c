/*
 * Copyright (C) 2006-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#include "Quasar/Quasar.h"
#include "Quasar/Acceptor.h"

#if ! defined(WIN32)
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#else
#  define MSG_NOSIGNAL 0
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static
bool Accept( qsrAcceptor * pAcceptor );
   
static
void Send( qsrAcceptor * pAcceptor, unsigned int indice );
   
static
void Receive( qsrAcceptor * pAcceptor, unsigned int indice );

static
void HandleAbnormalTermination( qsrAcceptor * pAcceptor, pid_t pid );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void qsrAcceptorInit( qsrAcceptor * pAcceptor )
{
   PSO_PRE_CONDITION( pAcceptor != NULL );

   pAcceptor->socketFD = PSO_INVALID_SOCKET;
#if defined (WIN32) 
   pAcceptor->cleanupNeeded = false;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void qsrAcceptorFini( qsrAcceptor * pAcceptor )
{
   PSO_PRE_CONDITION( pAcceptor != NULL );

   if ( pAcceptor->socketFD != PSO_INVALID_SOCKET ) {
#if defined (WIN32) 
      closesocket( pAcceptor->socketFD );
#else
      close( pAcceptor->socketFD );
#endif
      pAcceptor->socketFD = PSO_INVALID_SOCKET;
   }
   
#if defined (WIN32) 
   if ( pAcceptor->cleanupNeeded ) WSACleanup ();
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool Accept( qsrAcceptor * pAcceptor )
{
   PSO_SOCKET newSock = PSO_INVALID_SOCKET;
   int errcode, i;
#if defined (WIN32)
   unsigned long mode = 1;
#endif

#if ! defined (WIN32) 
   do {
#endif
      newSock = accept( pAcceptor->socketFD, NULL, 0 );
#if ! defined (WIN32)
   } while ( newSock == PSO_INVALID_SOCKET && errno == EINTR );
#endif
  
   if ( newSock == PSO_INVALID_SOCKET ) {
      errcode = GetSockError();
#if defined (WIN32) 
      if ( errcode == WSAEWOULDBLOCK )
         return true;
#else
      if ( errcode == EWOULDBLOCK || errcode == EAGAIN )
         return true;
#endif
      // The error is more serious...
      qsrSendMessage( &pAcceptor->pQuasar->log,
                       QSR_ERROR, 
                       "In function accept(), error = %d",
                       GetSockError() );
      return false;
   }
   
   /*
    * Set the new socket in non-blocking mode
    */

#if defined (WIN32)
   errcode = ioctlsocket( newSock, FIONBIO, &mode );
   if ( errcode == SOCKET_ERROR ) {
      qsrSendMessage( &pAcceptor->pQuasar->log,
                       QSR_ERROR, 
                       "In function ioctlsocket(), error = %d",
                       GetSockError() );
      return false;
   }
#else
   errcode = fcntl( newSock, F_SETFL, O_NONBLOCK);
   if ( errcode < 0 ) {
      qsrSendMessage( &pAcceptor->pQuasar->log, 
                       QSR_ERROR, 
                       "In function fcntl(), error = %d",
                       GetSockError() );
      return false;
   }
#endif

   for ( i = 1; i < FD_SETSIZE; ++i ) {
      if (  pAcceptor->dispatch[i].socketId == PSO_INVALID_SOCKET ) {
         pAcceptor->dispatch[i].socketId = newSock;
         break;
      }
   }
   
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void 
HandleAbnormalTermination( qsrAcceptor * pAcceptor, pid_t pid )
{
   qsrSendMessage( &pAcceptor->pQuasar->log, 
                    QSR_ERROR, 
                    "Abnormal termination of process %d",
                    pid );
//                                   " - attempting to recover." );

   qsrHandleAbnormalTermination( pAcceptor->pQuasar, pid );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool qsrPrepareConnection( qsrAcceptor * pAcceptor,
                           qsrQuasar   * pQuasar )
{
   int errcode = 0;
   int one = 1;
   unsigned short port;
   long int dummy = 0;
#if defined (WIN32) 
   WORD versionRequested;
   WSADATA wsaData;
   unsigned long mode = 1;
#endif
   struct sockaddr_in addr;

   PSO_PRE_CONDITION( pAcceptor != NULL );
   PSO_PRE_CONDITION( pQuasar   != NULL );

   pAcceptor->pQuasar = pQuasar;
   memset( &pAcceptor->answer, 0, sizeof pAcceptor->answer );
   strcpy( pAcceptor->answer.pathname, pAcceptor->pQuasar->params.memLocation );
   pAcceptor->answer.memorySizekb = pAcceptor->pQuasar->params.memorySizekb;

   dummy = strtol( pAcceptor->pQuasar->params.qsrAddress, NULL, 10 );
   if ( dummy <= 0 || dummy > 65535 ) {
      qsrSendMessage( &pAcceptor->pQuasar->log, QSR_ERROR, "Error getting port number" );
      return false;
   }
   port = (unsigned short) dummy;

#if defined (WIN32) 
   versionRequested = MAKEWORD( 2, 2 );
 
   errcode = WSAStartup( versionRequested, &wsaData );
   if ( errcode != 0 ) {
      qsrSendMessage( &pAcceptor->pQuasar->log, 
                       QSR_ERROR, 
                       "In function WSAStartup(), error = %d",
                       errcode );
      return false;
   }
   pAcceptor->cleanupNeeded = true;
   
#endif

   pAcceptor->socketFD = socket( PF_INET, SOCK_STREAM, 0 );
   if ( pAcceptor->socketFD == PSO_INVALID_SOCKET ) {
      qsrSendMessage( &pAcceptor->pQuasar->log, 
                       QSR_ERROR, 
                       "In function socket(), error = %d",
                       GetSockError() );
      return false;
   }
   
   errcode = setsockopt( pAcceptor->socketFD, SOL_SOCKET, SO_REUSEADDR, 
                         (const char *)&one, sizeof (one) );
   if ( errcode != 0 ) {
      qsrSendMessage( &pAcceptor->pQuasar->log, 
                       QSR_ERROR, 
                       "In function setsockopt(), error = %d",
                       GetSockError() );
      return false;
   }

   // Set the socket in non-blocking mode.

#if defined (WIN32)
   errcode = ioctlsocket( pAcceptor->socketFD, FIONBIO, &mode );
   if ( errcode == SOCKET_ERROR ) {
      qsrSendMessage( &pAcceptor->pQuasar->log, 
                       QSR_ERROR, 
                       "In function ioctlsocket(), error = %d",
                       GetSockError() );
      return false;
   }
#else
   errcode = fcntl( pAcceptor->socketFD, F_SETFL, O_NONBLOCK);
   if ( errcode < 0 ) {
      qsrSendMessage( &pAcceptor->pQuasar->log, 
                       QSR_ERROR, 
                       "In function fcntl(), error = %d",
                       GetSockError() );
      return false;
   }
#endif

   memset( &addr, 0, sizeof(struct sockaddr_in) );

   addr.sin_family = PF_INET;
   addr.sin_port = htons( port );
   addr.sin_addr.s_addr = inet_addr( "127.0.0.1" );

   errcode = bind( pAcceptor->socketFD, 
                   (struct sockaddr *) &addr,
                   sizeof(struct sockaddr_in) );
   if ( errcode != 0 ) {
      qsrSendMessage( &pAcceptor->pQuasar->log, 
                       QSR_ERROR, 
                       "In function bind(), error = %d",
                       GetSockError() );
      return false;
   }

   errcode = listen( pAcceptor->socketFD, 5 );
   if ( errcode != 0 ) {
      qsrSendMessage( &pAcceptor->pQuasar->log, 
                       QSR_ERROR, 
                       "In function listen(), error = %d",
                       GetSockError() );
      return false;
   }
   
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void
Receive( qsrAcceptor * pAcceptor, unsigned int indice )
{
   int errcode = 0;
   struct qsrInput input;
   
   PSO_PRE_CONDITION( pAcceptor != NULL );
   PSO_PRE_CONDITION( indice < FD_SETSIZE );

   /*
    * A socket can be in a ready state for reading when:
    *  - there is something to read
    *  - the connection is terminated (normally)
    *
    * Because of this it is better to use the MSG_PEEK flag before properly 
    * reading the socket. Even if this means uglier code and calling recv
    * twice...
    *
    * Because of the very small payload of the input request (struct qsrInput),
    * it is impossible (I think) to get the data truncated.
    */

   do {
      errcode = recv( pAcceptor->dispatch[indice].socketId,
                      (char *)&input,
                      sizeof input,
                      MSG_PEEK | MSG_NOSIGNAL );
   } while ( errcode == -1 && errno == EINTR );

   if ( errcode <= 0 ) {
      // Abnormal termination of the connection
#if defined (WIN32)
      closesocket( pAcceptor->dispatch[indice].socketId );
#else
      close( pAcceptor->dispatch[indice].socketId );
#endif
      pAcceptor->dispatch[indice].socketId = PSO_INVALID_SOCKET;

      if ( pAcceptor->dispatch[indice].pid > 0 ) {
         HandleAbnormalTermination( pAcceptor, pAcceptor->dispatch[indice].pid );
      }
      pAcceptor->dispatch[indice].pid = -1;
   }
   else {
      // Read the input data
      do {
         errcode = recv( pAcceptor->dispatch[indice].socketId,
                         (char *)&input,
                         sizeof input,
                         MSG_NOSIGNAL );
      } while ( errcode == -1 && errno == EINTR );

      if ( errcode != sizeof input ) {
         // Abnormal termination of the connection
#if defined (WIN32)
         closesocket( pAcceptor->dispatch[indice].socketId );
#else
         close( pAcceptor->dispatch[indice].socketId );
#endif
         pAcceptor->dispatch[indice].socketId = PSO_INVALID_SOCKET;
         if ( pAcceptor->dispatch[indice].pid > 0 ) {
            HandleAbnormalTermination( pAcceptor, pAcceptor->dispatch[indice].pid );
         }
         pAcceptor->dispatch[indice].pid = -1;
      }
      else {
         if ( input.opcode == QSR_CONNECT ) {
            pAcceptor->dispatch[indice].pid = input.processId;
            pAcceptor->dispatch[indice].dataToBeWritten = sizeof pAcceptor->answer;
         }
         else {
            // normal termination      
#if defined (WIN32)
            shutdown( pAcceptor->dispatch[indice].socketId, SD_BOTH );      
            closesocket( pAcceptor->dispatch[indice].socketId );
#else
            shutdown( pAcceptor->dispatch[indice].socketId, 2 );      
            close( pAcceptor->dispatch[indice].socketId );
#endif
            pAcceptor->dispatch[indice].socketId = PSO_INVALID_SOCKET;
         }
      }
   }
   
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void
Send( qsrAcceptor * pAcceptor, unsigned int indice )
{
   int errcode = 0;
   char * ptr;
   int offset;
   
   PSO_PRE_CONDITION( pAcceptor != NULL );
   PSO_PRE_CONDITION( indice < FD_SETSIZE );

   ptr = (char*) &pAcceptor->answer;
   offset = sizeof pAcceptor->answer - pAcceptor->dispatch[indice].dataToBeWritten;

   do {
      errcode = send( pAcceptor->dispatch[indice].socketId, 
                      &ptr[offset], 
                      pAcceptor->dispatch[indice].dataToBeWritten, 
                      MSG_NOSIGNAL );
   } while ( errcode == -1 && errno == EINTR );

   if ( errcode == -1 ) {
      qsrSendMessage( &pAcceptor->pQuasar->log, 
                       QSR_ERROR, 
                       "In function send(), error = %d",
                       GetSockError() );
      /*
       * The following attempt at recovering from a send() failure could 
       * well be a bit problematic. We should probably check the exact error
       * that cause the problem and decide which cleanup is needed based
       * on that error... 
       * Since the chance of an error is very small when using the loopback
       * network interface, the work on this will wait!!!
       *
       * \todo Make sure that the code for the cleanup of the sockets (when 
       * in error) is appropriate for the specific error encountered.
       */
#if defined (WIN32)
      shutdown( pAcceptor->dispatch[indice].socketId, SD_BOTH );      
      closesocket( pAcceptor->dispatch[indice].socketId );
#else
      if ( errno != EPIPE ) {
         shutdown( pAcceptor->dispatch[indice].socketId, 2 );      
         close( pAcceptor->dispatch[indice].socketId );
      }
#endif

      pAcceptor->dispatch[indice].socketId = PSO_INVALID_SOCKET;
      qsrSendMessage( &pAcceptor->pQuasar->log,
                       QSR_WARNING, 
                       "Connection terminated abnormally %s%d",
                       "for process ", 
                       pAcceptor->dispatch[indice].pid );      
      return;
   }
   
   pAcceptor->dispatch[indice].dataToBeWritten -= errcode;

   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void 
qsrWaitForConnections( qsrAcceptor * pAcceptor )
{
   int errcode = 0;
   fd_set readSet, writeSet;
   int maxFD, fired;
   struct timeval timeout;   
   unsigned int i;
   bool rc;
   
   PSO_PRE_CONDITION( pAcceptor != NULL );

   /*
    * NOTE: since socket handles, on Windows, are not integers, we will
    * not used the handles as indices to the pAcceptor->dispatch array
    */

   pAcceptor->dispatch[0].socketId = pAcceptor->socketFD;
   pAcceptor->dispatch[0].pid = 0;
   pAcceptor->dispatch[0].dataToBeWritten = false;
   
   for ( i = 1; i < FD_SETSIZE; ++i ) {
      pAcceptor->dispatch[i].socketId = PSO_INVALID_SOCKET;
      pAcceptor->dispatch[i].pid = -1;
      pAcceptor->dispatch[i].dataToBeWritten = false;
   }   

   while ( true ) {
      int zzz = 0;
      if ( g_pQSR->controlWord & QSR_SHUTDOWN_REQUEST ) {
         break;
      }
      
      timeout.tv_sec = 1;
      timeout.tv_usec = 0;
      FD_ZERO( &readSet  );
      FD_ZERO( &writeSet );
      maxFD = 0;
      for ( i = 0; i < FD_SETSIZE; ++i ) {
         
         if ( pAcceptor->dispatch[i].socketId != PSO_INVALID_SOCKET) {
            if ( pAcceptor->dispatch[i].dataToBeWritten == 0 ) {
               FD_SET( pAcceptor->dispatch[i].socketId, &readSet);
               zzz++;
            }
            else {
               FD_SET( pAcceptor->dispatch[i].socketId, &writeSet);
               zzz++;
            }
#if ! defined (WIN32)
            if ( pAcceptor->dispatch[i].socketId+1 > maxFD ) {
               maxFD = pAcceptor->dispatch[i].socketId+1;
            }
#endif
         }
      }

      do {
         fired = select( maxFD, &readSet, &writeSet, NULL, &timeout );
      } while ( fired == -1 && errno == EINTR );
      

      if ( fired == -1 ) {
         qsrSendMessage( &pAcceptor->pQuasar->log, 
                          QSR_ERROR, 
                          "In function select(), error = %d",
                          GetSockError() );
         errcode = -1;
         break;
      }
      if ( fired == 0 ) continue;
      
      /*
       * Start with the socket listening for new connection requests
       */
      if ( FD_ISSET( pAcceptor->socketFD, &readSet ) ) {
         rc = Accept( pAcceptor );
         PSO_POST_CONDITION( rc == true || rc == false );
         if ( ! rc ) break;
         fired--;
      }
      if ( fired == 0 ) continue;
      
      /*
       * Process all open sockets 
       */
      for ( i = 1; i < FD_SETSIZE; ++i ) {
         if ( pAcceptor->dispatch[i].socketId != PSO_INVALID_SOCKET ) {
            if ( FD_ISSET( pAcceptor->dispatch[i].socketId, &writeSet ) ) {
               Send( pAcceptor, i );
               fired--;
            }
            else if ( FD_ISSET( pAcceptor->dispatch[i].socketId, &readSet ) ) {
               Receive( pAcceptor, i );
               fired--;
            }
         }
         if ( fired == 0 ) break;
      }
   }

   // Cleanup (close all sockets)
   for ( i = 0; i < FD_SETSIZE; ++i ) {
      if ( pAcceptor->dispatch[i].socketId != PSO_INVALID_SOCKET ) {
#if defined (WIN32) 
         shutdown( pAcceptor->dispatch[i].socketId, SD_BOTH );      
         closesocket( pAcceptor->dispatch[i].socketId );
#else
         shutdown( pAcceptor->dispatch[i].socketId, 2 );      
         close( pAcceptor->dispatch[i].socketId );
#endif
      }
   }
   pAcceptor->socketFD = PSO_INVALID_SOCKET;

}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

