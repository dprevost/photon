/* -*- c++ -*- */
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

#ifndef QSR_HANDLER_H
#define QSR_HANDLER_H

#include "Nucleus/SessionContext.h"
#include "Quasar/quasarErrors.h"

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

// Forward declaration
struct ConfigParams;
struct psonMemoryHeader;
struct psonSession;
struct qsrMemoryManager;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 *  This class initialize the shared memory. 
 *    - it will create the shared memory if it does not exist.
 *    - it will open it otherwise and make sure that its content is valid
 */

struct qsrHandler
{
   struct ConfigParams * pConfig;

   struct qsrMemoryManager * pMemManager;

   struct psonMemoryHeader * pMemHeader;
   
   psonSessionContext context;
};

typedef struct qsrHandler qsrHandler;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool qsrHandlerInit( qsrHandler               * pHandler,
                     struct ConfigParams      * pConfig,
                     struct psonMemoryHeader ** ppMemoryAddress,
                     bool                       verifyMemOnly );

void qsrHandlerFini( qsrHandler * pHandler );

void qsrHandleCrash( qsrHandler * pHandler, pid_t pid );

bool qsrHandlerCreateSession( qsrHandler * pHandler );

bool qsrHandlerSystemObjects( qsrHandler * pHandler );

#if 0
   void CleanSession( psonSession* pSession );
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

#endif /* QSR_HANDLER_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

