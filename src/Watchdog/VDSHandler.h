/* -*- c++ -*- */
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

#ifndef VDSW_HANDLER_H
#define VDSW_HANDLER_H

#include "Nucleus/SessionContext.h"
#include "Watchdog/quasarErrors.h"

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

// Forward declaration
struct ConfigParams;
struct psnMemoryHeader;
struct psnSession;
struct vdswMemoryManager;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 *  This class initialize the VDS. 
 *    - it will create the VDS if it does not exist.
 *    - it will open it otherwise and make sure that its content is valid
 */

struct vdswHandler
{
   struct ConfigParams * pConfig;

   struct vdswMemoryManager * pMemManager;

   struct psnMemoryHeader * pMemHeader;
   
   psnSessionContext context;
};

typedef struct vdswHandler vdswHandler;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool vdswHandlerInit( vdswHandler              * pHandler,
                      struct ConfigParams      * pConfig,
                      struct psnMemoryHeader ** ppMemoryAddress,
                      bool                       verifyVDSOnly );

void vdswHandlerFini( vdswHandler * pHandler );

void vdswHandleCrash( vdswHandler * pHandler, pid_t pid );

#if 0
   void CleanSession( psnSession* pSession );
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

#endif /* VDSW_HANDLER_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

