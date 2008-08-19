/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of photon (photonsoftware.org).
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

#ifndef PSN_MEMORY_HEADER_H
#define PSN_MEMORY_HEADER_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Nucleus/Engine.h"
#include "Nucleus/TreeNode.h"
#include "Nucleus/Hash.h"

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define PSN_MEMORY_VERSION 1

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * The memory header is allocated at the beginning of the memory-mapped 
 * file (or whatever else replaces it). Although this structure is
 * quite small - 1 block will be reserved for it - this will garantee that 
 * we can modify VDS on the fly if this structure is changed in future
 * releases.
 */
struct psnMemoryHeader
{
   char            cookie[4];
   int             version;
   int             running;  /* flag that will indicates system crashes */
   ptrdiff_t       treeMgrOffset;
   ptrdiff_t       processMgrOffset;
   ptrdiff_t       allocatorOffset;
   bool            logON;
   size_t          totalLength;
   
   size_t   sizeofPtr;
   bool     bigEndian;
   size_t   blockSize;
   size_t   alignmentStruct;
   size_t   alignmentChar;
   size_t   alignmentInt16;
   size_t   alignmentInt32;
   size_t   alignmentInt64;
   size_t   allocationUnit;
   
   char     cpu_type[20];
   char     compiler[20];
   char     cxxcompiler[20];
   char     compilerVersion[10];
   char     watchdogVersion[10];
   char     creationTime[30];
   
   psnHashItem topHashItem;
   psnObjectDescriptor topDescriptor;
   
};

typedef struct psnMemoryHeader psnMemoryHeader;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSN_MEMORY_HEADER_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

