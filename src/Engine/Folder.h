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

#ifndef VDSE_FOLDER_H
#define VDSE_FOLDER_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Engine.h"
#include "MemoryObject.h"
#include "TreeNode.h"
#include "PageGroup.h"
#include "Hash.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Wrapper for the container holding the information on the tree of
 * shared objects.
 */	

typedef struct vdseFolder
{
   /** Always first */
   struct vdseMemObject memObject;

   /** Basic info for all leaves and branches of our tree. */
   struct vdseTreeNode  nodeObject;

   struct vdseHash      hashObj;

   /** Variable size struct - always put at the end */
   struct vdsePageGroup pageGroup;

} vdseFolder;
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

VDSF_ENGINE_EXPORT
int vdseFolderInit( vdseFolder*         pFolder,
                    ptrdiff_t           parentOffset,
                    size_t              numberOfPages,
                    size_t              expectedNumOfChilds,
                    vdseTxStatus*       pTxStatus,
                    size_t              origNameLength,
                    vdsChar_T*          origName,
                    vdseSessionContext* pContext );

VDSF_ENGINE_EXPORT
void vdseFolderFini( vdseFolder*         pFolder,
                     vdseSessionContext* pContext );


   
VDSF_ENGINE_EXPORT
int vdseFolderGetObject( vdseFolder*            pFolder,
                         const vdsChar_T*       objectName,
                         size_t                 strLength, 
                         vdseObjectDescriptor** ppDescriptor,
                         vdseSessionContext*    pContext );

VDSF_ENGINE_EXPORT
int vdseFolderInsertObject( vdseFolder*         pFolder,
                            const vdsChar_T*    objectName,
                            const vdsChar_T*    originalName,
                            size_t              strLength, 
                            enum vdsObjectType  objectType,
                            size_t              numPages,
                            size_t              expectedNumOfChilds,
                            vdseSessionContext* pContext );

VDSF_ENGINE_EXPORT
int vdseFolderDeleteObject( vdseFolder*         pFolder,
                            const vdsChar_T*    objectName,
                            size_t              strLength, 
                            vdseSessionContext* pContext );
                            
VDSF_ENGINE_EXPORT
bool vdseFolderDeletable( vdseFolder*         pFolder,
                          vdseSessionContext* pContext );

VDSF_ENGINE_EXPORT
void vdseFolderRemoveObject( vdseFolder*         pFolder,
                             vdseMemObject*      pMemObj,
                             const vdsChar_T*    objectName,
                             size_t              nameLength,
                             vdseSessionContext* pContext );

#if 0

/** Can be used by both rollback-create and commit-destroy */
VDSF_ENGINE_EXPORT
vdsErrors vdseFolderRollbackCreate( vdseFolder*         pFolder,
                          ptrdiff_t       childOffset,
                          enum vdsObjectType type,
                          vdseSessionContext* pContext );
   
VDSF_ENGINE_EXPORT
vdsErrors vdseFolderRollbackDestroy( vdseFolder*         pFolder,
                          ptrdiff_t       childOffset,
                           vdseSessionContext* pContext );

   /** This function does the actual removal of the entry in the list of the
    *  folder object (once there are no pending transactions, no sessions
    *  have this object open, etc...).
    */
VDSF_ENGINE_EXPORT
   vdsErrors vdseFolderRemoveObject( vdseFolder*         pFolder,
                          BaseNode*     pNode,
                           enum vdsObjectType type,
                           vdseSessionContext* pContext );

VDSF_ENGINE_EXPORT
   bool vdseFolderSelfTest( vdseFolder*         pFolder,
                          vdseMemAlloc* pAlloc );
   
/*     void Recover( char**     pArrayBuffers, */
/*                   size_t*    pNumOfAllocatedBuffer, */
/*                   vds_lock_T lockValue ); */
   
#endif
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSE_FOLDER_H */
