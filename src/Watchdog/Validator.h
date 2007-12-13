/* -*- c++ -*- */
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

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#ifndef VDSW_VALIDATOR_H
#define VDSW_VALIDATOR_H

#include "Engine/MemoryHeader.h"

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

// Forward declaration
struct vdseFolder;
struct vdseHashMap;
struct vdseQueue;
struct vdseLinkedList;
struct vdseMemoryHeader;
struct vdseMemObject;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

/**
 *  This class validates an existing VDS. 
 */

class vdsValidator
{
public:
   
   vdsValidator( vdseMemoryHeader * pMemoryAddress );
   
   ~vdsValidator();
   
   int Validate();
   
private:

   void echo( char * message );
   
   bool ValidateOffset( ptrdiff_t offset )
   {
      return (offset > 0 && offset < (ptrdiff_t)m_pMemoryAddress->totalLength );
   }
   
   int ValidateFolder( vdseFolder * pFolder );

   int ValidateHashMap( vdseHashMap * pHashMap );

   int ValidateQueue( vdseQueue * pQueue );
   
   int ValidateList( vdseLinkedList * pList );

   int ValidateMemObject( vdseMemObject * pMemObject );

   // Set but not used yet (if true, the mem allocator is considered suspect
   // and the allocator is rebuild from scratch).
   bool m_bTestAllocator;

   vdseMemoryHeader * m_pMemoryAddress;

};


// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#endif /* VDSW_VALIDATOR_H */
