/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of photon (photonsoftware.org).
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

#ifndef PSN_LOG_FILE_H
#define PSN_LOG_FILE_H

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
#define PSN_LOGFILE_SIGNATURE ((unsigned int)0xf211c428)

/**
 * This module is used to log transactions to disk once they are committed.
 * Rollback transactions do not need to be logged since they will be treated
 * the same as an incomplete transactions during a crash recovery (the data
 * will be discarded).
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psnLogFile
{
   /** File handle as returned by open() */
   int handle;

   /** Set to PSN_LOGFILE_SIGNATURE at initialization. */
   unsigned int initialized;

   /** File name (the full path) */
   char filename[PATH_MAX];
   
};

typedef struct psnLogFile psnLogFile;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

VDSF_ENGINE_EXPORT
void psnCloseLogFile( psnLogFile      * logFile,
                       pscErrorHandler * pError );

/**
 * Initialize the log file.
 *
 * pSession is only used to uniquely identify the log file (since each 
 * session of a given process would otherwise have the same name).
 */
VDSF_ENGINE_EXPORT
psoErrors psnInitLogFile( psnLogFile      * logFile,
                           const char       * dirName,
                           void             * pSession,
                           pscErrorHandler * pError );
   
VDSF_ENGINE_EXPORT
psoErrors psnLogTransaction( psnLogFile      * logFile,
                              int                transactionId,
                              pscErrorHandler * pError );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSN_LOG_FILE_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
