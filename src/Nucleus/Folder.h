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
#include "Nucleus/MemoryObject.h"
#include "Nucleus/TreeNode.h"
#include "Nucleus/BlockGroup.h"
#include "Nucleus/Hash.h"
#include "Nucleus/HashTx.h"
#include "Nucleus/Definitions.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/*
 * The folder functions are divided in three groups as an attempt to 
 * simplify the organization of the code:
 *  - TopFolder* are functions called by the API where the root is "/"
 *  - APIFolder* are functions called by the API using an api folder object. 
 *  - Folder* are functions used by the other two to actually do the job...
 */

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
   
   /*
    * This field cannot be set or modified by the API. It is set to false
    * by the "constructor". Quasar will set it to true when creating a new
    * shared-memory and constructing its system objects.
    *
    * When this field is set to true, no data can be added or removed 
    * directly by accessing the object through the API. Updates are done 
    * indirectly as a consequence of some API functions.
    */
   bool isSystemObject;

   /** Variable size struct - always put at the end */
   struct psonBlockGroup blockGroup;

};

typedef struct psonFolder psonFolder;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * The following functions are called by the api folder objects and allow
 * this object to create/destroy other objects or to retrieve information
 * on its children.
 */
/**
 * Creates an immediate folder child of the folder.
 */
PHOTON_ENGINE_EXPORT
bool psonAPIFolderCreateFolder( psonFolder         * pFolder,
                                const char         * objectName,
                                uint32_t             nameLengthInBytes,
                                psonSessionContext * pContext );

/**
 * Creates an immediate child of the folder.
 */
PHOTON_ENGINE_EXPORT
bool psonAPIFolderCreateObject( psonFolder          * pFolder,
                                const char          * objectName,
                                uint32_t              nameLengthInBytes,
                                psoObjectDefinition * pDefinition,
                                psonDataDefinition  * pDataDefinition,
                                psonKeyDefinition   * pKeyDefinition,
                                psonSessionContext  * pContext );

/**
 * Destroy an immediate child of the folder.
 */
PHOTON_ENGINE_EXPORT
bool psonAPIFolderDestroyObject( psonFolder         * pFolder,
                                 const char         * objectName,
                                 uint32_t             nameLengthInBytes,
                                 psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonAPIFolderGetDefinition( psonFolder          * pFolder,
                                 const char          * objectName,
                                 uint32_t              strLength,
                                 psoObjectDefinition * pDefinition,
                                 psonDataDefinition ** ppDataDefinition,
                                 psonKeyDefinition  ** ppKeyDefinition,
                                 psonSessionContext  * pContext );

PHOTON_ENGINE_EXPORT
bool psonAPIFolderGetFirst( psonFolder         * pFolder,
                            psonFolderItem     * pItem,
                            psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonAPIFolderGetNext( psonFolder         * pFolder,
                           psonFolderItem     * pItem,
                           psonSessionContext * pContext );

/* Retrieve the status of the current folder */
PHOTON_ENGINE_EXPORT
void psonAPIFolderStatus( psonFolder   * pFolder,
                          psoObjStatus * pStatus );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * if  the object is actually removed, the ptr ppOldMemObj
 * is set so that the transaction does not try to lock the object.
 */
PHOTON_ENGINE_EXPORT
void psonFolderCommitEdit( psonFolder         * pFolder,
                           psonHashTxItem     * pHashItem, 
                           enum psoObjectType   objectType,
                           psonMemObject     ** ppOldMemObj,
                           psonSessionContext * pContext );

/**
 * Delete an object, recursively.
 */ 
PHOTON_ENGINE_EXPORT
bool psonFolderDeleteObject( psonFolder         * pFolder,
                             const char         * objectName,
                             uint32_t             strLength, 
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
bool psonFolderFindObject( psonFolder         * pFolder,
                           const char         * objectName,
                           uint32_t             strLength, 
                           psonHashTxItem    ** ppFoundFolder,
                           psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonFolderGetDefinition( psonFolder          * pFolder,
                              const char          * objectName,
                              uint32_t              strLength,
                              psoObjectDefinition * pDefinition,
                              psonDataDefinition ** ppDataDefinition,
                              psonKeyDefinition  ** ppKeyDefinition,
                              psonSessionContext  * pContext );

PHOTON_ENGINE_EXPORT
bool psonFolderGetDefLength( psonFolder          * pFolder,
                             const char          * objectName,
                             uint32_t              strLength,
                             uint32_t            * pDataDefLength,
                             uint32_t            * pKeyDefLength,
                             psonSessionContext  * pContext );

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
                             psonDataDefinition  * pDataDefinition,
                             psonKeyDefinition   * pKeyDefinition,
                             size_t                numBlocks,
                             size_t                expectedNumOfChilds,
                             psonSessionContext  * pContext );


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
                             psonHashTxItem     * pHashItem,
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
 *
 * The body of these functions is in TopFolder.c instead of Folder.c (Folder.c
 * is getting quite large and has too many unit tests...).
 */

PHOTON_ENGINE_EXPORT
bool psonTopFolderCloseObject( psonFolderItem     * pDescriptor,
                               psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonTopFolderCreateFolder( psonFolder         * pFolder,
                                const char         * objectName,
                                uint32_t             nameLengthInBytes,
                                psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonTopFolderCreateObject( psonFolder          * pFolder,
                                const char          * objectName,
                                uint32_t              nameLengthInBytes,
                                psoObjectDefinition * pDefinition,
                                psonDataDefinition  * pDataDefinition,
                                psonKeyDefinition   * pKeyDefinition,
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
                          psonDataDefinition ** ppDataDefinition,
                          psonKeyDefinition  ** ppKeyDefinition,
                          psonSessionContext  * pContext );

PHOTON_ENGINE_EXPORT
bool psonTopFolderGetDefLength( psonFolder         * pFolder,
                                const char         * objectName,
                                uint32_t             nameLengthInBytes,
                                uint32_t           * pDataDefLength,
                                uint32_t           * pKeyDefLength,
                                psonSessionContext * pContext );

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
