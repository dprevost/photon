/*
 * Copyright (C) 2006-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#ifndef PSON_MEM_BITMAP_H
#define PSON_MEM_BITMAP_H

#include "Nucleus/Engine.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psonMemBitmap
{
   size_t lengthInBits;
   
   /* */
   size_t allocGranularity;

   ptrdiff_t baseAddressOffset;
   
   unsigned char bitmap[1];
   
};

typedef struct psonMemBitmap psonMemBitmap;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Initialize the psonMemBitmap struct. 
 */
PHOTON_ENGINE_EXPORT
void psonMemBitmapInit( psonMemBitmap * pBitmap,
                        ptrdiff_t       offset,
                        size_t          totalLength,
                        size_t          allocationUnit );

PHOTON_ENGINE_EXPORT
void psonMemBitmapFini( psonMemBitmap * pBitmap );

static inline
bool psonIsBufferFree( psonMemBitmap * pBitmap,
                       ptrdiff_t       offset );

static inline
void psonSetBufferAllocated( psonMemBitmap * pBitmap,
                             ptrdiff_t       offset,
                             size_t          length );

static inline
void psonSetBufferFree( psonMemBitmap * pBitmap,
                        ptrdiff_t       offset,
                        size_t          length );

static inline
size_t psonGetBitmapLengthBytes( size_t length, size_t allocationUnit );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "MemBitmap.inl"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSON_MEM_BITMAP_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

