/*
 * Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#ifndef PSON_TREE_NODE_H
#define PSON_TREE_NODE_H

#include "Engine.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
	
struct psonObjectDescriptor
{
   /** Offset of the object */
   ptrdiff_t offset;
   
   /** Offset to the psonTreeNode struct. */
   ptrdiff_t nodeOffset;
   
   /** Offset to the memory object struct. */
   ptrdiff_t memOffset;
   
   /** The object type as seen from the API. */
   enum psoObjectType apiType;
   
   /** 
    * The length in bytes of the name as originally entered.
    */
   int nameLengthInBytes;
   
   /** 
    * The original name (not including the parent folder name).
    * This name does include the trailing zero ('\0'),
    * and can be used as a valid C string!
    */
   char originalName[1];
   
};

typedef struct psonObjectDescriptor psonObjectDescriptor;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This structure contains the information needed for all the leaves and
 * the branches of the tree of objects/containers visible from the API.
 */
struct psonTreeNode
{
   /** Count the number of uncommitted/unrollbacked transaction ops are
    * still to be processed on this object (or more exactly on its data).
    * This is used to make sure that an object is not destroyed while
    * transactions (on its data) are not processed yet.
    */
   size_t txCounter;
  
   /** In units of char, not bytes. */
   size_t myNameLength;
   
   /** Offset to the original string naming this object. */
   ptrdiff_t myNameOffset;

   /** Offset to the string used for the key (lowercase of the original). */
   ptrdiff_t myHashItem;
   
   /** Offset to the transaction info (psonTxStatus). */
   ptrdiff_t txStatusOffset;

   /** Offset to the parent of this object. */
   /* PSON_NULL_OFFSET for top folder ("/") */
   ptrdiff_t myParentOffset;
   
};

typedef struct psonTreeNode psonTreeNode;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline 
void psonTreeNodeInit( psonTreeNode * pNode,
                       ptrdiff_t      txStatusOffset,
                       uint32_t       originalNameLength,
                       ptrdiff_t      originalNameOffset,
                       ptrdiff_t      parentOffset,
                       ptrdiff_t      hashItemOffset )
{
   PSO_PRE_CONDITION( pNode != NULL );
   
   pNode->txCounter      = 0;
   pNode->myNameLength   = originalNameLength;
   pNode->myNameOffset   = originalNameOffset;
   pNode->txStatusOffset = txStatusOffset;
   pNode->myParentOffset = parentOffset;
   pNode->myHashItem     = hashItemOffset;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline 
void psonTreeNodeFini( psonTreeNode * pNode )
{
   PSO_PRE_CONDITION( pNode != NULL );
   
   pNode->txCounter      = 0;
   pNode->myNameLength   = 0;
   pNode->myNameOffset   = PSON_NULL_OFFSET;
   pNode->txStatusOffset = PSON_NULL_OFFSET;
   pNode->myParentOffset = PSON_NULL_OFFSET;
   pNode->myHashItem     = PSON_NULL_OFFSET;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSON_TREE_NODE_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

