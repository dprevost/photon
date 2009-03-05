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

#ifndef QSR_VERIFY_COMMON_H
#define QSR_VERIFY_COMMON_H

#include "Nucleus/MemoryHeader.h"
#include "Nucleus/MemBitmap.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

extern psonMemoryHeader * g_pMemoryAddress;
extern bool g_bTestAllocator;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psonLinkedList;
struct psonFolder;
struct psonHashMap;
struct psonHash;
struct psonHashTx;
struct psonFastMap;
struct psonQueue;
struct psonMemObject;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum qsrRecoverError 
{
   QSR_REC_OK = 0,
   QSR_REC_CHANGES,
   QSR_REC_START_ERRORS = 100,
   /* Object added but not committed or object deleted and committed */
   QSR_REC_DELETED_OBJECT,
   QSR_REC_UNHANDLED_ERROR,
   QSR_REC_UNRECOVERABLE_ERROR
};
typedef enum qsrRecoverError qsrRecoverError;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct qsrVerifyStruct
{
   bool   verbose;
   int    spaces;
   FILE * fp;
   bool   doRepair;
   size_t numObjectsOK;
   size_t numObjectsRepaired;
   size_t numObjectsDeleted;
   size_t numObjectsError;
   psonMemBitmap * pBitmap;
};
typedef struct qsrVerifyStruct qsrVerifyStruct;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void qsrEcho( qsrVerifyStruct * pVerify, char * message )
{
   int i;
   
   if ( pVerify->verbose ) {
      for ( i = 0; i < pVerify->spaces; ++i ) {
         fprintf( pVerify->fp, " " );
      }
      fprintf( pVerify->fp, "%s\n", message );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void qsrResetBitmap( psonMemBitmap * pBitmap )
{
   size_t length;
   
   length = ( (pBitmap->lengthInBits - 1 ) >> 3 ) + 1;
   memset( pBitmap->bitmap, -1, length );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
bool qsrVerifyOffset( qsrVerifyStruct * pVerify, ptrdiff_t offset )
{
   return psonIsBufferFree( pVerify->pBitmap, offset );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void 
qsrPopulateBitmap( struct qsrVerifyStruct   * pVerify,
                    struct psonMemObject      * pMemObj,
                    struct psonSessionContext * pContext );

enum qsrRecoverError
qsrVerifyFastMap( qsrVerifyStruct   * pVerify,
                   struct psonFastMap     * pHashMap, 
                   psonSessionContext * pContext  );

enum qsrRecoverError
qsrVerifyFolder( qsrVerifyStruct   * pVerify,
                  struct psonFolder  * pFolder,
                  psonSessionContext * pContext );

enum qsrRecoverError
qsrVerifyHash( qsrVerifyStruct * pVerify,
                struct psonHash  * pHash,
                ptrdiff_t          memObjOffset );

enum qsrRecoverError
qsrVerifyHashTx( qsrVerifyStruct  * pVerify,
                  struct psonHashTx * pHash,
                  ptrdiff_t           memObjOffset );

enum qsrRecoverError
qsrVerifyList( qsrVerifyStruct      * pVerify,
                struct psonLinkedList * pList );

enum qsrRecoverError
qsrVerifyHashMap( qsrVerifyStruct   * pVerify,
                   struct psonHashMap * pHashMap, 
                   psonSessionContext * pContext  );

enum qsrRecoverError
qsrVerifyMemObject( struct qsrVerifyStruct   * pVerify,
                     struct psonMemObject      * pMemObj,
                     struct psonSessionContext * pContext );

enum qsrRecoverError
qsrVerifyQueue( qsrVerifyStruct   * pVerify,
                 struct psonQueue   * pQueue, 
                 psonSessionContext * pContext  );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* QSR_VERIFY_COMMON_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

