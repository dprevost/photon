/*
 * Copyright (C) 2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef VDSE_HASH_MAP_H
#define VDSE_HASH_MAP_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Engine.h"
#include "MemoryObject.h"
#include "TreeNode.h"
#include "BlockGroup.h"
#include "Hash.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct vdseHashMap
{
   /** Always first */
   struct vdseMemObject memObject;

   /** Basic info for all leaves and branches of our tree. */
   struct vdseTreeNode  nodeObject;

   struct vdseHash      hashObj;

   /** Variable size struct - always put at the end */
   struct vdseBlockGroup blockGroup;

} vdseHashMap;
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

VDSF_ENGINE_EXPORT
int vdseHashMapInit( vdseHashMap        * pHashMap,
                     ptrdiff_t            parentOffset,
                     size_t               numberOfBlocks,
                     size_t               expectedNumOfChilds,
                     vdseTxStatus       * pTxStatus,
                     size_t               origNameLength,
                     vdsChar_T          * origName,
                     ptrdiff_t            keyOffset,
                     vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
void vdseHashMapFini( vdseHashMap        * pHashMap,
                      vdseSessionContext * pContext );

VDSF_ENGINE_EXPORTVDS_NO_SUCH_ITEM
int vdseHashMapGetItem( vdseHashMap        * pHashMap,
                        const void         * pKey,
                        size_t               keyLength, 
                        vdseHashItem      ** ppItem,
                        vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
int vdseHashMapInsertItem( vdseHashMap        * pHashMap,
                           const void         * pKey,
                           size_t               keyLength,
                           const void         * pItem,
                           size_t               itemLength,
                           vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
int vdseHashMapDeleteItem( vdseHashMap        * pHashMap,
                           const void         * key,
                           size_t               keyLength, 
                           vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
void vdseHashMapCommitAdd( vdseHashMap * pHashMap, 
                           ptrdiff_t     itemOffset );

VDSF_ENGINE_EXPORT
void vdseHashMapRollbackAdd( vdseHashMap        * pHashMap, 
                             ptrdiff_t            itemOffset,
                             vdseSessionContext * pContext  );

VDSF_ENGINE_EXPORT
void vdseHashMapCommitRemove( vdseHashMap        * pHashMap, 
                              ptrdiff_t            itemOffset,
                              vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
void vdseHashMapRollbackRemove( vdseHashMap * pHashMap, 
                                ptrdiff_t     itemOffset );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSE_HASH_MAP_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

