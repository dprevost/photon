/* -*- c++ -*- */
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

#ifndef VDSE_LINK_NODE_H
#define VDSE_LINK_NODE_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Engine.h"       

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * A vdseLinkNode is the unit element of our linked lists. This might
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
typedef struct	vdseLinkNode
{
   volatile ptrdiff_t nextOffset;
   volatile ptrdiff_t previousOffset;

} vdseLinkNode;

/** Initialize the elements of the vdseLinkNode to NULL_OFFSET. */
static inline
void vdseLinkNodeInit( vdseLinkNode* pNode )
{
   VDS_PRE_CONDITION( pNode != NULL );
   
   pNode->nextOffset     = NULL_OFFSET;
   pNode->previousOffset = NULL_OFFSET;
}

/** Test the values of the elements of the vdseLinkNode.
 *  Return true (1) if both elements are not the NULL_OFFSET
 *  and false (0) if one or both are the NULL_OFFSET.
 */
static inline
int  vdseLinkNodeTest( vdseLinkNode* pNode )
{
   VDS_PRE_CONDITION( pNode != NULL );

   if ( pNode->nextOffset     == NULL_OFFSET || 
        pNode->previousOffset == NULL_OFFSET )
   {
      return 0;
   }
   return 1;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSE_LINK_NODE_H */