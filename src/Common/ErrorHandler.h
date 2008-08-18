/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of the vdsf (Virtual Data Space Framework) Library.
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation and appearing in the file COPYING included in the
 * packaging of this library.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifndef PSC_ERROR_HANDLER_H
#define PSC_ERROR_HANDLER_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Common/Common.h"

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \defgroup pscErrorHandler The Error Handler module
 *
 * This module provides a unified mechanism to store error codes and
 * to retrieve error messages associated with the error codes.
 *
 * Additionaly, the error codes for a specific event can be chained.
 *
 * For example, the errno ENOENT ("No such file or directory") is by itself
 * a bit vague if you're working with different types of files. In the same
 * fashion, the error "ERROR_WITH_CONFIG_FILE" is also vague.
 * In many cases programmers have often reinvented the wheel, like
 * "CONFIG_FILE_DOES_NOT_EXIST", etc. which duplicates the information
 * provided by errno (or GetLastError() on Windows).
 *
 * IMHO, a better solution is to provide a limited "stack" of error messages
 * (the size of the chain of errors is limited by PSC_ERROR_CHAIN_LENGTH).
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
 * \ingroup pscCommon
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

/** Unique identifier for the pscErrorHandler struct. */
#define PSC_ERROR_HANDLER_SIGNATURE ((unsigned)0xfd13a982)

/** A handle to the function that converts an error code into an error 
 *  message. 
 */
typedef int pscErrMsgHandle;

#define PSC_NO_ERRHANDLER    ((pscErrMsgHandle)-1)
#define PSC_ERRNO_HANDLE     ((pscErrMsgHandle) 0)
#if defined (WIN32 )
#  define PSC_WINERR_HANDLE  ((pscErrMsgHandle) 1)
#  define PSC_SOCKERR_HANDLE ((pscErrMsgHandle) 2)
#endif

#define PSC_ERROR_CHAIN_LENGTH  5

typedef int (*pscErrMsgHandler_T)(int, char *, unsigned int);

typedef struct pscErrorHandler
{
   /** Set to PSC_ERROR_HANDLER_SIGNATURE at initialization. */
   unsigned int initialized;
   
   /** The length of the chain of errors. */
   int chainLength;
   
   /** Error codes. */
   int errorCode[PSC_ERROR_CHAIN_LENGTH];
   
   /** Handle to the function used to retrieve the error message. */
   pscErrMsgHandle errorHandle[PSC_ERROR_CHAIN_LENGTH];
   
} pscErrorHandler;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*! \brief Use for initializing our internal data. */
VDSF_COMMON_EXPORT
bool pscInitErrorDefs();

/*! \brief Clear our internal data. */
VDSF_COMMON_EXPORT
void pscFiniErrorDefs();

/*! \brief Add a function to handle error messages. */
VDSF_COMMON_EXPORT
pscErrMsgHandle pscAddErrorMsgHandler( const char         * name, 
                                       pscErrMsgHandler_T   handler );

/*! \brief Use for initializing the struct or to reset it, as needed. */
VDSF_COMMON_EXPORT
void pscInitErrorHandler( pscErrorHandler * pErrorHandler );

/*! \brief Terminate access to the struct pscErrorHandler */
VDSF_COMMON_EXPORT
void pscFiniErrorHandler( pscErrorHandler * pErrorHandler );

/*! Test to see if errors were found. 
 * 
 * \param[in] pErrorHandler A pointer to the pscErrorHandler struct itself.
 *
 * \retval true  (1) if there are errors.
 * \retval false (0) if no errors.
 *
 * \pre \em pErrorHandler cannot be NULL.
 */
static inline
bool pscAnyErrors( pscErrorHandler * pErrorHandler )
{
   VDS_PRE_CONDITION( pErrorHandler != NULL );

   return ( pErrorHandler->chainLength > 0 ); 
}

/*! \brief Retrieves the error message or a concatenation of all error 
 *         messages (if more than one)
 */
VDSF_COMMON_EXPORT
size_t pscGetErrorMsg( pscErrorHandler * pErrorHandler,
                       char            * msg, 
                       size_t            maxLength );

/*! \brief Retrieves the length of the error message or the length of a 
 *         concatenation of all error messages (if more than one) - the
 *         space for a NULL terminator is not included.
 */
VDSF_COMMON_EXPORT
size_t pscGetErrorMsgLength( pscErrorHandler * pErrorHandler );

/*! \brief 
 * Sets both the error code and the handler for the 
 * error message. It will first reset the chain of error codes to zero.
 */
VDSF_COMMON_EXPORT
void pscSetError( pscErrorHandler * pErrorHandler, 
                  pscErrMsgHandle   handle,
                  int               errorCode );   

/*! \brief
 * Adds the error code and the handler for the error message to an 
 * existing chain.
 */
VDSF_COMMON_EXPORT
void pscChainError( pscErrorHandler * pErrorHandler, 
                    pscErrMsgHandle   handle,
                    int               errorCode );  

/*! \brief
 * Retrieves the last error number or zero if no errors.
 */
static inline 
int pscGetLastError( pscErrorHandler * pErrorHandler )
{
   VDS_PRE_CONDITION( pErrorHandler != NULL );

   if ( pErrorHandler->chainLength > 0 )
      return pErrorHandler->errorCode[pErrorHandler->chainLength-1];
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * @}
 */

END_C_DECLS

#endif /* PSC_ERROR_HANDLER_H */

