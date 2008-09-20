/* -*- c++ -*- */
/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of Photon (photonsoftware.org).
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

#ifndef PSOQ_HANDLER_H
#define PSOQ_HANDLER_H

#include "Nucleus/SessionContext.h"
#include "Quasar/quasarErrors.h"

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

// Forward declaration
struct ConfigParams;
struct psonMemoryHeader;
struct psonSession;
struct psoqMemoryManager;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 *  This class initialize the shared memory. 
 *    - it will create the shared memory if it does not exist.
 *    - it will open it otherwise and make sure that its content is valid
 */

struct psoqHandler
{
   struct ConfigParams * pConfig;

   struct psoqMemoryManager * pMemManager;

   struct psonMemoryHeader * pMemHeader;
   
   psonSessionContext context;
};

typedef struct psoqHandler psoqHandler;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psoqHandlerInit( psoqHandler              * pHandler,
                      struct ConfigParams      * pConfig,
                      struct psonMemoryHeader ** ppMemoryAddress,
                      bool                       verifyVDSOnly );

void psoqHandlerFini( psoqHandler * pHandler );

void psoqHandleCrash( psoqHandler * pHandler, pid_t pid );

#if 0
   void CleanSession( psonSession* pSession );
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

#endif /* PSOQ_HANDLER_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

