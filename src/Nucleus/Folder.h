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

#ifndef PSON_FOLDER_H
#define PSON_FOLDER_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Nucleus/Engine.h"
#include "Nucleus/DataType.h"
#include "Nucleus/MemoryObject.h"
#include "Nucleus/TreeNode.h"
#include "Nucleus/BlockGroup.h"
#include "Nucleus/Hash.h"
#include "Nucleus/HashTx.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psonFolderItem
{
   psonHashTxItem * pHashItem;

   ptrdiff_t   itemOffset;

   int status;
   
};

typedef struct psonFolderItem psonFolderItem;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Wrapper for the container holding the information on the tree of
 * shared objects.
 */
struct psonFolder
{
   /** The memory object struct. */
   struct psonMemObject memObject;

   /** Basic info for all leaves and branches of our tree. */
   struct psonTreeNode  nodeObject;

   struct psonHashTx      hashObj;

   /** Variable size struct - always put at the end */
   struct psonBlockGroup blockGroup;

};

typedef struct psonFolder psonFolder;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * if ppOldMemObj is non-NULL and the object is actually removed, the ptr
 * is set so that the transaction does not try to lock the object.
 */
PHOTON_ENGINE_EXPORT
void psonFolderCommitEdit( psonFolder         * pFolder,
                           psonHashTxItem     * pHashItem, 
                           enum psoObjectType   objectType,
                           psonMemObject     ** ppOldMemObj,
                           psonSessionContext * pContext );

/**
 * Creates an immediate child of the folder.
 */
PHOTON_ENGINE_EXPORT
bool psonFolderCreateObject( psonFolder          * pFolder,
                             const char          * objectName,
                             uint32_t              nameLengthInBytes,
                             psoObjectDefinition * pDefinition,
                             psoKeyDefinition    * pKey,
                             psoFieldDefinition  * pFields,
                             psonSessionContext  * pContext );

/**
 * Delete an object, recursively.
 */ 
PHOTON_ENGINE_EXPORT
bool psonFolderDeleteObject( psonFolder         * pFolder,
                             const char         * objectName,
                             uint32_t             strLength, 
                             psonSessionContext * pContext );

/**
 * Destroy an immediate child of the folder.
 */
PHOTON_ENGINE_EXPORT
bool psonFolderDestroyObject( psonFolder         * pFolder,
                              const char         * objectName,
                              uint32_t             nameLengthInBytes,
                              psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonFolderEditObject( psonFolder         * pFolder,
                           const char         * objectName,
                           uint32_t             strLength, 
                           enum psoObjectType   objectType, 
                           psonFolderItem     * pFolderItem,
                           psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
void psonFolderFini( psonFolder         * pFolder,
                     psonSessionContext * pContext );


PHOTON_ENGINE_EXPORT
bool psonFolderGetDefinition( psonFolder          * pFolder,
                              const char          * objectName,
                              uint32_t              strLength,
                              psoObjectDefinition * pDefinition,
                              psonFieldDef       ** ppInternalDef,
                              psonSessionContext  * pContext );

PHOTON_ENGINE_EXPORT
bool psonFolderGetFirst( psonFolder         * pFolder,
                         psonFolderItem     * pItem,
                         psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonFolderGetNext( psonFolder         * pFolder,
                        psonFolderItem     * pItem,
                        psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonFolderGetObject( psonFolder         * pFolder,
                          const char         * objectName,
                          uint32_t             strLength, 
                          enum psoObjectType   objectType, 
                          psonFolderItem     * pFolderItem,
                          psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonFolderGetStatus( psonFolder         * pFolder,
                          const char         * objectName,
                          uint32_t             strLength, 
                          psoObjStatus       * pStatus,
                          psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonFolderInit( psonFolder         * pFolder,
                     ptrdiff_t            parentOffset,
                     size_t               numberOfBlocks,
                     size_t               expectedNumOfChilds,
                     psonTxStatus       * pTxStatus,
                     uint32_t             origNameLength,
                     char               * origName,
                     ptrdiff_t            hashItemOffset,
                     psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonFolderInsertObject( psonFolder          * pFolder,
                             const char          * objectName,
                             const char          * originalName,
                             uint32_t              strLength,
                             psoObjectDefinition * pDefinition,
                             psoKeyDefinition    * pKey,
                             psoFieldDefinition  * pFields,
                             size_t                numBlocks,
                             size_t                expectedNumOfChilds,
                             psonSessionContext  * pContext );

/* Retrieve the status of the current folder */
PHOTON_ENGINE_EXPORT
void psonFolderMyStatus( psonFolder   * pFolder,
                         psoObjStatus * pStatus );

PHOTON_ENGINE_EXPORT
bool psonFolderRelease( psonFolder         * pFolder,
                        psonFolderItem     * pItem,
                        psonSessionContext * pContext );

/** 
 * This function does the actual removal of the entry in the list of the
 * folder object (once there are no pending transactions, no sessions
 * have this object open, etc...).
 */
PHOTON_ENGINE_EXPORT
void psonFolderRemoveObject( psonFolder         * pFolder,
                             psonHashTxItem       * pHashItem,
                             psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
void psonFolderResize( psonFolder         * pFolder, 
                       psonSessionContext * pContext  );


PHOTON_ENGINE_EXPORT
void psonFolderRollbackEdit( psonFolder         * pFolder,
                             psonHashTxItem     * pHashItem, 
                             enum psoObjectType   objectType,
                             bool               * isRemoved,
                             psonSessionContext * pContext );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * The next 7 functions should only be used by the API, to create, destroy,
 * open or close a memory object. Or to obtain its status or definition.
 */

PHOTON_ENGINE_EXPORT
bool psonTopFolderCloseObject( psonFolderItem     * pDescriptor,
                               psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonTopFolderCreateObject( psonFolder          * pFolder,
                                const char          * objectName,
                                uint32_t              nameLengthInBytes,
                                psoObjectDefinition * pDefinition,
                                psoKeyDefinition    * pKey,
                                psoFieldDefinition  * pFields,
                                psonSessionContext  * pContext );

PHOTON_ENGINE_EXPORT
bool psonTopFolderDestroyObject( psonFolder         * pFolder,
                                 const char         * objectName,
                                 uint32_t             nameLengthInBytes,
                                 psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonTopFolderEditObject( psonFolder         * pFolder,
                              const char         * objectName,
                              uint32_t             nameLengthInBytes,
                              enum psoObjectType   objectType, 
                              psonFolderItem     * pFolderItem,
                              psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonTopFolderGetDef( psonFolder          * pFolder,
                          const char          * objectName,
                          uint32_t              nameLengthInBytes,
                          psoObjectDefinition * pDefinition,
                          psonFieldDef       ** ppInternalDef,
                          psonSessionContext  * pContext );

PHOTON_ENGINE_EXPORT
bool psonTopFolderGetStatus( psonFolder         * pFolder,
                             const char         * objectName,
                             uint32_t             nameLengthInBytes,
                             psoObjStatus       * pFolderItem,
                             psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonTopFolderOpenObject( psonFolder         * pFolder,
                              const char         * objectName,
                              uint32_t             nameLengthInBytes,
                              enum psoObjectType   objectType, 
                              psonFolderItem     * pFolderItem,
                              psonSessionContext * pContext );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSON_FOLDER_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
