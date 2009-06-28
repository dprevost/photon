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

#ifndef PSOC_ERROR_HANDLER_H
#define PSOC_ERROR_HANDLER_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Common/Common.h"

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \defgroup psocErrorHandler The Error Handler module
 *
 * This module provides a unified mechanism to store error codes and
 * to retrieve error messages associated with the error codes.
 *
 * Additionally, the error codes for a specific event can be chained.
 *
 * For example, the errno ENOENT ("No such file or directory") is by itself
 * a bit vague if you're working with different types of files. In the same
 * fashion, the error "ERROR_WITH_CONFIG_FILE" is also vague.
 * In many cases programmers have often reinvented the wheel, like
 * "CONFIG_FILE_DOES_NOT_EXIST", etc. which duplicates the information
 * provided by errno (or GetLastError() on Windows).
 *
 * IMHO, a better solution is to provide a limited "stack" of error messages
 * (the size of the chain of errors is limited by PSOC_ERROR_CHAIN_LENGTH).
 *
 * Currently the code will generate appropriate error messages for errno
 * and for GetLastError() and WSAGetLastError() on Windows.
 *
 * On top of this, "private" error messages can be added by adding a message
 * handler (a pointer to a function that transform an error code into an
 * error message) to the list of message handlers. In fact
 * you don't even to write these handlers, you can let the utility
 * errorParser built it for you (transforming the comments in a header file
 * describing the error codes in error messages).
 *
 * \ingroup psocCommon
 * @{
 */

/**
 * \file
 *
 * This file provides:
 *  - a unified mechanism to store error codes (which can be chained).
 *  - a method to retrieve error messages associated with the error codes.
 *  - a method to add your own error codes and associated messages.
 *
 */

/** Unique identifier for the psocErrorHandler struct. */
#define PSOC_ERROR_HANDLER_SIGNATURE ((unsigned)0xfd13a982)

/** A handle to the function that converts an error code into an error
 *  message.
 */
typedef int psocErrMsgHandle;

#define PSOC_NO_ERRHANDLER    ((psocErrMsgHandle)-1)
#define PSOC_ERRNO_HANDLE     ((psocErrMsgHandle) 0)
#if defined (WIN32 )
#  define PSOC_WINERR_HANDLE  ((psocErrMsgHandle) 1)
#  define PSOC_SOCKERR_HANDLE ((psocErrMsgHandle) 2)
#endif

#define PSOC_ERROR_CHAIN_LENGTH  5

typedef int (*psocErrMsgHandler_T)(int, char *, unsigned int);

typedef struct psocErrorHandler
{
   /** Set to PSOC_ERROR_HANDLER_SIGNATURE at initialization. */
   unsigned int initialized;

   /** The length of the chain of errors. */
   int chainLength;

   /** Error codes. */
   int errorCode[PSOC_ERROR_CHAIN_LENGTH];

   /** Handle to the function used to retrieve the error message. */
   psocErrMsgHandle errorHandle[PSOC_ERROR_CHAIN_LENGTH];

} psocErrorHandler;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*! \brief Use for initializing our internal data. */
PHOTON_COMMON_EXPORT
bool psocInitErrorDefs();

/*! \brief Clear our internal data. */
PHOTON_COMMON_EXPORT
void psocFiniErrorDefs();

/*! \brief Add a function to handle error messages. */
PHOTON_COMMON_EXPORT
psocErrMsgHandle psocAddErrorMsgHandler( const char          * name,
                                         psocErrMsgHandler_T   handler );

/*! \brief Use for initializing the struct or to reset it, as needed. */
PHOTON_COMMON_EXPORT
void psocInitErrorHandler( psocErrorHandler * pErrorHandler );

/*! \brief Terminate access to the struct psocErrorHandler */
PHOTON_COMMON_EXPORT
void psocFiniErrorHandler( psocErrorHandler * pErrorHandler );

/*! Test to see if errors were found.
 *
 * \param[in] pErrorHandler A pointer to the psocErrorHandler struct itself.
 *
 * \retval true  (1) if there are errors.
 * \retval false (0) if no errors.
 *
 * \pre \em pErrorHandler cannot be NULL.
 */
static inline
bool psocAnyErrors( psocErrorHandler * pErrorHandler )
{
   PSO_PRE_CONDITION( pErrorHandler != NULL );

   return ( pErrorHandler->chainLength > 0 );
}

/*! \brief Retrieves the error message or a concatenation of all error
 *         messages (if more than one)
 */
PHOTON_COMMON_EXPORT
size_t psocGetErrorMsg( psocErrorHandler * pErrorHandler,
                        char             * msg,
                        size_t             maxLength );

/*! \brief Retrieves the length of the error message or the length of a
 *         concatenation of all error messages (if more than one) - the
 *         space for a NULL terminator is not included.
 */
PHOTON_COMMON_EXPORT
size_t psocGetErrorMsgLength( psocErrorHandler * pErrorHandler );

/*! \brief
 * Sets both the error code and the handler for the
 * error message. It will first reset the chain of error codes to zero.
 */
PHOTON_COMMON_EXPORT
void psocSetError( psocErrorHandler * pErrorHandler,
                   psocErrMsgHandle   handle,
                   int                errorCode );

/*! \brief
 * Adds the error code and the handler for the error message to an
 * existing chain.
 */
PHOTON_COMMON_EXPORT
void psocChainError( psocErrorHandler * pErrorHandler,
                     psocErrMsgHandle   handle,
                     int                errorCode );

/*! \brief
 * Retrieves the last error number or zero if no errors.
 */
static inline
int psocGetLastError( psocErrorHandler * pErrorHandler )
{
   PSO_PRE_CONDITION( pErrorHandler != NULL );
   PSO_INV_CONDITION(
      pErrorHandler->initialized == PSOC_ERROR_HANDLER_SIGNATURE );

   if ( pErrorHandler->chainLength > 0 )
      return pErrorHandler->errorCode[pErrorHandler->chainLength-1];
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * @}
 */

END_C_DECLS

#endif /* PSOC_ERROR_HANDLER_H */

