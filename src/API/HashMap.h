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

#ifndef VDSA_HASH_MAP_H
#define VDSA_HASH_MAP_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "API/CommonObject.h"
#include "Engine/HashMap.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct vdsaHashMap
{
   vdsaObjetType type;

   vdsaCommonObject object;
   
   /**
    * This struct holds both the result of the iteration (the pointer to
    * the current vdseHashItem) and the values needed to get the next item.
    */
   vdseHashMapItem iterator;

   /** 
    * The bucket location in the internal array of the hash.
    *
    * Used for iterating in a folder. The hash object is made of an
    * array of buckets - each bucket being a linked list of offsets
    * (ptrdiff_t) to the real data of type vdseHashItem. Therefore
    * we need both pieces of information to iterate through the hash.
    */
//   size_t  bucketIterator;
   
   /** 
    * The location in the list of the bucket (of the hash).
    *
    * Used for iterating in a folder. The hash object is made of an
    * array of buckets - each bucket being a linked list of offsets
    * (ptrdiff_t) to the real data of type vdseHashItem. Therefore
    * we need both pieces of information to iterate through the hash.
    */
//   void * pItemIterator;
   
} vdsaHashMap;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSA_HASH_MAP_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

