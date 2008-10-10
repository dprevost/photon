/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@photonsoftware.org>
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

#ifndef PSO_H
#define PSO_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include <photon/psoErrors.h>
#include <photon/psoCommon.h>

#include <photon/psoProcess.h>
#include <photon/psoSession.h>
#include <photon/psoFolder.h>
/*
 * Hash maps and maps implements the same data container but differs with 
 * respect to transactions. 
 * 
 * Hash maps are read-write and locks are used to insure data integrity.
 * Maps are read-only (with an update procedure) and no locks are required.
 */
#include <photon/psoHashMap.h>
#include <photon/psoFastMap.h>
#include <photon/psoQueue.h>
#include <photon/psoLifo.h>

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSO_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

