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

#ifndef PSOT_BARRIER_H
#define PSOT_BARRIER_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Common/Common.h"
#include "Common/ErrorHandler.h"

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * This code is based on an article from
 * SunOpsis Vol. 4, No. 1 by Richard Marejka
 * (Richard.Marejka@canada.sun.com).
 *
 * To be honest, the technique was taken from code (for example
 * the ACE toolkit and the file barrier.c copyrighted by Sun and
 * written by R. Marejka) which were inspired by that article.
 * I haven't been able to find the article itself.
 *
 */

 
struct psotSubBarrier
{
#if defined (WIN32)
   HANDLE           waitEvent;
   CRITICAL_SECTION waitLock;
#else
   pthread_cond_t  waitVar;        /* cv for waiters at barrier    */
   pthread_mutex_t waitLock;        /* mutex for waiters at barrier */
#endif
   int     numRunners;        /* number of running threads    */
};
 
struct psotBarrier
{
   /** Set to PSOT_BARRIER_SIGNATURE at initialization. */
   unsigned int initialized;

   /** maximum number of running threads */
   int     numThreads;

   struct psotSubBarrier  subBarrier[2];
   int currentSubBarrier;
};

typedef struct psotBarrier psotBarrier;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*! \brief Initialize the struct psotBarrier. */
int psotInitBarrier( psotBarrier     * pBarrier,
                     int               numThreads,
                     psocErrorHandler * pError );

/*! \brief Terminate access to the struct psotBarrier. */
void psotFiniBarrier( psotBarrier * pBarrier );

void psotBarrierWait( psotBarrier * pBarrier );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * @}
 */

END_C_DECLS

#endif /* PSOT_BARRIER_H */

