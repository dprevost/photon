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

#ifndef PSO_INFO_H
#define PSO_INFO_H

#include <google/protobuf-c/protobuf-c.h>

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This data structure is used to retrieve the status of the Photon 
 * shared memory.
 */
struct psoInfo
{
   ProtobufCMessage base;

   /** Total size of the virtual data space. */
   uint64_t total_size_in_bytes;

   /** Total size of the allocated blocks. */
   uint64_t allocated_size_in_bytes;

   /** 
    * Number of API objects in the Photon shared memory.
    *
    * Internal objects are not counted. 
    */
   uint64_t num_objects;

   /** Total number of groups of blocks. */
   uint64_t num_groups;

   protobuf_c_boolean has_num_mallocs;

   /** Number of calls to allocate groups of blocks. */
   uint64_t num_mallocs;

   protobuf_c_boolean has_num_frees;

   /** Number of calls to free groups of blocks. */
   uint64_t num_frees;

   /** Largest contiguous group of free blocks. */
   uint64_t largest_free_in_bytes;

   /** Shared-memory version. */
   int32_t memory_version;

   /** Endianess (0 for little endian, 1 for big endian). */
   protobuf_c_boolean big_endian;

   /** Compiler name. */
   char *compiler;

   /** Compiler version (if available). */
   char *compiler_version;

   /** Platform. */
   char *platform;

   /** Shared lib version. */
   char *dll_version;

   /** The version of quasar used to create the shared memory. */
   char *quasar_version;

   /** Timestamp of the creation of the shared memory. */
   char *creation_time;


/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
}
#endif

#endif /* PSO_INFO_H */
