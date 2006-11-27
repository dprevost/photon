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

#include "DirAccess.h"
#include "Common.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*!
 * 
 * \param[in] pIterator  A pointer to the vdscDirIterator struct itself.
 * \param[in] pError     A pointer to a vdscErrorHandler struct (for handling
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
 *                           calling vdscCloseDir()).
 *
 * \invariant \em pIterator->initialized must equal 
 *                ::VDSC_DIR_ACCESS_SIGNATURE.
 *
 * \post The return value cannot be NULL if no error was encountered.
 *
 */
const char* vdscDirGetNextFileName( vdscDirIterator*  pIterator,
                                    vdscErrorHandler* pError )
{
#if ! defined (WIN32)
   struct dirent * pEntry;
#endif

   VDS_PRE_CONDITION( pIterator != NULL );
   VDS_INV_CONDITION( pIterator->initialized == VDSC_DIR_ACCESS_SIGNATURE );
   VDS_PRE_CONDITION( pError    != NULL );

#if defined (WIN32)
   VDS_PRE_CONDITION( pIterator->dirName[0] != '\0' );
#else
   VDS_PRE_CONDITION( pIterator->pDir != NULL );
#endif

#if defined (WIN32)
   if ( pIterator->handle == VDS_INVALID_HANDLE )
   {
      pIterator->handle = 
         FindFirstFile( pIterator->dirName, &pIterator->data );
   }
   if ( pIterator->handle == VDS_INVALID_HANDLE )
   {
      vdscSetError( pError, VDSC_WINERR_HANDLE, GetLastError() );
      return NULL;
   }
   
   for ( ;; )
   {
      int err = FindNextFile( pIterator->handle, &pIterator->data );
      if ( err == 0 )
      {
         if ( GetLastError() != ERROR_NO_MORE_FILES )
            vdscSetError( pError, VDSC_WINERR_HANDLE, GetLastError() );
         return NULL;
      }
      
      if ( strlen( pIterator->data.cFileName ) == 1 && 
           pIterator->data.cFileName[0] == '.' )
         continue;
      if ( strlen( pIterator->data.cFileName ) == 2 && 
           pIterator->data.cFileName[0] == '.' && 
           pIterator->data.cFileName[1] == '.' )
         continue;

      VDS_POST_CONDITION( pIterator->data.cFileName  != NULL );

      return pIterator->data.cFileName;
   }

#else
   for ( ;; )
   {
      errno = 0; /* To be safe */
      pEntry = readdir( pIterator->pDir );
      if ( pEntry == NULL )
      {
         if ( errno != 0 )
            vdscSetError( pError, VDSC_ERRNO_HANDLE, errno );
         break;
      }
      
      if ( strlen( pEntry->d_name ) == 1 && pEntry->d_name[0] == '.' )
         continue;
      if ( strlen( pEntry->d_name ) == 2 && 
           pEntry->d_name[0] == '.' && pEntry->d_name[1] == '.' )
         continue;
      
      VDS_POST_CONDITION( pEntry->d_name != NULL );

      return pEntry->d_name;
   }

   return NULL;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*!
 * 
 * \param[in] pIterator  A pointer to the vdscDirIterator struct itself.
 *
 * \pre \em pIterator cannot be NULL.
 * \invariant \em pIterator->initialized must equal 
 *                ::VDSC_DIR_ACCESS_SIGNATURE.
 *
 */
void vdscFiniDir( vdscDirIterator * pIterator )
{
   VDS_PRE_CONDITION( pIterator != NULL );
   VDS_INV_CONDITION( pIterator->initialized == VDSC_DIR_ACCESS_SIGNATURE );

#if defined ( WIN32 )
   pIterator->handle = VDS_INVALID_HANDLE;
   memset( pIterator->dirName, 0, PATH_MAX);
#else
   pIterator->pDir = NULL;
#endif

   pIterator->initialized = 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*!
 * 
 * \param[in] pIterator  A pointer to the vdscDirIterator struct itself.
 *
 * \pre \em pIterator cannot be NULL.
 *
 */
void vdscInitDir( vdscDirIterator * pIterator )
{
   VDS_PRE_CONDITION( pIterator != NULL );

#if defined ( WIN32 )
   pIterator->handle = VDS_INVALID_HANDLE;
   memset( pIterator->dirName, 0, PATH_MAX);
#else
   pIterator->pDir = NULL;
#endif

   pIterator->initialized = VDSC_DIR_ACCESS_SIGNATURE;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*!
 * 
 * \param[in] pIterator  A pointer to the vdscDirIterator struct itself.
 *
 * \pre \em pIterator cannot be NULL.
 * \invariant \em pIterator->initialized must equal 
 *                ::VDSC_DIR_ACCESS_SIGNATURE.
 *
 */
void vdscCloseDir( vdscDirIterator * pIterator )
{
   VDS_PRE_CONDITION( pIterator != NULL );
   VDS_INV_CONDITION( pIterator->initialized == VDSC_DIR_ACCESS_SIGNATURE );

#if defined (WIN32)
   if ( pIterator->handle != VDS_INVALID_HANDLE )
      FindClose( pIterator->handle );
   pIterator->handle = VDS_INVALID_HANDLE;
   memset( pIterator->dirName, 0, PATH_MAX);
#else
   if ( pIterator->pDir != NULL )
      closedir (pIterator->pDir);   
   pIterator->pDir = NULL;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*!
 * 
 * \param[in] pIterator  A pointer to the vdscDirIterator struct itself.
 * \param[in] dirName    The directory name.
 * \param[in] pError     A pointer to a vdscErrorHandler struct (for handling
 *                       C library or Win32 API errors).
 *
 * \retval 0 on success
 * \retval -1 on error (use pError to retrieve the error)
 *
 * \pre \em pIterator cannot be NULL.
 * \pre \em dirName cannot be NULL.
 * \pre \em pError cannot be NULL.
 * \pre (Win32 specific) \em pIterator->dirName must be empty (which insures
 *                           that this function was not called twice without
 *                           calling vdscCloseDir()).
 * \pre (Posix specific) \em pIterator->pDir must be NULL (which insures
 *                           that this function was not called twice without
 *                           calling vdscCloseDir()).
 *
 * \invariant \em pIterator->initialized must equal 
 *                ::VDSC_DIR_ACCESS_SIGNATURE.
 *
 */
int vdscOpenDir( vdscDirIterator * pIterator, 
                 const char*       dirName,
                 vdscErrorHandler* pError )
{
#if defined (WIN32)
   int i = 0;
#endif

   VDS_PRE_CONDITION( pIterator != NULL );
   VDS_INV_CONDITION( pIterator->initialized == VDSC_DIR_ACCESS_SIGNATURE );
   VDS_PRE_CONDITION( dirName   != NULL );
   VDS_PRE_CONDITION( pError    != NULL );

#if defined (WIN32)
   VDS_PRE_CONDITION( pIterator->dirName[0] == '\0' );
#else
   VDS_PRE_CONDITION( pIterator->pDir == NULL );
#endif

#if defined (WIN32)
   pError = pError; /* Avoid a warning */
   i = strlen( dirName );
   if ( i > PATH_MAX - 3 )
      return -1;

   if ( dirName[i-1] == '/' || dirName[i-1] == '\\' )
      sprintf( pIterator->dirName, "%s*", dirName );
   else
      sprintf( pIterator->dirName, "%s/*", dirName );

#else
   pIterator->pDir = opendir( dirName );
   if ( pIterator->pDir == NULL )
   {
      vdscSetError( pError, VDSC_ERRNO_HANDLE, errno );
      return -1;
   }
#endif

#if ! defined (WIN32)
   VDS_POST_CONDITION( pIterator->pDir != NULL );
#endif

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */


