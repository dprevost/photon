/*
 * Copyright (C) 2006-2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef VDS_QUEUE_H
#define VDS_QUEUE_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include <vdsf/vdsCommon.h>

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
extern "C" {
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** Close a FIFO queue. */
VDSF_EXPORT
int vdsQueueClose(  VDS_HANDLE objectHandle );
   
/** Insert a data element in the queue. */
VDSF_EXPORT
int vdsQueuePush( VDS_HANDLE   objectHandle, 
                  const void * pItem, 
                  size_t       length );

/** Open a FIFO queue. */
VDSF_EXPORT
int vdsQueueOpen(  VDS_HANDLE   sessionHandle,
                   const char * queueName,
                   size_t       nameLengthInBytes,
                   VDS_HANDLE * objectHandle );

/** Remove the first inserted item from a FIFO queue. */
VDSF_EXPORT
int vdsQueuePop( VDS_HANDLE   objectHandle,
                 vdsDataEntry * pEntry );

/** 
 * Release the "soft lock" on the data - to be used when you're done with 
 * vdsQueuePop() or vdsQueueGetItem().
 */
VDSF_EXPORT
int vdsQueueRelease( VDS_HANDLE     objectHandle,
                     vdsDataEntry * pEntry );

/** 
 * Iterates through the queue - no data items are removed from the queue
 * by this function.
 */
VDSF_EXPORT
int vdsQueueGetFirst( VDS_HANDLE        objectHandle,
                      vdsDataEntry    * pEntry );

/** 
 * Iterates through the queue - no data items are removed from the queue
 * by this function.
 */
VDSF_EXPORT
int vdsQueueGetNext( VDS_HANDLE        objectHandle,
                     vdsDataEntry    * pEntry );

#if 0
VDSF_EXPORT
int vdsQueueStatus( VDS_HANDLE   objectHandle,
                    size_t     * pNumValidItems,
                    size_t     * pNumTotalItems );
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDS_QUEUE_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

