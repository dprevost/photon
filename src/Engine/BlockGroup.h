/*
 * Copyright (C) 2006, 2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef VDSE_PAGE_GROUP_H
#define VDSE_PAGE_GROUP_H

#include "Engine.h"
#include "MemBitmap.h"
#include "LinkNode.h"
#include "LinkedList.h"

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct vdseBlockGroup
{
   /* The linked list itself */
   vdseLinkNode node;

   /* The number of blocks associate with each member of the list. */
   size_t numBlocks;

   size_t maxFreeBytes;
   
   vdseLinkedList freeList;

   bool isDeletable;
   
   /* Must be last since this struct contains a "variable-length" array. */
   vdseMemBitmap bitmap;
   
} vdseBlockGroup;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Initialize the vdseBlockGroup struct. 
 */
VDSF_ENGINE_EXPORT
void vdseBlockGroupInit( vdseBlockGroup* pGroup,
                         ptrdiff_t      groupOffset,
                         size_t         numBlocks );

VDSF_ENGINE_EXPORT
void vdseBlockGroupFini( vdseBlockGroup* pGroup );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

#endif /* VDSE_PAGE_GROUP_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
