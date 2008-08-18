/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef VDST_BARRIER_H
#define VDST_BARRIER_H

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

 
struct vdstSubBarrier
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
 
struct vdstBarrier
{
   /** Set to VDST_BARRIER_SIGNATURE at initialization. */
   unsigned int initialized;

   /** maximum number of running threads */
   int     numThreads;

   struct vdstSubBarrier  subBarrier[2];
   int currentSubBarrier;
};

typedef struct vdstBarrier vdstBarrier;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*! \brief Initialize the struct vdstBarrier. */
int vdstInitBarrier( vdstBarrier     * pBarrier,
                     int               numThreads,
                     pscErrorHandler * pError );

/*! \brief Terminate access to the struct vdstBarrier. */
void vdstFiniBarrier( vdstBarrier * pBarrier );

void vdstBarrierWait( vdstBarrier * pBarrier );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * @}
 */

END_C_DECLS

#endif /* VDST_BARRIER_H */

