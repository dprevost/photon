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

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#include "Acceptor.h"
#include "Watchdog.h"

#if ! defined(WIN32)
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#else
#  define MSG_NOSIGNAL 0
#endif

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

vdswAcceptor::vdswAcceptor()
   : m_socketFD ( VDS_INVALID_SOCKET )
#if defined (WIN32)
     , m_cleanupNeeded ( false )
#endif
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

vdswAcceptor::~vdswAcceptor()
{
   if ( m_socketFD != VDS_INVALID_SOCKET ) {
#if defined (WIN32) 
      closesocket( m_socketFD );
#else
      close( m_socketFD );
#endif
      m_socketFD = VDS_INVALID_SOCKET;
   }
   
#if defined (WIN32) 
   if ( m_cleanupNeeded ) WSACleanup ();
#endif
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int
vdswAcceptor::Accept()
{
   VDS_SOCKET newSock = VDS_INVALID_SOCKET;
   int errcode, i;
   
#if ! defined (WIN32) 
   do {
#endif
      newSock = accept( m_socketFD, NULL, 0 );
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
      m_pWatchdog->m_log.SendMessage( WD_ERROR, 
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
      m_pWatchdog->m_log.SendMessage( WD_ERROR, 
                                    "In function ioctlsocket(), error = %d",
                                    GetSockError() );
      return -1;
   }
#else
   errcode = fcntl( newSock, F_SETFL, O_NONBLOCK);
   if ( errcode < 0 ) {
      m_pWatchdog->m_log.SendMessage( WD_ERROR, 
                                    "In function fcntl(), error = %d",
                                    GetSockError() );
      return -1;
   }
#endif

   for ( i = 1; i < FD_SETSIZE; ++i ) {
      if (  m_dispatch[i].socketId == VDS_INVALID_SOCKET ) {
         m_dispatch[i].socketId = newSock;
         break;
      }
   }
   
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void 
vdswAcceptor::HandleAbnormalTermination( pid_t pid )
{
   m_pWatchdog->m_log.SendMessage( WD_ERROR, 
                                   "Abnormal termination of process %d",
                                   pid );
//                                   " - attempting to recover." );

   m_pWatchdog->HandleAbnormalTermination( pid );
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

bool 
vdswAcceptor::IsconnectionAlive(  int indice )
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
   errcode = kill( m_dispatch[indice].socketId, 0 );
   if ( errcode == -1 ) {
//      ESRCH
   }
   return true;
#endif

#if 0   
   if ( errcode <= 0 ) {
      // Abnormal termination of the connection
#if defined (WIN32)
      closesocket( m_dispatch[indice].socketId );
#else
      close( m_dispatch[indice].socketId );
#endif
      m_dispatch[indice].socketId = VDS_INVALID_SOCKET;

      if ( m_dispatch[indice].pid > 0 )
         HandleAbnormalTermination( m_dispatch[indice].pid );
      m_dispatch[indice].pid = -1;
   }
#endif
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int 
vdswAcceptor::PrepareConnection( vdswWatchdog* pWatchdog )
{
   int errcode = 0;
   int one = 1;
   unsigned short port;
   long int dummy = 0;

   m_pWatchdog = pWatchdog;
   memset( &m_answer, 0, sizeof m_answer );
   strcpy( m_answer.pathname, m_pWatchdog->m_params.wdLocation );
   m_answer.memorySizekb = m_pWatchdog->m_params.memorySizekb;

   dummy = strtol( m_pWatchdog->m_params.wdAddress, NULL, 10 );
   if ( dummy <= 0 || dummy > 65535 ) {
      m_pWatchdog->m_log.SendMessage( WD_ERROR, "Error getting port number" );
      return -1;
   }
   port = (unsigned short) dummy;
     
//   errcode = sscanf( m_pWatchdog->m_params.wdAddress, "%uh", &port );
//   if ( errcode != 1 )
//   {
//      m_pWatchdog->m_log.SendMessage( WD_ERROR, "Error getting port number" );
//      return -1;
//   }

#if defined (WIN32) 
   WORD versionRequested;
   WSADATA wsaData;
 
   versionRequested = MAKEWORD( 2, 2 );
 
   errcode = WSAStartup( versionRequested, &wsaData );
   if ( errcode != 0 ) {
      m_pWatchdog->m_log.SendMessage( WD_ERROR, 
                                    "In function WSAStartup(), error = %d",
                                    errcode );
      return -1;
   }
   m_cleanupNeeded = true;
   
#endif

   m_socketFD = socket( PF_INET, SOCK_STREAM, 0 );
   if ( m_socketFD == VDS_INVALID_SOCKET ) {
      m_pWatchdog->m_log.SendMessage( WD_ERROR, 
                                    "In function socket(), error = %d",
                                    GetSockError() );
      return -1;
   }
   
   errcode = setsockopt( m_socketFD, SOL_SOCKET, SO_REUSEADDR, 
                         (const char *)&one, sizeof (one) );
   if ( errcode != 0 ) {
      m_pWatchdog->m_log.SendMessage( WD_ERROR, 
                                    "In function setsockopt(), error = %d",
                                    GetSockError() );
      return -1;
   }

   // Set the socket in non-blocking mode.

#if defined (WIN32)
   unsigned long mode = 1;
   errcode = ioctlsocket( m_socketFD, FIONBIO, &mode );
   if ( errcode == SOCKET_ERROR ) {
      m_pWatchdog->m_log.SendMessage( WD_ERROR, 
                                    "In function ioctlsocket(), error = %d",
                                    GetSockError() );
      return -1;
   }
#else
   errcode = fcntl( m_socketFD, F_SETFL, O_NONBLOCK);
   if ( errcode < 0 ) {
      m_pWatchdog->m_log.SendMessage( WD_ERROR, 
                                    "In function fcntl(), error = %d",
                                    GetSockError() );
      return -1;
   }
#endif

   struct sockaddr_in addr;
   memset( &addr, 0, sizeof addr );

   addr.sin_family = PF_INET;
   addr.sin_port = htons( port );
   addr.sin_addr.s_addr = inet_addr( "127.0.0.1" );

   errcode = bind( m_socketFD, 
                   (sockaddr *) &addr,
                   sizeof(sockaddr_in) );
   if ( errcode != 0 ) {
      m_pWatchdog->m_log.SendMessage( WD_ERROR, 
                                    "In function bind(), error = %d",
                                    GetSockError() );
      return -1;
   }

   errcode = listen( m_socketFD, 5 );
   if ( errcode != 0 ) {
      m_pWatchdog->m_log.SendMessage( WD_ERROR, 
                                    "In function listen(), error = %d",
                                    GetSockError() );
      return -1;
   }
   
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void
vdswAcceptor::Receive( int indice )
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
      errcode = recv( m_dispatch[indice].socketId,
                      (char *)&input,
                      sizeof input,
                      MSG_PEEK | MSG_NOSIGNAL );
   } while ( errcode == -1 && errno == EINTR );

   if ( errcode <= 0 ) {
      // Abnormal termination of the connection
#if defined (WIN32)
      closesocket( m_dispatch[indice].socketId );
#else
      close( m_dispatch[indice].socketId );
#endif
      m_dispatch[indice].socketId = VDS_INVALID_SOCKET;

      if ( m_dispatch[indice].pid > 0 ) {
         HandleAbnormalTermination( m_dispatch[indice].pid );
      }
      m_dispatch[indice].pid = -1;
   }
   else {
      // Read the input data
      do {
         errcode = recv( m_dispatch[indice].socketId,
                         (char *)&input,
                         sizeof input,
                         MSG_NOSIGNAL );
      } while ( errcode == -1 && errno == EINTR );

      if ( errcode != sizeof input ) {
         // Abnormal termination of the connection
#if defined (WIN32)
         closesocket( m_dispatch[indice].socketId );
#else
         close( m_dispatch[indice].socketId );
#endif
         m_dispatch[indice].socketId = VDS_INVALID_SOCKET;
         if ( m_dispatch[indice].pid > 0 ) {
            HandleAbnormalTermination( m_dispatch[indice].pid );
         }
         m_dispatch[indice].pid = -1;
      }
      else {
         if ( input.opcode == WD_CONNECT ) {
            m_dispatch[indice].pid = input.processId;
            m_dispatch[indice].dataToBeWritten = sizeof m_answer;
         }
         else {
            // normal termination      
#if defined (WIN32)
            shutdown( m_dispatch[indice].socketId, SD_BOTH );      
            closesocket( m_dispatch[indice].socketId );
#else
            shutdown( m_dispatch[indice].socketId, 2 );      
            close( m_dispatch[indice].socketId );
#endif
            m_dispatch[indice].socketId = VDS_INVALID_SOCKET;
         }
      }
   }
   
   return;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void
vdswAcceptor::Send( int indice )
{
   int errcode = 0;
   char* ptr = (char*) &m_answer;
   int offset = sizeof m_answer - m_dispatch[indice].dataToBeWritten;
   
   do {
      errcode = send( m_dispatch[indice].socketId, 
                      &ptr[offset], 
                      m_dispatch[indice].dataToBeWritten, 
                      MSG_NOSIGNAL );
   } while ( errcode == -1 && errno == EINTR );

   if ( errcode == -1 ) {
      m_pWatchdog->m_log.SendMessage( WD_ERROR, 
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
      shutdown( m_dispatch[indice].socketId, SD_BOTH );      
      closesocket( m_dispatch[indice].socketId );
#else
      if ( errno != EPIPE ) {
         shutdown( m_dispatch[indice].socketId, 2 );      
         close( m_dispatch[indice].socketId );
      }
#endif

      m_dispatch[indice].socketId = VDS_INVALID_SOCKET;
      m_pWatchdog->m_log.SendMessage( WD_WARNING, 
                                      "Connection terminated abnormally %s%d",
                                      "for process ", 
                                      m_dispatch[indice].pid );      
      return;
   }
   
   m_dispatch[indice].dataToBeWritten -= errcode;

   return;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void 
vdswAcceptor::WaitForConnections()
{
   int errcode = 0;
   fd_set readSet, writeSet;
   int i, maxFD, fired;
   struct timeval timeout;   

   /*
    * NOTE: since socket handles, on Windows, are not integers, we will
    * not used the handles as indices to the m_dispatch array
    */

   m_dispatch[0].socketId = m_socketFD;
   m_dispatch[0].pid = 0;
   m_dispatch[0].dataToBeWritten = false;
   
   for ( i = 1; i < FD_SETSIZE; ++i ) {
      m_dispatch[i].socketId = VDS_INVALID_SOCKET;
      m_dispatch[i].pid = -1;
      m_dispatch[i].dataToBeWritten = false;
   }   

   while ( true ) {
      int zzz = 0;
      if ( vdswWatchdog::g_pWD->m_controlWord & WD_SHUTDOWN_REQUEST ) {
         break;
      }
      
      timeout.tv_sec = 1;
      timeout.tv_usec = 0;
      FD_ZERO( &readSet  );
      FD_ZERO( &writeSet );
      maxFD = 0;
      for ( i = 0; i < FD_SETSIZE; ++i ) {
         
         if ( m_dispatch[i].socketId != VDS_INVALID_SOCKET) {
            if ( m_dispatch[i].dataToBeWritten == 0 ) {
               FD_SET( m_dispatch[i].socketId, &readSet);
               zzz++;
            }
            else {
               FD_SET( m_dispatch[i].socketId, &writeSet);
               zzz++;
            }
#if ! defined (WIN32)
            if ( m_dispatch[i].socketId+1 > maxFD ) {
               maxFD = m_dispatch[i].socketId+1;
            }
#endif
         }
      }

      do {
         fired = select( maxFD, &readSet, &writeSet, NULL, &timeout );
      } while ( fired == -1 && errno == EINTR );
      

      if ( fired == -1 ) {
         m_pWatchdog->m_log.SendMessage( WD_ERROR, 
                                       "In function select(), error = %d",
                                       GetSockError() );
         errcode = -1;
         break;
      }
      if ( fired == 0 ) continue;
      
      /*
       * Start with the socket listening for new connection requests
       */
      if ( FD_ISSET( m_socketFD, &readSet ) ) {
         errcode = Accept();
         if ( errcode != 0 ) break;
         fired--;
      }
      if ( fired == 0 ) continue;
      
      /*
       * Process all open sockets 
       */
      for ( i = 1; i < FD_SETSIZE; ++i ) {
         if ( m_dispatch[i].socketId != VDS_INVALID_SOCKET ) {
            if ( FD_ISSET( m_dispatch[i].socketId, &writeSet ) ) {
               Send( i );
               fired--;
            }
            else if ( FD_ISSET( m_dispatch[i].socketId, &readSet ) ) {
               Receive( i );
               fired--;
            }
         }
         if ( fired == 0 ) break;
      }
   }

   // Cleanup (close all sockets)
   for ( i = 0; i < FD_SETSIZE; ++i ) {
      if ( m_dispatch[i].socketId != VDS_INVALID_SOCKET ) {
#if defined (WIN32) 
         shutdown( m_dispatch[i].socketId, SD_BOTH );      
         closesocket( m_dispatch[i].socketId );
#else
         shutdown( m_dispatch[i].socketId, 2 );      
         close( m_dispatch[i].socketId );
#endif
      }
   }
   m_socketFD = VDS_INVALID_SOCKET;

}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

