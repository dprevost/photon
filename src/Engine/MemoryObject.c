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

#include "MemoryObject.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Initialize a vdseMemObject struct. 
 *
 *
 * \param[in] pMemObj A pointer to the data struct we are initializing.
 * \param[in] objType The buffer used to store the message.
 * \param[in] objSize The size of the object struct, needed to calculate the
 *                    amount of free space left in the page(s).
 * \param[in] numPages The initial number of pages allocated to this object.
 *
 * \retval VDS_OK  No error found
 * \retval VDS_NOT_ENOUGH_RESOURCES Something went wrong in allocating 
 *                                  resources for the Process Lock.
 *
 * \pre \em pMemObj cannot be NULL.
 * \pre \em objType must be valid (greater than VDSE_IDENT_FIRST and less than 
 *          VDSE_IDENT_LAST).
 * \pre \em objSize must be greater than zero.
 * \pre \em numPages must be greater than zero.
 */

enum vdsErrors 
vdseMemObjectInit( vdseMemObject* pMemObj,
                   enum ObjectIdentifier objType,
                   size_t objSize,
                   size_t numPages )
{
   int errcode = 0;
   
   VDS_PRE_CONDITION( pMemObj != NULL );
   VDS_PRE_CONDITION( objSize > 0 );
   VDS_PRE_CONDITION( numPages > 0 );
   VDS_PRE_CONDITION( objType > VDSE_IDENT_FIRST && 
                      objType < VDSE_IDENT_LAST );

   /* In case InitProcessLock fails */
   pMemObj->objType = VDSE_IDENT_CLEAR;
   
   errcode =  vdscInitProcessLock( &pMemObj->lock );
   if ( errcode != 0 )
      return VDS_NOT_ENOUGH_RESOURCES;
   
   pMemObj->objType = objType;
   pMemObj->accessCounter = 0;
   
   /* Align it on VDSE_MEM_ALIGNMENT bytes boundary */
   objSize = ( (objSize - 1) / VDSE_MEM_ALIGNMENT + 1 )* VDSE_MEM_ALIGNMENT;
   /* Always leave space for the navigator struct */
   pMemObj->remainingBytes = (numPages * PAGESIZE) - objSize - 
                             sizeof(vdsePageNavig);
   pMemObj->navigator.numPagesGroup = numPages;
   pMemObj->navigator.nextGroupOfPages = NULL_OFFSET;
   
   return VDS_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Terminate access to (destroy) a vdseMemObject struct. 
 *
 *
 * \param[in] pMemObj A pointer to the data struct we are initializing.
 *
 * \pre \em pMemObj cannot be NULL.
 * \pre \em objType must be valid (greater than VDSE_IDENT_FIRST and less than 
 *          VDSE_IDENT_LAST).
 */

enum vdsErrors 
vdseMemObjectFini( vdseMemObject* pMemObj )
{
   VDS_PRE_CONDITION( pMemObj != NULL );
   VDS_PRE_CONDITION( pMemObj->objType > VDSE_IDENT_FIRST && 
                      pMemObj->objType < VDSE_IDENT_LAST );

   pMemObj->objType = VDSE_IDENT_CLEAR;

   pMemObj->accessCounter = 0;
   pMemObj->remainingBytes = 0;
   pMemObj->navigator.numPagesGroup = 0;
   pMemObj->navigator.nextGroupOfPages = NULL_OFFSET;

   if ( vdscFiniProcessLock( &pMemObj->lock ) != 0 )
      return VDS_NOT_ENOUGH_RESOURCES;
   return VDS_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
