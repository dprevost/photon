/*
 * Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#ifndef PSOA_SESSION_H
#define PSOA_SESSION_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "API/api.h"
#include "Common/ThreadLock.h"
#include "Nucleus/SessionContext.h"
#include "API/Process.h"
#include "API/ListReaders.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psonSession;
struct psonMemoryHeader;
struct psoaCommonObject;

/**
 * This class handles transactions and other session wide concerns. For 
 * example, opening, closing, creating and destroying shared memory 
 * objects.
 */

typedef struct psoaSession
{
   psoaObjetType type;
   
   psonSessionContext context;

   /** Pointer to the header of the shared memory */
   struct psonMemoryHeader* pHeader;

   struct psonSession* pCleanup;

   /** Pointer to the hashmap holding the data definitions */
   struct psonHashMap * pDataDefMap;

   /** Pointer to the hashmap holding the key definitions */
   struct psonHashMap * pKeyDefMap;
   
   bool terminated;
   
   int numberOfObjects;
   
   int numberOfEdits;
   
   psoaListReaders listReaders;
   
} psoaSession;


/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * No lock is taken on the psoaSession.
 */
PHOTON_API_EXPORT
int psoaCloseSession( psoaSession* pSession );

PHOTON_API_EXPORT
int psoaSessionCloseObj( psoaSession             * pSession,
                         struct psoaCommonObject * pObject );

PHOTON_API_EXPORT
int psoaSessionOpenObj( psoaSession             * pSession,
                        psoObjectType             objectType,
                        psoaEditMode              editMode,
                        const char              * objectName,
                        uint32_t                  nameLengthInBytes,
                        struct psoaCommonObject * pObject );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSOA_SESSION_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

