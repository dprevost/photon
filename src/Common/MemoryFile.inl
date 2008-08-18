/* -*- c -*- */
/* :mode=c:  - For jedit, previous line for emacs */
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

/*!
 *
 * This function is to be used for debugging applications build on VDSF.
 * It disables write access to the shared memory between API calls. The
 * function pscSetReadWrite() is used to enable write access during calls.
 *
 * This could be used to search for a stray pointer corrupting the
 * shared memory while developping applications. This function 
 * requires the C library function mprotect() or equivalent.
 *
 * \param[in] pMem    A pointer to the pscMemoryFile struct itself.
 * \param[in] pError  A pointer to the pscErrorHandler struct (used for 
 *                    handling errors from the OS).
 *
 * \retval true  on success
 * \retval false on error (use pError to retrieve the error)
 *
 * \pre \em pMem cannot be NULL.
 * \pre \em pError cannot be NULL.
 * \invariant \em pMem->initialized must equal ::PSC_MEMFILE_SIGNATURE.
 * \invariant \em pMem->baseAddr cannot be NULL.
 *
 */

bool pscSetReadOnly( pscMemoryFile   * pMem,
                     pscErrorHandler * pError )
{
   int errcode;
   bool ok = true;
#if defined (WIN32)
   unsigned long oldProt;
#endif

   VDS_PRE_CONDITION( pMem   != NULL );
   VDS_INV_CONDITION( pMem->initialized == PSC_MEMFILE_SIGNATURE );
   VDS_INV_CONDITION( pMem->baseAddr != VDS_MAP_FAILED );
   VDS_PRE_CONDITION( pError != NULL );

#if defined (WIN32)
   errcode = VirtualProtect( pMem->baseAddr, 
                             pMem->length, 
                             PAGE_READONLY, 
                             &oldProt );
   if ( errcode == 0 ) {
      pscSetError( pError, PSC_WINERR_HANDLE, GetLastError() );      
      ok = false;
   }
#elif HAVE_MPROTECT
   errcode = mprotect( pMem->baseAddr, pMem->length, PROT_READ );
   if ( errcode != 0 ) {
      pscSetError( pError, PSC_ERRNO_HANDLE, errno );
      ok = false;
   }
#else
   /* autoconf should have spotted this */
   ok = false;
#endif

   return ok;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*!
 *
 * This function is to be used for debugging applications build on VDSF.
 * It enables write access to the shared memory during API calls. The
 * function pscSetReadOnly() is used to block access between calls.
 *
 * This could be used to search for a stray pointer corrupting the
 * shared memory while developping applications. This function 
 * requires the C library function mprotect() or equivalent.
 *
 * \retval 0 on success
 * \retval -1 on error (use pError to retrieve the error)
 *
 * \param[in] pMem    A pointer to the pscMemoryFile struct itself.
 * \param[in] pError  A pointer to the pscErrorHandler struct (used for 
 *                    handling errors from the OS).
 *
 * \pre \em pMem cannot be NULL.
 * \pre \em pError cannot be NULL.
 * \invariant \em pMem->initialized must equal ::PSC_MEMFILE_SIGNATURE.
 * \invariant \em pMem->baseAddr cannot be NULL.
 *
 */

bool pscSetReadWrite( pscMemoryFile   * pMem,
                      pscErrorHandler * pError )
{
   int errcode;
   bool ok = true;
#if defined (WIN32)
   unsigned long oldProt;
#endif

   VDS_PRE_CONDITION( pMem != NULL );
   VDS_INV_CONDITION( pMem->initialized == PSC_MEMFILE_SIGNATURE );
   VDS_INV_CONDITION( pMem->baseAddr != VDS_MAP_FAILED );
   VDS_PRE_CONDITION( pError != NULL );

#if defined (WIN32)
   errcode = VirtualProtect( pMem->baseAddr, 
                             pMem->length, 
                             PAGE_READWRITE, 
                             &oldProt );
   if ( errcode == 0 ) {
      pscSetError( pError, PSC_WINERR_HANDLE, GetLastError() );      
      ok = false;
   }
#elif HAVE_MPROTECT
   errcode = mprotect( pMem->baseAddr, pMem->length, PROT_READ | PROT_WRITE );
   if ( errcode != 0 ) {
      pscSetError( pError, PSC_ERRNO_HANDLE, errno );
      ok = false;
   }
#else
   /* autoconf should have spotted this */
   ok = false;
#endif

   return ok;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

