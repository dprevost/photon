/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@photonsoftware.org>
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

#ifndef QSR_VERIFY_PSO_H
#define QSR_VERIFY_PSO_H

#include "Nucleus/MemoryHeader.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void qsrVerify( psonMemoryHeader * pMemoryAddress, 
                 size_t           * pNumObjectsOK,
                 size_t           * pNumObjectsRepaired,
                 size_t           * pNumObjectsDeleted,
                 size_t           * pNumObjectsError,
                 FILE             * fp );

void qsrRepair( psonMemoryHeader * pMemoryAddress,
                 size_t           * pNumObjectsOK,
                 size_t           * pNumObjectsRepaired,
                 size_t           * pNumObjectsDeleted,
                 size_t           * pNumObjectsError,
                 FILE             * fp );
     
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* QSR_VERIFY_PSO_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

