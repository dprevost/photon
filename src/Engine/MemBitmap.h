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

#ifndef VDSE_MEM_BITMAP_H
#define VDSE_MEM_BITMAP_H

#include "Engine.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct vdseMemBitmap
{
   size_t lengthInBits;
   
   /* */
   size_t allocGranularity;

   ptrdiff_t baseAddressOffset;
   
   unsigned char bitmap[1];
   
} vdseMemBitmap;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Initialize the vdseMemBitmap struct. 
 */
VDSF_ENGINE_EXPORT
void vdseMemBitmapInit( vdseMemBitmap * pBitmap,
                        ptrdiff_t       offset,
                        size_t          totalLength,
                        size_t          allocationUnit );

VDSF_ENGINE_EXPORT
void vdseMemBitmapFini( vdseMemBitmap * pBitmap );

static inline
bool vdseIsBufferFree( vdseMemBitmap * pBitmap,
                       ptrdiff_t       offset );

static inline
void vdseSetBufferAllocated( vdseMemBitmap * pBitmap,
                             ptrdiff_t       offset,
                             size_t          length );

static inline
void vdseSetBufferFree( vdseMemBitmap * pBitmap,
                        ptrdiff_t       offset,
                        size_t          length );

static inline
size_t vdseGetBitmapLengthBytes( size_t length, size_t allocationUnit );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "MemBitmap.inl"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSE_MEM_BITMAP_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

