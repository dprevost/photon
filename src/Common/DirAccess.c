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

#include "DirAccess.h"
#include "Common.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*!
 * 
 * \param[in] pIterator  A pointer to the psocDirIterator struct itself.
 * \param[in] pError     A pointer to a psocErrorHandler struct (for handling
 *                       C library or Win32 API errors and more).
 *
 * \return The function returns a pointer to the next file name on success 
 *         or NULL if either there are no more files or an error was 
 *         encountered (on error, the error field of pError will be non-zero).
 *
 * \pre \em pIterator cannot be NULL.
 * \pre \em pError cannot be NULL.
 * \pre (Posix specific) \em pIterator->pDir must be NULL (which insures
 *                           that this function was not called twice without
 *                           calling psocCloseDir()).
 *
 * \invariant \em pIterator->initialized must equal 
 *                ::PSOC_DIR_ACCESS_SIGNATURE.
 *
 * \post The return value cannot be NULL if no error was encountered.
 *
 */
const char * psocDirGetNextFileName( psocDirIterator  * pIterator,
                                     psocErrorHandler * pError )
{
#if ! defined (WIN32)
   struct dirent * pEntry;
#endif

   PSO_PRE_CONDITION( pIterator != NULL );
   PSO_INV_CONDITION( pIterator->initialized == PSOC_DIR_ACCESS_SIGNATURE );
   PSO_PRE_CONDITION( pError    != NULL );

#if defined (WIN32)
   PSO_PRE_CONDITION( pIterator->dirName[0] != '\0' );
#else
   PSO_PRE_CONDITION( pIterator->pDir != NULL );
#endif

#if defined (WIN32)
   if ( pIterator->handle == PSO_INVALID_HANDLE ) {
      pIterator->handle = 
         FindFirstFile( pIterator->dirName, &pIterator->data );
   }
   if ( pIterator->handle == PSO_INVALID_HANDLE ) {
      psocSetError( pError, PSOC_WINERR_HANDLE, GetLastError() );
      return NULL;
   }
   
   for ( ;; ) {

      int err = FindNextFile( pIterator->handle, &pIterator->data );
      if ( err == 0 ) {
         if ( GetLastError() != ERROR_NO_MORE_FILES ) {
            psocSetError( pError, PSOC_WINERR_HANDLE, GetLastError() );
         }
         return NULL;
      }
      
      if ( strlen( pIterator->data.cFileName ) == 1 && 
           pIterator->data.cFileName[0] == '.' ) {
         continue;
      }
      if ( strlen( pIterator->data.cFileName ) == 2 && 
           pIterator->data.cFileName[0] == '.' && 
           pIterator->data.cFileName[1] == '.' ) {
         continue;
      }
      PSO_POST_CONDITION( pIterator->data.cFileName  != NULL );

      return pIterator->data.cFileName;
   }

#else
   for ( ;; ) {
      errno = 0; /* To be safe */
      pEntry = readdir( pIterator->pDir );
      if ( pEntry == NULL ) {
         if ( errno != 0 ) {
            psocSetError( pError, PSOC_ERRNO_HANDLE, errno );
         }
         break;
      }
      
      if ( strlen( pEntry->d_name ) == 1 && pEntry->d_name[0] == '.' ) {
         continue;
      }
      if ( strlen( pEntry->d_name ) == 2 && 
           pEntry->d_name[0] == '.' && pEntry->d_name[1] == '.' ) {
         continue;
      }
      
      PSO_POST_CONDITION( pEntry->d_name != NULL );

      return pEntry->d_name;
   }

   return NULL;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*!
 * 
 * \param[in] pIterator  A pointer to the psocDirIterator struct itself.
 *
 * \pre \em pIterator cannot be NULL.
 * \invariant \em pIterator->initialized must equal 
 *                ::PSOC_DIR_ACCESS_SIGNATURE.
 *
 */
void psocFiniDir( psocDirIterator * pIterator )
{
   PSO_PRE_CONDITION( pIterator != NULL );
   PSO_INV_CONDITION( pIterator->initialized == PSOC_DIR_ACCESS_SIGNATURE );

#if defined ( WIN32 )
   pIterator->handle = PSO_INVALID_HANDLE;
   memset( pIterator->dirName, 0, PATH_MAX);
#else
   pIterator->pDir = NULL;
#endif

   pIterator->initialized = 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*!
 * 
 * \param[in] pIterator  A pointer to the psocDirIterator struct itself.
 *
 * \pre \em pIterator cannot be NULL.
 *
 */
void psocInitDir( psocDirIterator * pIterator )
{
   PSO_PRE_CONDITION( pIterator != NULL );

#if defined ( WIN32 )
   pIterator->handle = PSO_INVALID_HANDLE;
   memset( pIterator->dirName, 0, PATH_MAX);
#else
   pIterator->pDir = NULL;
#endif

   pIterator->initialized = PSOC_DIR_ACCESS_SIGNATURE;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*!
 * 
 * \param[in] pIterator  A pointer to the psocDirIterator struct itself.
 *
 * \pre \em pIterator cannot be NULL.
 * \invariant \em pIterator->initialized must equal 
 *                ::PSOC_DIR_ACCESS_SIGNATURE.
 *
 */
void psocCloseDir( psocDirIterator * pIterator )
{
   PSO_PRE_CONDITION( pIterator != NULL );
   PSO_INV_CONDITION( pIterator->initialized == PSOC_DIR_ACCESS_SIGNATURE );

#if defined (WIN32)
   if ( pIterator->handle != PSO_INVALID_HANDLE ) {
      FindClose( pIterator->handle );
   }
   pIterator->handle = PSO_INVALID_HANDLE;
   memset( pIterator->dirName, 0, PATH_MAX);
#else
   if ( pIterator->pDir != NULL ) {
      closedir (pIterator->pDir);
   }
   pIterator->pDir = NULL;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*!
 * 
 * \param[in] pIterator  A pointer to the psocDirIterator struct itself.
 * \param[in] dirName    The directory name.
 * \param[in] pError     A pointer to a psocErrorHandler struct (for handling
 *                       C library or Win32 API errors).
 *
 * \retval true on success
 * \retval false on error (use pError to retrieve the error)
 *
 * \pre \em pIterator cannot be NULL.
 * \pre \em dirName cannot be NULL.
 * \pre \em pError cannot be NULL.
 * \pre (Win32 specific) \em pIterator->dirName must be empty (which insures
 *                           that this function was not called twice without
 *                           calling psocCloseDir()).
 * \pre (Posix specific) \em pIterator->pDir must be NULL (which insures
 *                           that this function was not called twice without
 *                           calling psocCloseDir()).
 *
 * \invariant \em pIterator->initialized must equal 
 *                ::PSOC_DIR_ACCESS_SIGNATURE.
 *
 */
bool psocOpenDir( psocDirIterator  * pIterator, 
                  const char       * dirName,
                  psocErrorHandler * pError )
{
#if defined (WIN32)
   int i = 0;
#endif

   PSO_PRE_CONDITION( pIterator != NULL );
   PSO_INV_CONDITION( pIterator->initialized == PSOC_DIR_ACCESS_SIGNATURE );
   PSO_PRE_CONDITION( dirName   != NULL );
   PSO_PRE_CONDITION( pError    != NULL );

#if defined (WIN32)
   PSO_PRE_CONDITION( pIterator->dirName[0] == '\0' );
#else
   PSO_PRE_CONDITION( pIterator->pDir == NULL );
#endif

#if defined (WIN32)
   pError = pError; /* Avoid a warning */
   i = strlen( dirName );
   if ( i > PATH_MAX - 3 ) return -1;

   if ( dirName[i-1] == '/' || dirName[i-1] == '\\' ) {
      sprintf( pIterator->dirName, "%s*", dirName );
   }
   else {
      sprintf( pIterator->dirName, "%s/*", dirName );
   }
   
#else
   pIterator->pDir = opendir( dirName );
   if ( pIterator->pDir == NULL ) {
      psocSetError( pError, PSOC_ERRNO_HANDLE, errno );
      return false;
   }
#endif

#if ! defined (WIN32)
   PSO_POST_CONDITION( pIterator->pDir != NULL );
#endif

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */


