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

#ifndef PSOT_THREAD_WRAP_H
#define PSOT_THREAD_WRAP_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Common/Common.h"
#include "Common/ErrorHandler.h"

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * A very simple (simplistic?) wrapper for creating either Posix threads or
 * Win32 threads. It could be used to wrap other thread models (VxWorks
 * or other real-time OSes) as needed.
 *
 * The current implementation is essentially build for implementing 
 * multi-threads tests (to avoid having to always implement two different
 * set of tests for the two basic thread models).
 *
 * Restriction for Windows: the wrapper cannot be used in conjonction with
 * MFC. Thread creation in MFC should be done with AfxBeginThread() (the 
 * current code creates threads with beginthreadex()).
 *
 * This code might eventually be migrated to the Common directory if 
 * it is used by other parts of Photon.
 */
 
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef int (*PSOT_THREAD_FUNCTION)(void*);

typedef struct psotThreadWrap
{
#if defined (WIN32)
   /** The WIN32 thread identifier */
   HANDLE hThread;
#else
   /** The POSIX thread identifier */
   pthread_t threadId;
#endif

   /** Address of the routine to be executed */
   PSOT_THREAD_FUNCTION startRoutine;
   
   /** Argument to the thread */
   void* arg;

   /** Return code of the thread */
   int returnCode;
   
} psotThreadWrap;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psotCreateThread( psotThreadWrap*      pThread, 
                      PSOT_THREAD_FUNCTION startRoutine,
                      void*                arg,
                      psocErrorHandler*    pError );

int psotJoinThread( psotThreadWrap*   pThread, 
//                    void*             retValue,
                    psocErrorHandler* pError);
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSOT_THREAD_WRAP_H */
