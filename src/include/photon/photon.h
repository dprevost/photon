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

#ifndef PSO_H
#define PSO_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include <photon/psoErrors.h>
#include <photon/psoCommon.h>
#include <photon/psoPhotonODBC.h>

#include <photon/Process.h>
#include <photon/Session.h>
#include <photon/DataDefinition.h>
#include <photon/KeyDefinition.h>
#include <photon/Folder.h>
/*
 * Hash maps and fast maps implements the same data container but differs 
 * with respect to transactions. 
 * 
 * Hash maps are read-write and locks are used to insure data integrity.
 * Fast maps are read-only (with an update procedure) and no locks are 
 * required.
 */
#include <photon/HashMap.h>
#include <photon/FastMap.h>
#include <photon/Queue.h>
#include <photon/Lifo.h>

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSO_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

