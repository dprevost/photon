/* -*- c -*- */
/* :mode=c:  - For jedit, previous line for emacs */
/*
 * Copyright (C) 2006 Daniel Prevost <dprevost@users.sourceforge.net>
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

#include "MemBitmap.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline
bool vdseIsBufferFree( vdseMemBitmap* pBitmap,
                       ptrdiff_t      offset )
{
   size_t byte, bit;
   size_t inUnitsOfAllocation;
   ptrdiff_t localOffset;
   
   VDS_PRE_CONDITION( pBitmap != NULL );
   VDS_PRE_CONDITION( offset  != NULL_OFFSET );

   localOffset = offset - pBitmap->baseAddressOffset;
   if ( localOffset < 0 )
      return false;

   inUnitsOfAllocation = localOffset / pBitmap->allocGranularity;
   if ( inUnitsOfAllocation >= pBitmap->lengthInBits )
      return false;

   byte = inUnitsOfAllocation >> 3; /* Equivalent to divide by  8  */

   /*
    * We use the highest bit for the lower page so that the bitmap
    * is "ordered".
    */
   bit = 7 - (inUnitsOfAllocation & 7);

   return ( (pBitmap->bitmap[byte] & (unsigned char)(1 << bit)) == 0 );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline
void vdseSetBufferAllocated( vdseMemBitmap* pBitmap,
                             ptrdiff_t      offset,
                             size_t         length )
{
   size_t byte, bit, i;
   size_t inUnitsOfAllocation;
   ptrdiff_t localOffset;
   
   VDS_PRE_CONDITION( pBitmap != NULL );
   VDS_PRE_CONDITION( offset  != NULL_OFFSET );
   VDS_PRE_CONDITION( length > 0 );
   
   localOffset = offset - pBitmap->baseAddressOffset;

   inUnitsOfAllocation = localOffset / pBitmap->allocGranularity;
   length = length / pBitmap->allocGranularity;
   
   byte = inUnitsOfAllocation >> 3; /* Equivalent to divide by  8  */
   /*
    * We use the highest bit for the lower page so that the bitmap
    * is "ordered".
    */
   bit = 7 - (inUnitsOfAllocation & 7);

   for ( i = 0; i < length; ++i )
   {
      /* Setting the bit to one */
      pBitmap->bitmap[byte] |= (unsigned char)(1 << bit);
      if ( bit == 0 )
      {
         bit = 8;
         byte++;
      }
      bit--;
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline
void vdseSetBufferFree( vdseMemBitmap* pBitmap,
                        ptrdiff_t      offset,
                        size_t         length )
{
   size_t byte, bit, i;
   size_t inUnitsOfAllocation;
   ptrdiff_t localOffset;

   VDS_PRE_CONDITION( pBitmap != NULL );
   VDS_PRE_CONDITION( offset  != NULL_OFFSET );
   VDS_PRE_CONDITION( length > 0 );

   localOffset = offset - pBitmap->baseAddressOffset;
   
   inUnitsOfAllocation = localOffset / pBitmap->allocGranularity;
   length = length / pBitmap->allocGranularity;
   
   byte = inUnitsOfAllocation >> 3; /* Equivalent to divide by  8  */
   /*
    * We use the highest bit for the lower page so that the bitmap
    * is "ordered".
    */
   bit = 7 - (inUnitsOfAllocation & 7);
   
   for ( i = 0; i < length; ++i )
   {
      /* Setting the bit to zero */
      pBitmap->bitmap[byte] &= (unsigned char)(~(1 << bit));
      if ( bit == 0 )
      {
         bit = 8;
         byte++;
      }
      bit--;
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline
size_t vdseGetBitmapLengthBytes( size_t length, size_t allocationUnit )
{
   /* Testing that it is non-zero and a power of two */
   VDS_PRE_CONDITION( allocationUnit > 0  && 
                      ! (allocationUnit & (allocationUnit-1)) );
   VDS_PRE_CONDITION( length > 0 );
   
   /* We "align" it to a multiple of allocationUnit */
   length = ((length - 1) / allocationUnit + 1 ) * allocationUnit;
//   fprintf( stderr, "l = %u %u\n", length, ( (length/allocationUnit - 1) >> 3 ) + 1 );
   return ( (length/allocationUnit - 1) >> 3 ) + 1;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

