/* -*- c -*- */
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

/*!
 *
 * This function is to be used for debugging applications build on VDSF.
 * It disables write access to the shared memory between API calls. The
 * function vdscSetReadWrite() is used to enable write access during calls.
 *
 * This could be used to search for a stray pointer corrupting the
 * shared memory while developping applications. This function 
 * requires the C library function mprotect() or equivalent.
 *
 * \param[in] pMem    A pointer to the vdscMemoryFile struct itself.
 * \param[in] pError  A pointer to the vdscErrorHandler struct (used for 
 *                    handling errors from the OS).
 *
 * \retval 0 on success
 * \retval -1 on error (use pError to retrieve the error)
 *
 * \pre \em pMem cannot be NULL.
 * \pre \em pError cannot be NULL.
 * \invariant \em pMem->initialized must equal ::VDSC_MEMFILE_SIGNATURE.
 * \invariant \em pMem->baseAddr cannot be NULL.
 *
 */

int
vdscSetReadOnly( vdscMemoryFile*   pMem,
                 vdscErrorHandler* pError )
{
   int errcode;
#if defined (WIN32)
   int err;
   unsigned long oldProt;
#endif

   VDS_PRE_CONDITION( pMem   != NULL );
   VDS_INV_CONDITION( pMem->initialized == VDSC_MEMFILE_SIGNATURE );
   VDS_INV_CONDITION( pMem->baseAddr != VDS_MAP_FAILED );
   VDS_PRE_CONDITION( pError != NULL );

#if defined (WIN32)

   err = VirtualProtect( pMem->baseAddr, 
                         pMem->length, 
                         PAGE_READONLY, 
                         &oldProt );
   if ( err == 0 )
   {
      vdscSetError( pError, VDSC_WINERR_HANDLE, GetLastError() );      
      errcode = -1;
   }
   else
      errcode = 0;

#elif HAVE_MPROTECT

   errcode = mprotect( pMem->baseAddr, pMem->length, PROT_READ );
   if ( errcode != 0 )
      vdscSetError( pError, VDSC_ERRNO_HANDLE, errno );      

#else

   errcode = -1;

#endif

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*!
 *
 * This function is to be used for debugging applications build on VDSF.
 * It enables write access to the shared memory during API calls. The
 * function vdscSetReadOnly() is used to block access between calls.
 *
 * This could be used to search for a stray pointer corrupting the
 * shared memory while developping applications. This function 
 * requires the C library function mprotect() or equivalent.
 *
 * \retval 0 on success
 * \retval -1 on error (use pError to retrieve the error)
 *
 * \param[in] pMem    A pointer to the vdscMemoryFile struct itself.
 * \param[in] pError  A pointer to the vdscErrorHandler struct (used for 
 *                    handling errors from the OS).
 *
 * \pre \em pMem cannot be NULL.
 * \pre \em pError cannot be NULL.
 * \invariant \em pMem->initialized must equal ::VDSC_MEMFILE_SIGNATURE.
 * \invariant \em pMem->baseAddr cannot be NULL.
 *
 */

int
vdscSetReadWrite( vdscMemoryFile   * pMem,
                  vdscErrorHandler * pError )
{
   int errcode;
#if defined (WIN32)
   int err;
   unsigned long oldProt;
#endif

   VDS_PRE_CONDITION( pMem != NULL );
   VDS_INV_CONDITION( pMem->initialized == VDSC_MEMFILE_SIGNATURE );
   VDS_INV_CONDITION( pMem->baseAddr != VDS_MAP_FAILED );
   VDS_PRE_CONDITION( pError != NULL );

#if defined (WIN32)

   err = VirtualProtect( pMem->baseAddr, 
                         pMem->length, 
                         PAGE_READWRITE, 
                         &oldProt );
   if ( err == 0 )
   {
      vdscSetError( pError, VDSC_WINERR_HANDLE, GetLastError() );      
      errcode = -1;
   }
   else
      errcode = 0;

#elif HAVE_MPROTECT

   errcode = mprotect( pMem->baseAddr, pMem->length, PROT_READ | PROT_WRITE );
   if ( errcode != 0 )
      vdscSetError( pError, VDSC_ERRNO_HANDLE, errno );      

#else

   errcode = -1;

#endif

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
