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

#ifndef VDSA_FOLDER_H
#define VDSA_FOLDER_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "API/CommonObject.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Forward declarations */
//struct vdseMemoryHeader;
//struct vdseProcess;
//struct vdseObjectContext;
//struct vdsProxyObject;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct vdsaFolder
{
   vdsaObjetType type;

   vdsaCommonObject object;
   
   /** 
    * The bucket location in the internal array of the hash.
    *
    * Used for iterating in a folder. The hash object is made of an
    * array of buckets - each bucket being a linked list of offsets
    * (ptrdiff_t) to the real data of type vdseHashItem. Therefore
    * we need both pieces of information to iterate through the hash.
    */
   size_t  bucketIterator;
   
   /** 
    * The location in the list of the bucket (of the hash).
    *
    * Used for iterating in a folder. The hash object is made of an
    * array of buckets - each bucket being a linked list of offsets
    * (ptrdiff_t) to the real data of type vdseHashItem. Therefore
    * we need both pieces of information to iterate through the hash.
    */
   void * pItemIterator;
   
} vdsaFolder;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSA_FOLDER_H */

