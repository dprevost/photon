/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#include "Acceptor.h"
#include "Watchdog.h"

#if ! defined(WIN32)
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#else
#  define MSG_NOSIGNAL 0
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static
int Accept( vdswAcceptor * pAcceptor );
   
static
void Send( vdswAcceptor * pAcceptor, int indice );
   
static
void Receive( vdswAcceptor * pAcceptor, int indice );

static
void HandleAbnormalTermination( vdswAcceptor * pAcceptor, pid_t pid );

static
bool IsconnectionAlive( vdswAcceptor * pAcceptor, int indice );
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdswAcceptorInit( vdswAcceptor * pAcceptor )
{
   VDS_PRE_CONDITION( pAcceptor != NULL );

   pAcceptor->socketFD = VDS_INVALID_SOCKET;
#if defined (WIN32) 
   pAcceptor->cleanupNeeded = false;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdswAcceptorFini( vdswAcceptor * pAcceptor )
{
   if ( pAcceptor->socketFD != VDS_INVALID_SOCKET ) {
#if defined (WIN32) 
      closesocket( pAcceptor->socketFD );
#else
      close( pAcceptor->socketFD );
#endif
      pAcceptor->socketFD = VDS_INVALID_SOCKET;
   }
   
#if defined (WIN32) 
   if ( pAcceptor->cleanupNeeded ) WSACleanup ();
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int Accept( vdswAcceptor * pAcceptor )
{
   VDS_SOCKET newSock = VDS_INVALID_SOCKET;
   int errcode, i;
   
#if ! defined (WIN32) 
   do {
#endif
      newSock = accept( pAcceptor->socketFD, NULL, 0 );
#if ! defined (WIN32)
   } while ( newSock == VDS_INVALID_SOCKET && errno == EINTR );
#endif
  
   if ( newSock == VDS_INVALID_SOCKET ) {
      errcode = GetSockError();
#if defined (WIN32) 
      if ( errcode == WSAEWOULDBLOCK )
         return 0;
#else
      if ( errcode == EWOULDBLOCK || errcode == EAGAIN )
         return 0;
#endif
      // The error is more serious...
      vdswSendMessage( &pAcceptor->pWatchdog->log,
                       WD_ERROR, 
                       "In function accept(), error = %d",
                       GetSockError() );
      return -1;
   }
   
   /*
    * Set the new socket in non-blocking mode
    */

#if defined (WIN32)
   unsigned long mode = 1;
   errcode = ioctlsocket( newSock, FIONBIO, &mode );
   if ( errcode == SOCKET_ERROR ) {
      vdswSendMessage( &pAcceptor->pWatchdog->log,
                       WD_ERROR, 
                       "In function ioctlsocket(), error = %d",
                       GetSockError() );
      return -1;
   }
#else
   errcode = fcntl( newSock, F_SETFL, O_NONBLOCK);
   if ( errcode < 0 ) {
      vdswSendMessage( &pAcceptor->pWatchdog->log, 
                       WD_ERROR, 
                       "In function fcntl(), error = %d",
                       GetSockError() );
      return -1;
   }
#endif

   for ( i = 1; i < FD_SETSIZE; ++i ) {
      if (  pAcceptor->dispatch[i].socketId == VDS_INVALID_SOCKET ) {
         pAcceptor->dispatch[i].socketId = newSock;
         break;
      }
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void 
HandleAbnormalTermination( vdswAcceptor * pAcceptor, pid_t pid )
{
   vdswSendMessage( &pAcceptor->pWatchdog->log, 
                    WD_ERROR, 
                    "Abnormal termination of process %d",
                    pid );
//                                   " - attempting to recover." );

   vdswHandleAbnormalTermination( pAcceptor->pWatchdog, pid );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool 
IsconnectionAlive( vdswAcceptor * pAcceptor, int indice )
{
   int errcode = 0;
   
   /* 
    * We test to see if the other process is alive by:
    *
    *   on linux/unix, sending a NOOP signal to the process and ...
    *
    *   on windows, by opening the process with as limited access as
    *   possible and ...
    */

#if defined(WIN32)
   return true;
#else
   errcode = kill( pAcceptor->dispatch[indice].socketId, 0 );
   if ( errcode == -1 ) {
//      ESRCH
   }
   return true;
#endif

#if 0   
   if ( errcode <= 0 ) {
      // Abnormal termination of the connection
#if defined (WIN32)
      closesocket( pAcceptor->dispatch[indice].socketId );
#else
      close( pAcceptor->dispatch[indice].socketId );
#endif
      pAcceptor->dispatch[indice].socketId = VDS_INVALID_SOCKET;

      if ( pAcceptor->dispatch[indice].pid > 0 )
         HandleAbnormalTermination( pAcceptor->dispatch[indice].pid );
      pAcceptor->dispatch[indice].pid = -1;
   }
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int 
vdswPrepareConnection( vdswAcceptor * pAcceptor,
                       vdswWatchdog * pWatchdog )
{
   int errcode = 0;
   int one = 1;
   unsigned short port;
   long int dummy = 0;

   pAcceptor->pWatchdog = pWatchdog;
   memset( &pAcceptor->answer, 0, sizeof pAcceptor->answer );
   strcpy( pAcceptor->answer.pathname, pAcceptor->pWatchdog->params.wdLocation );
   pAcceptor->answer.memorySizekb = pAcceptor->pWatchdog->params.memorySizekb;

   dummy = strtol( pAcceptor->pWatchdog->params.wdAddress, NULL, 10 );
   if ( dummy <= 0 || dummy > 65535 ) {
      vdswSendMessage( &pAcceptor->pWatchdog->log, WD_ERROR, "Error getting port number" );
      return -1;
   }
   port = (unsigned short) dummy;
     
//   errcode = sscanf( pAcceptor->pWatchdog->m_params.wdAddress, "%uh", &port );
//   if ( errcode != 1 )
//   {
//      vdswSendMessage( pAcceptor->pWatchdog->log, WD_ERROR, "Error getting port number" );
//      return -1;
//   }

#if defined (WIN32) 
   WORD versionRequested;
   WSADATA wsaData;
 
   versionRequested = MAKEWORD( 2, 2 );
 
   errcode = WSAStartup( versionRequested, &wsaData );
   if ( errcode != 0 ) {
      vdswSendMessage( pAcceptor->pWatchdog->log, 
                       WD_ERROR, 
                       "In function WSAStartup(), error = %d",
                       errcode );
      return -1;
   }
   pAcceptor->cleanupNeeded = true;
   
#endif

   pAcceptor->socketFD = socket( PF_INET, SOCK_STREAM, 0 );
   if ( pAcceptor->socketFD == VDS_INVALID_SOCKET ) {
      vdswSendMessage( &pAcceptor->pWatchdog->log, 
                       WD_ERROR, 
                       "In function socket(), error = %d",
                       GetSockError() );
      return -1;
   }
   
   errcode = setsockopt( pAcceptor->socketFD, SOL_SOCKET, SO_REUSEADDR, 
                         (const char *)&one, sizeof (one) );
   if ( errcode != 0 ) {
      vdswSendMessage( &pAcceptor->pWatchdog->log, 
                       WD_ERROR, 
                       "In function setsockopt(), error = %d",
                       GetSockError() );
      return -1;
   }

   // Set the socket in non-blocking mode.

#if defined (WIN32)
   unsigned long mode = 1;
   errcode = ioctlsocket( pAcceptor->socketFD, FIONBIO, &mode );
   if ( errcode == SOCKET_ERROR ) {
      vdswSendMessage( pAcceptor->pWatchdog->log, 
                       WD_ERROR, 
                       "In function ioctlsocket(), error = %d",
                       GetSockError() );
      return -1;
   }
#else
   errcode = fcntl( pAcceptor->socketFD, F_SETFL, O_NONBLOCK);
   if ( errcode < 0 ) {
      vdswSendMessage( &pAcceptor->pWatchdog->log, 
                       WD_ERROR, 
                       "In function fcntl(), error = %d",
                       GetSockError() );
      return -1;
   }
#endif

   struct sockaddr_in addr;
   memset( &addr, 0, sizeof(struct sockaddr_in) );

   addr.sin_family = PF_INET;
   addr.sin_port = htons( port );
   addr.sin_addr.s_addr = inet_addr( "127.0.0.1" );

   errcode = bind( pAcceptor->socketFD, 
                   (struct sockaddr *) &addr,
                   sizeof(struct sockaddr_in) );
   if ( errcode != 0 ) {
      vdswSendMessage( &pAcceptor->pWatchdog->log, 
                       WD_ERROR, 
                       "In function bind(), error = %d",
                       GetSockError() );
      return -1;
   }

   errcode = listen( pAcceptor->socketFD, 5 );
   if ( errcode != 0 ) {
      vdswSendMessage( &pAcceptor->pWatchdog->log, 
                       WD_ERROR, 
                       "In function listen(), error = %d",
                       GetSockError() );
      return -1;
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void
Receive( vdswAcceptor * pAcceptor, int indice )
{
   int errcode = 0;
   struct WDInput input;
   
   /*
    * A socket can be in a ready state for reading when:
    *  - there is something to read
    *  - the connection is terminated (normally)
    *
    * Because of this it is better to use the MSG_PEEK flag before properly 
    * reading the socket. Even if this means uglier code and calling recv
    * twice...
    *
    * Because of the very small payload of the input request (struct WDInput),
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
      pAcceptor->dispatch[indice].socketId = VDS_INVALID_SOCKET;

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
         pAcceptor->dispatch[indice].socketId = VDS_INVALID_SOCKET;
         if ( pAcceptor->dispatch[indice].pid > 0 ) {
            HandleAbnormalTermination( pAcceptor, pAcceptor->dispatch[indice].pid );
         }
         pAcceptor->dispatch[indice].pid = -1;
      }
      else {
         if ( input.opcode == WD_CONNECT ) {
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
            pAcceptor->dispatch[indice].socketId = VDS_INVALID_SOCKET;
         }
      }
   }
   
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void
Send( vdswAcceptor * pAcceptor, int indice )
{
   int errcode = 0;
   char* ptr = (char*) &pAcceptor->answer;
   int offset = sizeof pAcceptor->answer - pAcceptor->dispatch[indice].dataToBeWritten;
   
   do {
      errcode = send( pAcceptor->dispatch[indice].socketId, 
                      &ptr[offset], 
                      pAcceptor->dispatch[indice].dataToBeWritten, 
                      MSG_NOSIGNAL );
   } while ( errcode == -1 && errno == EINTR );

   if ( errcode == -1 ) {
      vdswSendMessage( &pAcceptor->pWatchdog->log, 
                       WD_ERROR, 
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

      pAcceptor->dispatch[indice].socketId = VDS_INVALID_SOCKET;
      vdswSendMessage( &pAcceptor->pWatchdog->log,
                       WD_WARNING, 
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
vdswWaitForConnections( vdswAcceptor * pAcceptor )
{
   int errcode = 0;
   fd_set readSet, writeSet;
   int i, maxFD, fired;
   struct timeval timeout;   

   /*
    * NOTE: since socket handles, on Windows, are not integers, we will
    * not used the handles as indices to the pAcceptor->dispatch array
    */

   pAcceptor->dispatch[0].socketId = pAcceptor->socketFD;
   pAcceptor->dispatch[0].pid = 0;
   pAcceptor->dispatch[0].dataToBeWritten = false;
   
   for ( i = 1; i < FD_SETSIZE; ++i ) {
      pAcceptor->dispatch[i].socketId = VDS_INVALID_SOCKET;
      pAcceptor->dispatch[i].pid = -1;
      pAcceptor->dispatch[i].dataToBeWritten = false;
   }   

   while ( true ) {
      int zzz = 0;
      if ( g_pWD->controlWord & WD_SHUTDOWN_REQUEST ) {
         break;
      }
      
      timeout.tv_sec = 1;
      timeout.tv_usec = 0;
      FD_ZERO( &readSet  );
      FD_ZERO( &writeSet );
      maxFD = 0;
      for ( i = 0; i < FD_SETSIZE; ++i ) {
         
         if ( pAcceptor->dispatch[i].socketId != VDS_INVALID_SOCKET) {
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
         vdswSendMessage( &pAcceptor->pWatchdog->log, 
                          WD_ERROR, 
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
         errcode = Accept( pAcceptor );
         if ( errcode != 0 ) break;
         fired--;
      }
      if ( fired == 0 ) continue;
      
      /*
       * Process all open sockets 
       */
      for ( i = 1; i < FD_SETSIZE; ++i ) {
         if ( pAcceptor->dispatch[i].socketId != VDS_INVALID_SOCKET ) {
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
      if ( pAcceptor->dispatch[i].socketId != VDS_INVALID_SOCKET ) {
#if defined (WIN32) 
         shutdown( pAcceptor->dispatch[i].socketId, SD_BOTH );      
         closesocket( pAcceptor->dispatch[i].socketId );
#else
         shutdown( pAcceptor->dispatch[i].socketId, 2 );      
         close( pAcceptor->dispatch[i].socketId );
#endif
      }
   }
   pAcceptor->socketFD = VDS_INVALID_SOCKET;

}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

