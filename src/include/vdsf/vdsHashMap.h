/*
 * Copyright (C) 2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef VDS_HASH_MAP_H
#define VDS_HASH_MAP_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include <vdsf/vdsCommon.h>

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
extern "C" {
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

VDSF_EXPORT
int vdsHashMapClose( VDS_HANDLE objectHandle );

VDSF_EXPORT
int vdsHashMapDelete( VDS_HANDLE   objectHandle,
                      const void * key,
                      size_t       keyLength );

VDSF_EXPORT
int vdsHashMapGet( VDS_HANDLE   objectHandle,
                   const void * key,
                   size_t       keyLength,
                   void       * buffer,
                   size_t       bufferLength,
                   size_t     * returnedLength );

VDSF_EXPORT
int vdsHashMapGetFirst( VDS_HANDLE   objectHandle,
                        void       * buffer,
                        size_t       bufferLength,
                        size_t     * returnedLength );

VDSF_EXPORT
int vdsHashMapGetNext( VDS_HANDLE   objectHandle,
                       void       * buffer,
                       size_t       bufferLength,
                       size_t     * returnedLength );

VDSF_EXPORT
int vdsHashMapInsert( VDS_HANDLE   objectHandle,
                      const void * key,
                      size_t       keyLength,
                      const void * data,
                      size_t       dataLength );

VDSF_EXPORT
int vdsHashMapOpen( VDS_HANDLE   sessionHandle,
                    const char * hashMapName,
                    size_t       nameLengthInBytes,
                    VDS_HANDLE * objectHandle );

VDSF_EXPORT
int vdsHashMapStatus( VDS_HANDLE     objectHandle,
                      vdsObjStatus * pStatus );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDS_HASH_MAP_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

