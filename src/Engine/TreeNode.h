/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef VDSE_TREE_NODE_H
#define VDSE_TREE_NODE_H

#include "Engine.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
	
struct vdseObjectDescriptor
{
   /** Offset of the object */
   ptrdiff_t offset;
   
   /** Offset to the vdseTreeNode struct. */
   ptrdiff_t nodeOffset;
   
   /** Offset to the memory object struct. */
   ptrdiff_t memOffset;
   
   /** The object type as seen from the API. */
   enum vdsObjectType apiType;
   
   /** 
    * The length in bytes of the name as originally entered (but stored in
    * multi-bytes characters, if i18n is supportd).
    */
   int nameLengthInBytes;
   
   /** 
    * The original name (not including the parent folder name).
    * This name does include the trailing zero ('\0' or L'\0'),
    * and can be used as a valid C string!
    *
    * Note: it is stored as an array of multi-bytes characters if i18n is
    * supported.
    */
   vdsChar_T originalName[1];
   
};

typedef struct vdseObjectDescriptor vdseObjectDescriptor;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This structure contains the information needed for all the leaves and
 * the branches of the tree of objects/containers visible from the API.
 */
struct vdseTreeNode
{
   /** Count the number of uncommitted/unrollbacked transaction ops are
    * still to be processed on this object (or more exactly on its data).
    * This is used to make sure that an object is not destroyed while
    * transactions (on its data) are not processed yet.
    */
   size_t txCounter;
  
   /** In units of vdsChar_T, not bytes. */
   size_t myNameLength;
   
   /** Offset to the original string naming this object. */
   ptrdiff_t myNameOffset;

   /** Offset to the string used for the key (lowercase of the original). */
   ptrdiff_t myKeyOffset;
   
   /** Offset to the transaction info (vdseTxStatus). */
   ptrdiff_t txStatusOffset;

   /** Offset to the parent of this object. */
   /* NULL_OFFSET for top folder ("/") */
   ptrdiff_t myParentOffset;
   
};

typedef struct vdseTreeNode vdseTreeNode;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline 
void vdseTreeNodeInit( vdseTreeNode * pNode,
                       ptrdiff_t      txStatusOffset,
                       size_t         originalNameLength,
                       ptrdiff_t      originalNameOffset,
                       ptrdiff_t      parentOffset,
                       ptrdiff_t      keyOffset )
{
   VDS_PRE_CONDITION( pNode != NULL );
   
   pNode->txCounter      = 0;
   pNode->myNameLength   = originalNameLength;
   pNode->myNameOffset   = originalNameOffset;
   pNode->txStatusOffset = txStatusOffset;
   pNode->myParentOffset = parentOffset;
   pNode->myKeyOffset    = keyOffset;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline 
void vdseTreeNodeFini( vdseTreeNode* pNode )
{
   VDS_PRE_CONDITION( pNode != NULL );
   
   pNode->txCounter      = 0;
   pNode->myNameLength   = 0;
   pNode->myNameOffset   = NULL_OFFSET;
   pNode->txStatusOffset = NULL_OFFSET;
   pNode->myParentOffset = NULL_OFFSET;
   pNode->myKeyOffset    = NULL_OFFSET;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSE_TREE_NODE_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

