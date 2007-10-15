/*
 * Copyright (C) 2006-2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef VDS_SESSION_H
#define VDS_SESSION_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include <vdsf/vdsCommon.h>

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
extern "C" {
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This function initializes a session. It takes one output argument, 
 * the session handle.
 * 
 * Upon successful completion, the session handle is set and the function
 * returns zero. Otherwise the error code is returned and the handle is set
 * to NULL.
 * 
 * This function will also initiate a new transaction:
 *
 * Contrary to some other transaction management software, almost every 
 * call made is part of a transaction. Even viewing data (for example 
 * deleting the data by another session will be delayed until the current
 * session terminates its access).
 *
 * Upon normal termination, the current transaction is rolled back. You
 * MUST explicitly call vdseCommit to save your changes.
 */
VDSF_EXPORT
int vdsInitSession( VDS_HANDLE* sessionHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

VDSF_EXPORT
int vdsExitSession( VDS_HANDLE handle );
   
VDSF_EXPORT
int vdsCreateObject( VDS_HANDLE      handle,
                     const char    * objectName,
                     size_t          nameLengthInBytes,
                     vdsObjectType   objectType );
   
VDSF_EXPORT
int vdsDestroyObject( VDS_HANDLE   handle,
                      const char * objectName,
                      size_t       nameLengthInBytes );

VDSF_EXPORT
int vdsCommit( VDS_HANDLE handle );
   
VDSF_EXPORT
int vdsRollback( VDS_HANDLE handle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDS_SESSION_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

