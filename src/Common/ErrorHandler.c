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

#include "Common/ErrorHandler.h"
#include "Common/ThreadLock.h"

/** Unique identifier for the psocErrorDefinition struct. */
#define PSOC_ERROR_DEFINITION_SIGNATURE ((unsigned)0xfd13a982)

/**
 * Internal (hidden) struct to hold information about the functions required
 * to generate error messages from error codes.
 */
typedef struct psocErrorDefinition
{
   unsigned int                initialized;
   psocErrMsgHandler_T          handler;
   psocErrMsgHandle             handle;
   struct psocErrorDefinition * next;
   char                        name[1];
} psocErrorDefinition;

static psocErrorDefinition *g_definition = NULL;

static struct psocThreadLock g_lock;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Handles the generation of the error message for errno.
 *
 * This function is static (private to the current module).
 *
 * \param[in] errnum The error code.
 * \param[in,out] msg The buffer used to store the message.
 * \param[in]     msgLength The length in bytes of the provided string.
 *
 * \retval 0 on success
 * \retval -1 on error (the error code does not exists for example).
 *
 * \pre \em msg cannot be NULL.
 */
static int psocGetErrnoMsg( int errnum, char * msg, unsigned int msgLength )
{
   int errcode = 0;
   char* s = NULL;

   PSO_PRE_CONDITION( msg != NULL );

   /* To remove gcc warning messages */
   errcode = errcode;
   s = s;

   if ( msgLength > 0 ) {
#if HAVE_STRERROR_R
#  if STRERROR_R_CHAR_P
      /* That's a weird API call. I've added checks which are likely redundant
       * but... I think it's safer this way.
       */
      errno = 0;
      s = strerror_r( errnum, msg, msgLength );
      if ( s != NULL ) strncpy( msg, s, msgLength-1 );
      msg[msgLength-1] = '\0';
      if ( errno != 0 || s == NULL ) return -1;
#  else
      errcode = strerror_r( errnum, msg, msgLength );
      if ( errcode != 0 ) return -1;
#  endif
#else
      errno = 0;
      strncpy( msg, strerror(errnum), msgLength-1 );
      msg[msgLength-1] = '\0';
      if ( errno != 0 ) return -1;
#endif
   }
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined (WIN32)

/**
 * Handles the generation of the error message for WSAGetLastError() on Win32.
 *
 * This function is static (private to the current module).
 *
 * \param[in]     errnum    The error code.
 * \param[in,out] msg       The buffer used to store the message.
 * \param[in]     msgLength The length in bytes of the provided string.
 *
 * \retval 0 on success
 * \retval -1 on error (the error code does not exists for example).
 *
 * \pre \em msg cannot be NULL.
 */
static int psocGetSockErrMsg( int errnum, char * msg, unsigned int msgLength )
{
   PSO_PRE_CONDITION( msg != NULL );

   if ( msgLength > 0 ) {
      /*
       * Error codes and associated messages where copied from:
       * http://tangentsoft.net/wskfaq/examples/basics/ws-util.cpp
       * (public domain)
       */
      switch ( errnum ) {
      case WSABASEERR: /* 0 */
         strncpy( msg, "No error", msgLength - 1 );
         break;
      case WSAEINTR: /* 10004 */
         strncpy( msg, "Interrupted system call", msgLength - 1 );
         break;
      case WSAEBADF: /* 10009 */
         strncpy( msg, "Bad file number", msgLength - 1 );
         break;
      case WSAEACCES: /* 10013 */
         strncpy( msg, "Permission denied", msgLength - 1 );
         break;
      case WSAEFAULT: /* 10014 */
         strncpy( msg, "Bad address", msgLength - 1 );
         break;
      case WSAEINVAL: /* 10022 */
         strncpy( msg, "Invalid argument", msgLength - 1 );
         break;
      case WSAEMFILE: /* 10024 */
         strncpy( msg, "Too many open sockets", msgLength - 1 );
         break;
      case WSAEWOULDBLOCK: /* 10035 */
         strncpy( msg, "Operation would block", msgLength - 1 );
         break;
      case WSAEINPROGRESS: /* 10036 */
         strncpy( msg, "Operation now in progress", msgLength - 1 );
         break;
      case WSAEALREADY: /* 10037 */
         strncpy( msg, "Operation already in progress",
                  msgLength - 1 );
         break;
      case WSAENOTSOCK: /* 10038 */
         strncpy( msg, "Socket operation on non-socket",
                  msgLength - 1 );
         break;
      case WSAEDESTADDRREQ: /* 10039 */
         strncpy( msg, "Destination address required",
                  msgLength - 1 );
         break;
      case WSAEMSGSIZE: /* 10040 */
         strncpy( msg, "Message too long", msgLength - 1 );
         break;
      case WSAEPROTOTYPE: /* 10041 */
         strncpy( msg, "Protocol wrong type for socket",
                  msgLength - 1 );
         break;
      case WSAENOPROTOOPT: /* 10042 */
         strncpy( msg, "Bad protocol option", msgLength - 1 );
         break;
      case WSAEPROTONOSUPPORT: /* 10043 */
         strncpy( msg, "Protocol not supported", msgLength - 1 );
         break;
      case WSAESOCKTNOSUPPORT: /* 10044 */
         strncpy( msg, "Socket type not supported", msgLength - 1 );
         break;
      case WSAEOPNOTSUPP: /* 10045 */
         strncpy( msg, "Operation not supported on socket",
                  msgLength - 1 );
         break;
      case WSAEPFNOSUPPORT: /* 10046 */
         strncpy( msg, "Protocol family not supported",
                  msgLength - 1 );
         break;
      case WSAEAFNOSUPPORT: /* 10047 */
         strncpy( msg, "Address family not supported",
                  msgLength - 1 );
         break;
      case WSAEADDRINUSE: /* 10048 */
         strncpy( msg, "Address already in use", msgLength - 1 );
         break;
      case WSAEADDRNOTAVAIL: /* 10049 */
         strncpy( msg, "Can't assign requested address",
                  msgLength - 1 );
         break;
      case WSAENETDOWN: /* 10050 */
         strncpy( msg, "Network is down", msgLength - 1 );
         break;
      case WSAENETUNREACH: /* 10051 */
         strncpy( msg, "Network is unreachable", msgLength - 1 );
         break;
      case WSAENETRESET: /* 10052 */
         strncpy( msg, "Net connection reset", msgLength - 1 );
         break;
      case WSAECONNABORTED: /* 10053 */
         strncpy( msg, "Software caused connection abort",
                  msgLength - 1 );
         break;
      case WSAECONNRESET: /* 10054 */
         strncpy( msg, "Connection reset by peer", msgLength - 1 );
         break;
      case WSAENOBUFS: /* 10055 */
         strncpy( msg, "No buffer space available", msgLength - 1 );
         break;
      case WSAEISCONN: /* 10056 */
         strncpy( msg, "Socket is already connected", msgLength - 1 );
         break;
      case WSAENOTCONN: /* 10057 */
         strncpy( msg, "Socket is not connected", msgLength - 1 );
         break;
      case WSAESHUTDOWN: /* 10058 */
         strncpy( msg, "Can't send after socket shutdown",
                  msgLength - 1 );
         break;
      case WSAETOOMANYREFS: /* 10059 */
         strncpy( msg, "Too many references, can't splice",
                  msgLength - 1 );
         break;
      case WSAETIMEDOUT: /* 10060 */
         strncpy( msg, "Connection timed out", msgLength - 1 );
         break;
      case WSAECONNREFUSED: /* 10061 */
         strncpy( msg, "Connection refused", msgLength - 1 );
         break;
      case WSAELOOP: /* 10062 */
         strncpy( msg, "Too many levels of symbolic links",
                  msgLength - 1 );
         break;
      case WSAENAMETOOLONG: /* 10063 */
         strncpy( msg, "File name too long", msgLength - 1 );
         break;
      case WSAEHOSTDOWN: /* 10064 */
         strncpy( msg, "Host is down", msgLength - 1 );
         break;
      case WSAEHOSTUNREACH: /* 10065 */
         strncpy( msg, "No route to host", msgLength - 1 );
         break;
      case WSAENOTEMPTY: /* 10066 */
         strncpy( msg, "Directory not empty", msgLength - 1 );
         break;
      case WSAEPROCLIM: /* 10067 */
         strncpy( msg, "Too many processes", msgLength - 1 );
         break;
      case WSAEUSERS: /* 10068 */
         strncpy( msg, "Too many users", msgLength - 1 );
         break;
      case WSAEDQUOT: /* 10069 */
         strncpy( msg, "Disc quota exceeded", msgLength - 1 );
         break;
      case WSAESTALE: /* 10070 */
         strncpy( msg, "Stale NFS file handle", msgLength - 1 );
         break;
      case WSAEREMOTE: /* 10071 */
         strncpy( msg, "Too many levels of remote in path",
                  msgLength - 1 );
         break;
      case WSASYSNOTREADY: /* 10091 */
         strncpy( msg, "Network system is unavailable",
                  msgLength - 1 );
         break;
      case WSAVERNOTSUPPORTED: /* 10092 */
         strncpy( msg, "Winsock version out of range",
                  msgLength - 1 );
         break;
      case WSANOTINITIALISED: /* 10093 */
         strncpy( msg, "WSAStartup not yet called", msgLength - 1 );
         break;
      case WSAEDISCON: /* 10101 */
         strncpy( msg, "Graceful shutdown in progress",
                  msgLength - 1 );
         break;
      case WSAHOST_NOT_FOUND: /* 11001 */
         strncpy( msg, "Host not found", msgLength - 1 );
         break;
      case WSANO_DATA: /* 11004 */
         strncpy( msg, "No host data of that type was found",
                  msgLength - 1 );
         break;

      default:
         strncpy( msg, "Unknown socket error code", msgLength - 1 );
         msg[msgLength-1] = '\0';
         return -1;

      }
   }

   msg[msgLength-1] = '\0';
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Handles the generation of the error message for GetLastError() on Win32.
 *
 * This function is static (private to the current module).
 *
 * \param[in]     errnum    The error code.
 * \param[in,out] msg       The buffer used to store the message.
 * \param[in]     msgLength The length in bytes of the provided string.
 *
 * \retval 0 on success
 * \retval -1 on error (the error code does not exists for example).
 *
 * \pre \em msg cannot be NULL.
 */
static int psocGetWinErrMsg( int errnum, char * msg, unsigned int msgLength )
{
   char* buff = NULL;
   DWORD len;

   PSO_PRE_CONDITION( msg != NULL );

   if ( msgLength > 0 ) {
      len = FormatMessageA(
         FORMAT_MESSAGE_ALLOCATE_BUFFER |
         FORMAT_MESSAGE_FROM_SYSTEM,
         NULL,
         errnum,
         MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
         buff,
         0, NULL );

      if ( len == 0 ) {
         fprintf( stderr, "Abnormal error in FormatMessage, error = %d\n",
                  GetLastError() );
         return -1;
      }
      strncpy( msg, buff, msgLength - 1 );

      LocalFree( buff );
   }

   return 0;
}

#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This function should normally be called at the beginning of a program.
 * If included in a dynamic library, it might be called by the library
 * initialization routine. This routine uses the singleton pattern in case
 * it is called more than once.
 *
 * \retval 0 on success
 * \retval -1 on error (error initializing the lock, for example).
 *
 */

bool psocInitErrorDefs()
{
   int length;
   bool ok = true;
#if defined (WIN32 )
   psocErrorDefinition * pDefinition = NULL;
   psocErrorDefinition * previous = NULL;
#endif

   if ( g_definition == NULL ) {
      ok = psocInitThreadLock( &g_lock );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ok ) {
         psocAcquireThreadLock( &g_lock );
         if ( g_definition == NULL ) {
            length = offsetof(psocErrorDefinition,name) + strlen("errno") + 1;

            g_definition = malloc( length );
            if ( g_definition == NULL ) {
               fprintf( stderr, "Abnormal error in malloc\n" );
               ok = false;
            }
            else {
               g_definition->initialized = PSOC_ERROR_DEFINITION_SIGNATURE;
               g_definition->handle      = PSOC_ERRNO_HANDLE;
               g_definition->handler     = &psocGetErrnoMsg;
               g_definition->next        = NULL;
               strcpy( g_definition->name, "errno" );
#if defined (WIN32 )
               length = offsetof(psocErrorDefinition,name) +
                  strlen("Windows error") + 1;

               pDefinition = malloc( length );
               if ( pDefinition == NULL ) {
                  fprintf( stderr, "Abnormal error in malloc\n" );
                  ok = false;
               }
               else {
                  pDefinition->initialized = PSOC_ERROR_DEFINITION_SIGNATURE;
                  pDefinition->handle      = PSOC_WINERR_HANDLE;
                  pDefinition->handler     = &psocGetWinErrMsg;
                  pDefinition->next        = NULL;
                  strcpy( pDefinition->name, "Windows error" );

                  g_definition->next = pDefinition;
                  previous = pDefinition;

                  length = offsetof(psocErrorDefinition,name) +
                     strlen("Windows socket error") + 1;

                  pDefinition = malloc( length );
                  if ( pDefinition == NULL ) {
                     fprintf( stderr, "Abnormal error in malloc\n" );
                     ok = false;
                  }
                  else {
                     pDefinition->initialized = PSOC_ERROR_DEFINITION_SIGNATURE;
                     pDefinition->handle      = PSOC_SOCKERR_HANDLE;
                     pDefinition->handler     = &psocGetSockErrMsg;
                     pDefinition->next        = NULL;
                     strcpy( pDefinition->name, "Windows socket error" );

                     previous->next = pDefinition;
                  }
               } /* The second check on malloc failure */
#endif
            } /* The first check on malloc failure */
         } /* The second check on g_definition == NULL */
         psocReleaseThreadLock( &g_lock );
      } /* if initlock == true ) */
   } /* The first check on g_definition == NULL */

   return ok;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This function should normally be called at the end of a program.
 * If included in a dynamic library, it might be called by the library
 * deallocation routine. This routine uses the singleton pattern in case
 * it is called more than once.
 *
 *
 */
void psocFiniErrorDefs()
{
   psocErrorDefinition * pDefinition = NULL;
   psocErrorDefinition * pNext = NULL;

   if ( g_definition != NULL ) {
      psocAcquireThreadLock( &g_lock );
      if ( g_definition != NULL ) {
         pDefinition = g_definition;
         do {
            pNext = pDefinition->next; /* Save the address of the next one */
            free( pDefinition );
            pDefinition = pNext;
         }
         while ( pDefinition != NULL );

      }
      psocReleaseThreadLock( &g_lock );
   }
   g_definition = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \param[in]   name    A name for the message type. It is use to identify
 *                      the provenance of the error in error messages.
 * \param[in]   handler A pointer to the function used to transform the
 *                      error code into an error message.
 *
 * \return The function returns an opaque handle on success or
 *         ::PSOC_NO_ERRHANDLER on error.

 * \pre \em name cannot be NULL.
 * \pre \em handler cannot be NULL
 *
 * \invariant g_definition cannot be NULL.
 */

psocErrMsgHandle psocAddErrorMsgHandler( const char          * name,
                                         psocErrMsgHandler_T   handler )
{
   int length;
   psocErrMsgHandle handle = PSOC_NO_ERRHANDLER, i;

   psocErrorDefinition * pDefinition   = NULL;
   psocErrorDefinition * nextAvailable = NULL;

   PSO_INV_CONDITION( g_definition != NULL );
   PSO_PRE_CONDITION( name    != NULL );
   PSO_PRE_CONDITION( handler != NULL );

   psocAcquireThreadLock( &g_lock );

   length = offsetof(psocErrorDefinition,name) + strlen( name ) + 1;

   pDefinition = malloc( length );
   if ( pDefinition == NULL ) return handle;

   pDefinition->initialized = PSOC_ERROR_DEFINITION_SIGNATURE;
   pDefinition->next = NULL;
   strcpy( pDefinition->name, name );
   pDefinition->handler = handler;

   i = 1;
   nextAvailable = g_definition;
   while ( nextAvailable->next != NULL ) {
      if ( nextAvailable->initialized != PSOC_ERROR_DEFINITION_SIGNATURE ) {
         psocReleaseThreadLock( &g_lock );
         return handle;
      }

      nextAvailable = nextAvailable->next;
      i++;
   }

   nextAvailable->next = pDefinition;
   handle = i;

   psocReleaseThreadLock( &g_lock );

   return handle;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \param[in]  pErrorHandler A pointer to the psocErrorHandler struct itself.
 *
 * \pre \em pErrorHandler cannot be NULL.
 *
 * \invariant g_definition cannot be NULL.
 */
void psocInitErrorHandler( psocErrorHandler * pErrorHandler )
{
   int i;

   PSO_INV_CONDITION( g_definition  != NULL );
   PSO_PRE_CONDITION( pErrorHandler != NULL );

   for ( i = 0; i < PSOC_ERROR_CHAIN_LENGTH; ++i ) {
      pErrorHandler->errorCode[i]   = 0;
      pErrorHandler->errorHandle[i] = PSOC_NO_ERRHANDLER;
   }
   pErrorHandler->chainLength = 0;
   pErrorHandler->initialized = PSOC_ERROR_HANDLER_SIGNATURE;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \param[in]  pErrorHandler A pointer to the psocErrorHandler struct itself.
 *
 * \pre \em pErrorHandler cannot be NULL.
 *
 * \invariant \em pErrorHandler->initialized must equal
 *                ::PSOC_ERROR_HANDLER_SIGNATURE.
 */
void psocFiniErrorHandler( psocErrorHandler * pErrorHandler )
{
   int i;

   PSO_PRE_CONDITION( pErrorHandler != NULL );
   PSO_INV_CONDITION(
      pErrorHandler->initialized == PSOC_ERROR_HANDLER_SIGNATURE );

   for ( i = 0; i < PSOC_ERROR_CHAIN_LENGTH; ++i ) {
      pErrorHandler->errorCode[i]   = 0;
      pErrorHandler->errorHandle[i] = PSOC_NO_ERRHANDLER;
   }
   pErrorHandler->chainLength = 0;
   pErrorHandler->initialized = 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \param[in]     pErrorHandler A pointer to the psocErrorHandler struct
 *                              itself.
 * \param[in,out] msg           The buffer that will hold the message.
 * \param[in]     maxLength     The length (in bytes) of \em msg.
 *
 * \return The length of the message (or zero on error, for example when
 *         acquiring the lock fails).
 *
 * \pre \em pErrorHandler cannot be NULL.
 * \pre \em msg cannot be NULL.
 * \pre \em maxLength must be greater than zero.
 *
 * \invariant \em g_definition cannot be NULL.
 * \invariant \em pErrorHandler->initialized must equal
 *                ::PSOC_ERROR_HANDLER_SIGNATURE.
 */
size_t
psocGetErrorMsg( psocErrorHandler * pErrorHandler,
                 char             * msg,
                 size_t             maxLength )
{
   size_t len, length = maxLength, msgStart = 0;
   psocErrMsgHandle i;
   int k;
   psocErrorDefinition * nextAvailable = NULL;

   PSO_INV_CONDITION( g_definition != NULL );
   PSO_PRE_CONDITION( pErrorHandler != NULL );
   PSO_INV_CONDITION(
      pErrorHandler->initialized == PSOC_ERROR_HANDLER_SIGNATURE );
   PSO_PRE_CONDITION( msg != NULL );
   PSO_PRE_CONDITION( maxLength > 0 );

   if ( ! psocTryAcquireThreadLock( &g_lock, 100 ) ) return 0;

   for ( k = 0; k < pErrorHandler->chainLength; ++k ) {
      nextAvailable = g_definition;
      for ( i = 0; i < pErrorHandler->errorHandle[k]; i++ ) {
         nextAvailable = nextAvailable->next;
         if ( nextAvailable == NULL ) {
            psocReleaseThreadLock( &g_lock );
            return 0;
         }
      }

      if ( nextAvailable->initialized != PSOC_ERROR_DEFINITION_SIGNATURE ) {
         psocReleaseThreadLock( &g_lock );
         return 0;
      }

      nextAvailable->handler( pErrorHandler->errorCode[k],
                              &msg[msgStart],
                              length );
      /* Just in case */
      len = strnlen( &msg[msgStart], length );
      if ( len > length ) {
         length = 0;
      }
      else {
         length -= len;
      }
      msgStart += len;
   }

   psocReleaseThreadLock( &g_lock );

   msg[maxLength-1] = '\0';

   return strnlen( msg, maxLength );
}

/**
 * \param[in]     pErrorHandler A pointer to the psocErrorHandler struct
 *                              itself.
 *
 * \return The length of the message (or zero on error, for example when
 *         acquiring the lock fails).
 *
 * \pre \em pErrorHandler cannot be NULL.
 *
 * \invariant \em g_definition cannot be NULL.
 * \invariant \em pErrorHandler->initialized must equal
 *                ::PSOC_ERROR_HANDLER_SIGNATURE.
 */
size_t
psocGetErrorMsgLength( psocErrorHandler * pErrorHandler )
{
   size_t len, sum = 0;
   psocErrMsgHandle i;
   int k;
   psocErrorDefinition * nextAvailable = NULL;
   char tmpMsg[4096];

   PSO_INV_CONDITION( g_definition != NULL );
   PSO_PRE_CONDITION( pErrorHandler != NULL );
   PSO_INV_CONDITION(
      pErrorHandler->initialized == PSOC_ERROR_HANDLER_SIGNATURE );

   if ( ! psocTryAcquireThreadLock( &g_lock, 100 ) ) return 0;

   for ( k = 0; k < pErrorHandler->chainLength; ++k ) {
      nextAvailable = g_definition;
      for ( i = 0; i < pErrorHandler->errorHandle[k]; i++ ) {
         nextAvailable = nextAvailable->next;
         if ( nextAvailable == NULL ) {
            psocReleaseThreadLock( &g_lock );
            return 0;
         }
      }

      if ( nextAvailable->initialized != PSOC_ERROR_DEFINITION_SIGNATURE ) {
         psocReleaseThreadLock( &g_lock );
         return 0;
      }

      nextAvailable->handler( pErrorHandler->errorCode[k],
                              tmpMsg,
                              4096 );
      /* Just in case */
      len = strnlen( tmpMsg, 4096 );
      PSO_POST_CONDITION( len < 4096 );
      sum += len;

   }

   psocReleaseThreadLock( &g_lock );

   return sum;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \param[in]     pErrorHandler A pointer to the psocErrorHandler struct
 *                              itself.
 * \param[in,out] handle        Handle to the type of error codes/messages.
 * \param[in]     errorCode     The error code itself.
 *
 * \pre \em pErrorHandler cannot be NULL.
 * \pre \em handle must be valid (either one of the predefined ones or one
 *          returned by ::psocAddErrorMsgHandler
 *
 * \invariant \em g_definition cannot be NULL.
 * \invariant \em pErrorHandler->initialized must equal
 *                ::PSOC_ERROR_HANDLER_SIGNATURE.
 */
void psocSetError( psocErrorHandler * pErrorHandler,
                   psocErrMsgHandle   handle,
                   int                errorCode )
{
#if defined(USE_DBC)
   int i;
   psocErrorDefinition * nextAvailable = NULL;
#endif

   PSO_INV_CONDITION( g_definition != NULL );

   PSO_PRE_CONDITION( pErrorHandler != NULL );
   PSO_INV_CONDITION(
      pErrorHandler->initialized == PSOC_ERROR_HANDLER_SIGNATURE );

   /*
    * Was the error handler initialized?
    * Note: if we fail to get the lock we go on anyway since the lock
    * is only used for testing the pre-condition.
    */
#if defined(USE_DBC)
   if ( psocTryAcquireThreadLock(&g_lock, 1000) ) {
      i = 0;
      nextAvailable = g_definition;
      while ( nextAvailable->next != NULL && i <= handle ) {
         nextAvailable = nextAvailable->next;
         ++i;
      }

      psocReleaseThreadLock( &g_lock );
      PSO_PRE_CONDITION(  i >= handle );
   }
#endif

   pErrorHandler->errorHandle[0] = handle;
   pErrorHandler->errorCode[0]   = errorCode;
   pErrorHandler->chainLength = 1;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \param[in]     pErrorHandler A pointer to the psocErrorHandler struct
 *                              itself.
 * \param[in,out] handle        Handle to the type of error codes/messages.
 * \param[in]     errorCode     The error code itself.
 *
 * \pre \em pErrorHandler cannot be NULL.
 * \pre \em handle must be valid (either one of the predefined ones or one
 *          returned by ::psocAddErrorMsgHandler.
 *
 * \invariant \em g_definition cannot be NULL.
 * \invariant \em pErrorHandler->initialized must equal
 *                ::PSOC_ERROR_HANDLER_SIGNATURE.\
 * \invariant The length of the chain of errors must ge greater than zero
 *            and less than ::PSOC_ERROR_CHAIN_LENGTH.
 */
void psocChainError( psocErrorHandler * pErrorHandler,
                     psocErrMsgHandle   handle,
                     int                errorCode )
{
#if defined(USE_DBC)
   int i;
   psocErrorDefinition * nextAvailable = NULL;
#endif

   PSO_INV_CONDITION( g_definition != NULL );

   PSO_PRE_CONDITION( pErrorHandler != NULL );
   PSO_INV_CONDITION(
      pErrorHandler->initialized == PSOC_ERROR_HANDLER_SIGNATURE );
   PSO_INV_CONDITION( pErrorHandler->chainLength > 0 &&
                      pErrorHandler->chainLength < PSOC_ERROR_CHAIN_LENGTH );

   /*
    * Was the error handler initialized?
    * Note: if we fail to get the lock we go on anyway since the lock
    * is only used for testing the pre-condition.
    */
#if defined(USE_DBC)
   if ( psocTryAcquireThreadLock(&g_lock, 1000) ) {
      i = 0;
      nextAvailable = g_definition;
      while ( nextAvailable->next != NULL && i <= handle ) {
         nextAvailable = nextAvailable->next;
         ++i;
      }

      psocReleaseThreadLock( &g_lock );
      PSO_PRE_CONDITION( i >= handle );
   }
#endif

   pErrorHandler->errorHandle[pErrorHandler->chainLength] = handle;
   pErrorHandler->errorCode  [pErrorHandler->chainLength] = errorCode;
   pErrorHandler->chainLength++;
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

