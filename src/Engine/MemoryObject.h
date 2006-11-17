/*
 * Copyright (C) 2006 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef VDSE_MEMORY_OBJECT_H
#define VDSE_MEMORY_OBJECT_H

#include "Engine.h"

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define VDSE_MEM_ALIGNMENT 4

/**
 * This structure enables "navigation" of the pages allocated to a
 * memory object. 
 */
typedef struct vdsePageNavig
{
   /** Number of page in the current group of pages */
   size_t numPagesGroup;
   
   /** Offset to the next group of pages for the object or NULL_OFFSET if
    * we are at the end. 
    */
   ptrdiff_t nextGroupOfPages;
   
} vdsePageNavig;

/**
 * MemoryObject includes all the data containers (queues, etc.), the folders
 * and all "hidden" top level objects (the allocator, the sessions recovery
 * objects, etc.).
 */
typedef struct vdseMemObject
{
   /** Type of memory object */
   enum ObjectIdentifier objType;
   
   /** The lock... obviously */
   vdscProcessLock lock;

   /** Counts the number of clients who are accessing the object.
    * It should only be used from a Folder object, when the 
    * TreeManager is locked!
    */
   size_t accessCounter;

   /** The number of free bytes in the object allocated memory. */
   size_t remainingBytes;
   
   /** Used to navigate through the allocated pages of memory of an object. */
   vdsePageNavig navigator;
   
} vdseMemObject;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum vdsErrors 
vdseMemObjectInit( vdseMemObject*        pMemObj,
                   enum ObjectIdentifier objType,
                   size_t                objSize,
                   size_t                numPages );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

#endif /* VDSE_MEMORY_OBJECT_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

