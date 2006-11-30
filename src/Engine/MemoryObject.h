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
#include "LinkNode.h"
#include "LinkedList.h"
#include "SessionContext.h"

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define VDSE_MEM_ALIGNMENT 4

/**
 * MemoryObject includes all the data containers (queues, etc.), the folders
 * and all "hidden" top level objects (the allocator, the sessions recovery
 * objects, etc.).
 *
 * This struct should always be the first member of the struct defining an
 * object. This way, the identifier is always at the top of a page and it
 * should help debug, recover from crashes, etc. 
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

   /** Total number of pages for the current object */
   size_t totalPages;
   
   vdseLinkedList listPageGroup;
   
} vdseMemObject;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum vdsErrors 
vdseMemObjectInit( vdseMemObject*        pMemObj,
                   enum ObjectIdentifier objType,
                   size_t                numPages );

enum vdsErrors 
vdseMemObjectFini( vdseMemObject* pMemObj );

unsigned char* vdseMalloc( vdseMemObject*      pMemObj,
                           size_t              numBytes,
                           vdseSessionContext* pContext );

void vdseFree( vdseMemObject*      pMemObj,
               unsigned char*      ptr, 
               size_t              numBytes,
               vdseSessionContext* pContext );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

#endif /* VDSE_MEMORY_OBJECT_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

