/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@photonsoftware.org>
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

#ifndef PSO_PROCESS_H
#define PSO_PROCESS_H

#include <photon/psoCommon.h>

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \file
 * This file provides the API functions for Photon processes.
 */
/**
 * \defgroup psoProcess_c API functions for Photon processes.
 */
/*@{*/

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This function terminates all access to shared memory. This function 
 * will also close all sessions and terminate all accesses to 
 * the different objects. 
 * 
 * This function takes no argument and always end successfully (even
 * if called twice or if ::psoInit was not called).
 */
PHOTON_EXPORT
void psoExit();

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This function initializes access to the shared-memory of Photon.
 * It takes 2 input arguments, the address of Quasar, the Photon server,
 * and an integer (used as a boolean, 0 for false, 1 for true) to 
 * indicate if sessions and other objects (Queues, etc) are shared 
 * amongst threads (in the current process) and must be protected. 
 * Recommendation: always set protectionNeeded to 0 (false) unless you 
 * cannot do otherwise. 
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
 *
 * \param[in] quasarAddress The address of Quasar. Currently a string with 
 *            the port number ("12345").
 * \param[in] protectionNeeded A boolean value indicating if multi-threaded
 *            locks are needed or not.
 *
 * \return 0 on success or a ::psoErrors on error.

 */
PHOTON_EXPORT
int psoInit( const char * quasarAddress,
             int          protectionNeeded );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*@}*/

#ifdef __cplusplus
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSO_PROCESS_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

