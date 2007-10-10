/*
 * Copyright (C) 2006-2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef VDS_C_H
#define VDS_C_H

#include <vdsf/vdsErrors.h>
#include <vdsf/vdsCommon.h>

#include <vdsf/vdsProcess.h>
#include <vdsf/vdsSession.h>
#include <vdsf/vdsFolder.h>
#include <vdsf/vdsHashMap.h>
#include <vdsf/vdsQueue.h>

#ifdef __cplusplus
extern "C" {
#endif



VDSF_EXPORT
int vdsQueueOpen(  VDS_HANDLE  sessionHandle,
                   const char* queueName,
                   VDS_HANDLE* objectHandle );

VDSF_EXPORT
int vdsQueueClose(  VDS_HANDLE objectHandle );
   

VDSF_EXPORT
int vdsQueueInsertItem( VDS_HANDLE objectHandle, 
                        const void* pItem, 
                        size_t length );

VDSF_EXPORT
int vdsQueueRemoveItem( VDS_HANDLE objectHandle, 
                        void* pItem, 
                        size_t length );

/* The next function does not remove the items from the queue */

VDSF_EXPORT
int vdsQueueGetItem( VDS_HANDLE      objectHandle,
                     vdsIteratorType flag, 
                     void*           pItem, 
                     size_t          length );
   

VDSF_EXPORT
int vdsQueueStatus( VDS_HANDLE objectHandle,
                    size_t *   pNumValidItems,
                    size_t *   pNumTotalItems );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
}
#endif
 
#endif /* VDS_C_H */
