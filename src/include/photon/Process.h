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
void psoExit(void);

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This function initializes access to the shared-memory of Photon.
 *
 * The process name is optional. If not used, the process id will be used
 * to generate a name automatically. 
 *
 * Process names are unique - if a process with the same name already exist,
 * a number will be appended to the name.
 *
 * This feature creates a system folder under the "\proc" folder of Photon
 * that will be used in future releases to put process and session specific
 * information, for examples statistics for the current process.
 *
 * \param[in] address The address of Quasar, currently a string with 
 *            the port number and the prefix "port:" - "port:12345".
 *            It can also be the name of the shared-memory file - in this 
 *            case add the prefix "file:" to the file name.
 * \param[in] processName An optional name to identify the process. This 
 *            name can be NULL (the process id of the process will be used
 *            to generate the name).
 *
 * \return 0 on success or a ::psoErrors on error.
 */
PHOTON_EXPORT
int psoInit( const char * address, const char * processName );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*@}*/

#ifdef __cplusplus
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSO_PROCESS_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

