/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of vdsf (Virtual Data Space Framework) Library.
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

#ifndef VDS_H
#define VDS_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include <vdsf/vdsErrors.h>
#include <vdsf/vdsCommon.h>

#include <vdsf/vdsProcess.h>
#include <vdsf/vdsSession.h>
#include <vdsf/vdsFolder.h>
/*
 * Hash maps and maps implements the same data container but differs with 
 * respect to transactions. 
 * 
 * Hash maps are read-write and locks are used to insure data integrity.
 * Maps are read-only (with an update procedure) and no locks are required.
 */
#include <vdsf/vdsHashMap.h>
#include <vdsf/vdsFastMap.h>
#include <vdsf/vdsQueue.h>

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDS_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

