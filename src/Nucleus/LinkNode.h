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

#ifndef PSON_LINK_NODE_H
#define PSON_LINK_NODE_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Nucleus/Engine.h"       

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * A psonLinkNode is the unit element of our linked lists. This might
 * seems like a trivial struct but since we use linked lists in many 
 * places, using a struct for the previous and next element simplify 
 * some of the code. Also, do not forget that our linked lists do 
 * not act on pointers but used offsets instead like everything else
 * in shared memory. Using a struct also ensures that the type of previous
 * and next elements are ptrdiff_t and not int or something like this 
 * (int (or u_int?) would probably work on 32bits and fail on some 
 * 64bits machines - this type of error/bug is difficult to find, better
 * not make it in the first place...).
 */
struct psonLinkNode
{
   ptrdiff_t nextOffset;
   ptrdiff_t previousOffset;
};

typedef struct	psonLinkNode psonLinkNode;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * A "specialized" version of psonLinkNode. It is used in many places
 * and was put here to avoid circular dependencies...
 */
struct psonFreeBufferNode
{
   /* The linked node itself */
   psonLinkNode node;
   
   /* The number of buffers associate with each member of the list. */
   size_t numBuffers;
   
};

typedef struct psonFreeBufferNode psonFreeBufferNode;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** Initialize the elements of the psonLinkNode to PSON_NULL_OFFSET. */
static inline
void psonLinkNodeInit( psonLinkNode * pNode )
{
   PSO_PRE_CONDITION( pNode != NULL );
   
   pNode->nextOffset     = PSON_NULL_OFFSET;
   pNode->previousOffset = PSON_NULL_OFFSET;
}

static inline
void psonLinkNodeFini( psonLinkNode * pNode )
{
   PSO_PRE_CONDITION( pNode != NULL );

   psonLinkNodeInit( pNode );
}

/** Test the values of the elements of the psonLinkNode.
 *  Return true if both elements are not the PSON_NULL_OFFSET
 *  and false if one or both are the PSON_NULL_OFFSET.
 */
static inline
bool psonLinkNodeTest( psonLinkNode * pNode )
{
   PSO_PRE_CONDITION( pNode != NULL );

   if ( pNode->nextOffset     == PSON_NULL_OFFSET || 
        pNode->previousOffset == PSON_NULL_OFFSET ) {
      return false;
   }
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSON_LINK_NODE_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
