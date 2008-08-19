/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef PSN_MEM_BITMAP_H
#define PSN_MEM_BITMAP_H

#include "Nucleus/Engine.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psnMemBitmap
{
   size_t lengthInBits;
   
   /* */
   size_t allocGranularity;

   ptrdiff_t baseAddressOffset;
   
   unsigned char bitmap[1];
   
};

typedef struct psnMemBitmap psnMemBitmap;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Initialize the psnMemBitmap struct. 
 */
VDSF_ENGINE_EXPORT
void psnMemBitmapInit( psnMemBitmap * pBitmap,
                        ptrdiff_t       offset,
                        size_t          totalLength,
                        size_t          allocationUnit );

VDSF_ENGINE_EXPORT
void psnMemBitmapFini( psnMemBitmap * pBitmap );

static inline
bool psnIsBufferFree( psnMemBitmap * pBitmap,
                       ptrdiff_t       offset );

static inline
void psnSetBufferAllocated( psnMemBitmap * pBitmap,
                             ptrdiff_t       offset,
                             size_t          length );

static inline
void psnSetBufferFree( psnMemBitmap * pBitmap,
                        ptrdiff_t       offset,
                        size_t          length );

static inline
size_t psnGetBitmapLengthBytes( size_t length, size_t allocationUnit );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "MemBitmap.inl"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSN_MEM_BITMAP_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

