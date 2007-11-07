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

#ifndef VDS_PROCESS_H
#define VDS_PROCESS_H

#include <vdsf/vdsCommon.h>

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
extern "C" {
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This function terminates all access to the VDS. This function 
 * will also close all sessions and terminate all accesses to 
 * the different objects. 
 * 
 * This function takes a single argument, the handle to the process object 
 * and always end successfully.
 */
VDSF_EXPORT
void vdsExit( VDS_HANDLE processHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

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
VDSF_EXPORT
int vdsInit( const char* wdAddress,
             int         protectionNeeded,
             VDS_HANDLE* processHandle );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDS_PROCESS_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

