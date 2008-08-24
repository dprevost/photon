/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of Photon (photonsoftware.org).
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

#ifndef PSN_LINK_NODE_H
#define PSN_LINK_NODE_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Nucleus/Engine.h"       

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * A psnLinkNode is the unit element of our linked lists. This might
 * seems like a trivial struct but since we use linked lists in many 
 * places, using a struct for the previous and next element simplify 
 * some of the code. Also, do not forget that our linked lists do 
 * not act on pointers but used offsets instead like everything else
 * in the VDS. Using a struct also ensures that the type of previous
 * and next elements are ptrdiff_t and not int or something like this 
 * (int (or u_int?) would probably work on 32bits and fail on some 
 * 64bits machines - this type of error/bug is difficult to find, better
 * not make it in the first place...).
 */
struct psnLinkNode
{
   ptrdiff_t nextOffset;
   ptrdiff_t previousOffset;
};

typedef struct	psnLinkNode psnLinkNode;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * A "specialized" version of psnLinkNode. It is used in many places
 * and was put here to avoid circular dependencies...
 */
struct psnFreeBufferNode
{
   /* The linked node itself */
   psnLinkNode node;
   
   /* The number of buffers associate with each member of the list. */
   size_t numBuffers;
   
};

typedef struct psnFreeBufferNode psnFreeBufferNode;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** Initialize the elements of the psnLinkNode to PSN_NULL_OFFSET. */
static inline
void psnLinkNodeInit( psnLinkNode * pNode )
{
   PSO_PRE_CONDITION( pNode != NULL );
   
   pNode->nextOffset     = PSN_NULL_OFFSET;
   pNode->previousOffset = PSN_NULL_OFFSET;
}

static inline
void psnLinkNodeFini( psnLinkNode * pNode )
{
   PSO_PRE_CONDITION( pNode != NULL );

   psnLinkNodeInit( pNode );
}

/** Test the values of the elements of the psnLinkNode.
 *  Return true if both elements are not the PSN_NULL_OFFSET
 *  and false if one or both are the PSN_NULL_OFFSET.
 */
static inline
bool psnLinkNodeTest( psnLinkNode * pNode )
{
   PSO_PRE_CONDITION( pNode != NULL );

   if ( pNode->nextOffset     == PSN_NULL_OFFSET || 
        pNode->previousOffset == PSN_NULL_OFFSET ) {
      return false;
   }
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSN_LINK_NODE_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
