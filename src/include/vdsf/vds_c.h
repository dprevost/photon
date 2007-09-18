/*
 * Copyright (C) 2006 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of vdsf (Virtual Data Space Framework) Library.
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

#ifndef VDS_C_H
#define VDS_C_H

#include "vdsf/vdsErrors.h"
#include "vdsf/vdsCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* VDS_HANDLE;

/**
 * This function initializes access to a VDS. It takes 2 input 
 * arguments, the address of the watchdog and an integer (used as 
 * a boolean, 0 for false, 1 for true) to indicate if sessions and 
 * other objects (Queues, etc) are shared amongst threads (in the 
 * current process) and must be protected. Recommendation: always 
 * set protectionNeeded to 0 (false) unless you cannot do it otherwise. 
 * In other words it is recommended to use one session handle for
 * each thread. Also if the same queue needs to be accessed by two 
 * threads it is more efficient to have two different handles instead
 * of sharing a single one.
 * 
 * [Additional note: API objects (or C handles) are just proxies for 
 * the real objects sitting in shared memory. Proper synchronization 
 * is already done in shared memory and it is best to avoid to 
 * synchronize these proxy objects.]
 * 
 * Upon successful completion, the process handle is set. Otherwise 
 * the error code is returned.
 */
int vdsInit( const char* wdAddress,
             int         protectionNeeded,
             VDS_HANDLE* processHandle );

/**
 * This function terminates all access to the VDS. This function 
 * will also close all sessions and terminate all accesses to 
 * the different objects. 
 * 
 * This function takes a single argument, the handle to the process object 
 * and always end successfully.
 */
void vdsExit( VDS_HANDLE processHandle );


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

int vdsInitSession( VDS_HANDLE* sessionHandle );

void vdsExitSession( VDS_HANDLE handle );
   
int vdsCreateObject( VDS_HANDLE handle,
                     const char*   objectName,
                     vdsObjectType objectType );
   
int vdsDestroyObject( VDS_HANDLE handle,
                      const char* objectName );

int vdsCommit( VDS_HANDLE handle );
   
int vdsRollback( VDS_HANDLE handle );

int vdsFolderOpen( VDS_HANDLE  sessionHandle,
                   const char* folderName,
                   VDS_HANDLE* objectHandle );

int vdsFolderClose( VDS_HANDLE objectHandle );


int vdsQueueOpen(  VDS_HANDLE  sessionHandle,
                   const char* queueName,
                   VDS_HANDLE* objectHandle );

int vdsQueueClose(  VDS_HANDLE objectHandle );
   

int vdsQueueInsertItem( VDS_HANDLE objectHandle, 
                        const void* pItem, 
                        size_t length );

int vdsQueueRemoveItem( VDS_HANDLE objectHandle, 
                        void* pItem, 
                        size_t length );

/* The next function does not remove the items from the queue */

int vdsQueueGetItem( VDS_HANDLE      objectHandle,
                     vdsIteratorType flag, 
                     void*           pItem, 
                     size_t          length );
   

int vdsQueueStatus( VDS_HANDLE objectHandle,
                    size_t *   pNumValidItems,
                    size_t *   pNumTotalItems );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
}
#endif
 
#endif /* VDS_C_H */
