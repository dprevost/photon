/* -*- c -*- */
/* :mode=c:  - For jedit, previous line for emacs */
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

/*!
 *
 * This function is to be used for debugging applications build on Photon.
 * It disables write access to the shared memory between API calls. The
 * function psocSetReadWrite() is used to enable write access during calls.
 *
 * This could be used to search for a stray pointer corrupting the
 * shared memory while developping applications. This function 
 * requires the C library function mprotect() or equivalent.
 *
 * \param[in] pMem    A pointer to the psocMemoryFile struct itself.
 * \param[in] pError  A pointer to the psocErrorHandler struct (used for 
 *                    handling errors from the OS).
 *
 * \retval true  on success
 * \retval false on error (use pError to retrieve the error)
 *
 * \pre \em pMem cannot be NULL.
 * \pre \em pError cannot be NULL.
 * \invariant \em pMem->initialized must equal ::PSOC_MEMFILE_SIGNATURE.
 * \invariant \em pMem->baseAddr cannot be NULL.
 *
 */

bool psocSetReadOnly( psocMemoryFile   * pMem,
                      psocErrorHandler * pError )
{
   int errcode;
   bool ok = true;
#if defined (WIN32)
   unsigned long oldProt;
#endif

   PSO_PRE_CONDITION( pMem   != NULL );
   PSO_INV_CONDITION( pMem->initialized == PSOC_MEMFILE_SIGNATURE );
   PSO_INV_CONDITION( pMem->baseAddr != PSO_MAP_FAILED );
   PSO_PRE_CONDITION( pError != NULL );

#if defined (WIN32)
   errcode = VirtualProtect( pMem->baseAddr, 
                             pMem->length, 
                             PAGE_READONLY, 
                             &oldProt );
   if ( errcode == 0 ) {
      psocSetError( pError, PSOC_WINERR_HANDLE, GetLastError() );      
      ok = false;
   }
#elif HAVE_MPROTECT
   errcode = mprotect( pMem->baseAddr, pMem->length, PROT_READ );
   if ( errcode != 0 ) {
      psocSetError( pError, PSOC_ERRNO_HANDLE, errno );
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
 * This function is to be used for debugging applications build on Photon.
 * It enables write access to the shared memory during API calls. The
 * function psocSetReadOnly() is used to block access between calls.
 *
 * This could be used to search for a stray pointer corrupting the
 * shared memory while developping applications. This function 
 * requires the C library function mprotect() or equivalent.
 *
 * \retval 0 on success
 * \retval -1 on error (use pError to retrieve the error)
 *
 * \param[in] pMem    A pointer to the psocMemoryFile struct itself.
 * \param[in] pError  A pointer to the psocErrorHandler struct (used for 
 *                    handling errors from the OS).
 *
 * \pre \em pMem cannot be NULL.
 * \pre \em pError cannot be NULL.
 * \invariant \em pMem->initialized must equal ::PSOC_MEMFILE_SIGNATURE.
 * \invariant \em pMem->baseAddr cannot be NULL.
 *
 */

bool psocSetReadWrite( psocMemoryFile   * pMem,
                       psocErrorHandler * pError )
{
   int errcode;
   bool ok = true;
#if defined (WIN32)
   unsigned long oldProt;
#endif

   PSO_PRE_CONDITION( pMem != NULL );
   PSO_INV_CONDITION( pMem->initialized == PSOC_MEMFILE_SIGNATURE );
   PSO_INV_CONDITION( pMem->baseAddr != PSO_MAP_FAILED );
   PSO_PRE_CONDITION( pError != NULL );

#if defined (WIN32)
   errcode = VirtualProtect( pMem->baseAddr, 
                             pMem->length, 
                             PAGE_READWRITE, 
                             &oldProt );
   if ( errcode == 0 ) {
      psocSetError( pError, PSOC_WINERR_HANDLE, GetLastError() );      
      ok = false;
   }
#elif HAVE_MPROTECT
   errcode = mprotect( pMem->baseAddr, pMem->length, PROT_READ | PROT_WRITE );
   if ( errcode != 0 ) {
      psocSetError( pError, PSOC_ERRNO_HANDLE, errno );
      ok = false;
   }
#else
   /* autoconf should have spotted this */
   ok = false;
#endif

   return ok;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

