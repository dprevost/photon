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

#ifndef PSON_LOG_FILE_H
#define PSON_LOG_FILE_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Nucleus/Engine.h"
#include "Common/ErrorHandler.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define PSO_LOG_EXT "tlog"

/* Random generated signature... the first 8 bytes of the digest (md5)
 * of LogFile.c at that time... The signature is used to indicate
 * that the struct was properly initialized.
 */
#define PSON_LOGFILE_SIGNATURE ((unsigned int)0xf211c428)

/**
 * This module is used to log transactions to disk once they are committed.
 * Rollback transactions do not need to be logged since they will be treated
 * the same as an incomplete transactions during a crash recovery (the data
 * will be discarded).
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psonLogFile
{
   /** File handle as returned by open() */
   int handle;

   /** Set to PSON_LOGFILE_SIGNATURE at initialization. */
   unsigned int initialized;

   /** File name (the full path) */
   char filename[PATH_MAX];
   
};

typedef struct psonLogFile psonLogFile;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

PHOTON_ENGINE_EXPORT
void psonCloseLogFile( psonLogFile      * logFile,
                       psocErrorHandler * pError );

/**
 * Initialize the log file.
 *
 * pSession is only used to uniquely identify the log file (since each 
 * session of a given process would otherwise have the same name).
 */
PHOTON_ENGINE_EXPORT
psoErrors psonInitLogFile( psonLogFile      * logFile,
                           const char       * dirName,
                           void             * pSession,
                           psocErrorHandler * pError );
   
PHOTON_ENGINE_EXPORT
psoErrors psonLogTransaction( psonLogFile      * logFile,
                              int                transactionId,
                              psocErrorHandler * pError );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSON_LOG_FILE_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

