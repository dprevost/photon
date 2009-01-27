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

#include "MemoryFile.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*! 
 * All data members are initialized, including the \em initialized field
 * (to ::PSOC_MEMFILE_SIGNATURE).
 *
 * \param[in] pMem      A pointer to the psocMemoryFile struct itself.
 * \param[in] kblength  The length of the shared memory file. It is 
 *                      be provided in kilobytes (although we store it 
 *                      internally in bytes).
 * \param[in] filename  The filename of the backstore file to be used.

 * \pre \em pMem cannot be NULL
 * \pre \em kblength must be positive
 * \pre \em filename cannot be NULL or the empty string ("")
 */
void psocInitMemoryFile( psocMemoryFile * pMem,
                         size_t           kblength,
                         const char     * filename )
{
   PSO_PRE_CONDITION( pMem != NULL );
   PSO_PRE_CONDITION( kblength > 0 );
   PSO_PRE_CONDITION( filename != NULL );
   PSO_PRE_CONDITION( filename[0] != '\0' );
   
   pMem->baseAddr = PSO_MAP_FAILED;
   pMem->length   = kblength*1024;
   pMem->fileHandle = PSO_INVALID_HANDLE;
   memset( pMem->name, 0, PATH_MAX );
   strncpy( pMem->name, filename, PATH_MAX );

   pMem->initialized = PSOC_MEMFILE_SIGNATURE;
   
#if defined (WIN32)
   pMem->mapHandle = PSO_INVALID_HANDLE;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*!
 * \param[in] pMem      A pointer to the psocMemoryFile struct itself.
 *
 * \pre \em pMem cannot be NULL.
 * \invariant \em pMem->initialized must equal ::PSOC_MEMFILE_SIGNATURE.
 */
void psocFiniMemoryFile( psocMemoryFile * pMem )
{
   PSO_PRE_CONDITION( pMem != NULL );
   PSO_INV_CONDITION( pMem->initialized == PSOC_MEMFILE_SIGNATURE );
   
   pMem->baseAddr = PSO_MAP_FAILED;
   pMem->length   = 0;
   pMem->fileHandle = PSO_INVALID_HANDLE;
   memset( pMem->name, 0, PATH_MAX );

   pMem->initialized = 0;
   
#if defined (WIN32)
   pMem->mapHandle = PSO_INVALID_HANDLE;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*!
 * Tests in that function look a bit redundant - if the file is not
 * readable... we won't be able to use it anyway. However by trying
 * all combinations, we will hopefully provide better diagnostics to 
 * correct the problems if any.
 *
 * \param[in] pMem      A pointer to the psocMemoryFile struct itself.
 * \param[in,out] pStatus A pointer to the psocMemoryFileStatus struct. The
 *                        pointer itself is not modified but the content of
 *                        the struct is.
 *
 * \pre \em pMem cannot be NULL.
 * \pre \em pStatus cannot be NULL.
 * \invariant \em pMem->initialized must equal ::PSOC_MEMFILE_SIGNATURE.
 * \invariant \em pMem->name cannot be empty.
 */

void psocBackStoreStatus( psocMemoryFile       * pMem,
                          psocMemoryFileStatus * pStatus )
{
   int err = 0;
#if ! HAVE_ACCESS || ! HAVE_STAT
   FILE* fp = NULL;
#endif
#if HAVE__STAT
   struct _stat info;
#elif HAVE_STAT
   struct stat info;
#endif

   PSO_PRE_CONDITION( pMem    != NULL );
   PSO_INV_CONDITION( pMem->initialized == PSOC_MEMFILE_SIGNATURE );
   PSO_INV_CONDITION( pMem->name[0] != '\0' );
   PSO_PRE_CONDITION( pStatus != NULL );

   pStatus->fileExist     = 0;
   pStatus->fileReadable  = 0;
   pStatus->fileWritable  = 0;
   pStatus->lenghtOK      = 0;
   pStatus->actualLLength = 0;
   
#if HAVE_ACCESS
   err = access( pMem->name, F_OK );
   if ( err != 0 ) goto error;
   pStatus->fileExist = 1;
   err = access( pMem->name, R_OK );
   if ( err != 0 ) goto error;
   pStatus->fileReadable  = 1;
   err = access( pMem->name, W_OK );
   if ( err != 0 ) goto error;
   pStatus->fileWritable  = 1;
#else   
   fp = fopen( pMem->name, "r" );
   if ( fp == NULL ) goto error;
   pStatus->fileExist = 1;
   pStatus->fileReadable  = 1;
   fclose( fp );
   fp = fopen( pMem->name, "w" );
   if ( fp == NULL ) goto error;
   pStatus->fileWritable  = 1;
   fclose( fp );        
#endif

#if HAVE_STAT
   err = stat( pMem->name, &info );
   if ( err != 0 ) goto error;
   pStatus->actualLLength = info.st_size;
#else
   fp = fopen( pMem->name, "rb" );
   if ( fp == NULL ) goto error;
   if (fseek(fp, 0, SEEK_END) == 0 ) {
      pStatus->actualLLength = ftell(fp);
   }
   fclose(fp);
   if ( pStatus->actualLLength == EOF ) {
      pStatus->actualLLength = 0;
   }
#endif
   if ( pStatus->actualLLength != pMem->length ) goto error;
   pStatus->lenghtOK = 1;

 error:
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
/*!
 *
 * \param[in] pMem   A pointer to the psocMemoryFile struct itself.
 * \param[in] pError  A pointer to the psocErrorHandler struct (used for 
 *                    handling errors from the OS).
 *
 * \retval true  on success
 * \retval false on error (use pError to retrieve the error)
 *
 * \pre \em pMem cannot be NULL.
 * \pre \em pError cannot be NULL.
 * \invariant \em pMem->initialized must equal ::PSOC_MEMFILE_SIGNATURE.
 * \invariant \em pMem->name cannot be empty.
 * \invariant \em pMem->length must be positive.
 *
 */

bool psocCopyBackstore( psocMemoryFile   * pMem,
                        int                filePerms,
                        psocErrorHandler * pError )
{
   int fdIn = -1, fdOut = -1;
   char bckName [PATH_MAX];
   size_t length = 1024*1024, leftToCopy;
   unsigned char * buffer = NULL;
   int i, j, k;
   
   PSO_PRE_CONDITION( pMem    != NULL );
   PSO_INV_CONDITION( pMem->initialized == PSOC_MEMFILE_SIGNATURE );
   PSO_INV_CONDITION( pMem->name[0] != '\0' );
   PSO_INV_CONDITION( pMem->length > 0 );
   PSO_PRE_CONDITION( pError != NULL );
   PSO_PRE_CONDITION( ( filePerms & 0600 ) == 0600 );
   
   if ( strlen(pMem->name)+4+1 > PATH_MAX ) {
      errno = ENAMETOOLONG;
      goto error;
   }
   strcpy( bckName, pMem->name );
   strcat( bckName, ".bck" );
   
   if ( pMem->length < length ) length = pMem->length;
   leftToCopy = pMem->length;
   
   buffer = malloc( length );
   if ( buffer == NULL ) goto error;
   
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
   fdIn = _open( pMem->name, O_RDONLY );
#else
   fdIn = open( pMem->name, O_RDONLY );
#endif
   if ( fdIn < 0 ) goto error;
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
   fdOut = _open( bckName, O_WRONLY | O_CREAT | O_TRUNC, filePerms );
#else
   fdOut = open( bckName, O_WRONLY | O_CREAT | O_TRUNC, filePerms );
#endif
   if ( fdOut < 0 ) goto error;
   
   /* 
    * The code below is probably too complex - I rarely saw a case where
    * read/write did not complete when reading from a file (a socket or 
    * pipe, yes, a file?). But...
    */
   while ( leftToCopy > 0 ) {
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
      i = _read( fdIn, buffer, length );
#else
      i = read( fdIn, buffer, length );
#endif
      if ( i < 0 ) goto error;
      if ( i == 0 ) break;
      leftToCopy -= i;
      if ( leftToCopy < length ) length = leftToCopy;
      k = 0;
      do {
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
         j = _write( fdOut, &buffer[k], i );
#else
         j = write( fdOut, &buffer[k], i );
#endif
         if ( j < 0 ) goto error;
         i -= j;
         k += j;
      } while ( i > 0 );
   }
   
   free( buffer );
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
   _close( fdIn );
   _close( fdOut );
#else
   close( fdIn );
   close( fdOut );
#endif

   return true;
   
error:
   if ( buffer ) free( buffer );
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
   if ( fdIn  > -1 ) _close( fdIn );
   if ( fdOut > -1 ) _close( fdOut );
#else
   if ( fdIn  > -1 ) close( fdIn );
   if ( fdOut > -1 ) close( fdOut );
#endif

   psocSetError( pError, PSOC_ERRNO_HANDLE, errno );
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
/*!
 *
 * \param[in] pMem   A pointer to the psocMemoryFile struct itself.
 * \param[in] filePerms The file permissions for the soon to be created 
 *                      backstore file.
 * \param[in] pError  A pointer to the psocErrorHandler struct (used for 
 *                    handling errors from the OS).
 *
 * \retval true  on success
 * \retval false on error (use pError to retrieve the error)
 *
 * \pre \em pMem cannot be NULL.
 * \pre \em pError cannot be NULL.
 * \pre \em filePerms must have at least the read and write bits for the owner
 *          set on (0600). Otherwise we would not be able to access the
 *          file once created.
 * \invariant \em pMem->initialized must equal ::PSOC_MEMFILE_SIGNATURE.
 * \invariant \em pMem->name cannot be empty.
 * \invariant \em pMem->length must be positive.
 *
 */

bool psocCreateBackstore( psocMemoryFile   * pMem,
                          int                filePerms,
                          psocErrorHandler * pError )
{
   FILE* fp;
   size_t numWritten;
   int errcode = 0, fd;
   char buf[1];
   bool ok = true;
   
   PSO_PRE_CONDITION( pMem    != NULL );
   PSO_INV_CONDITION( pMem->initialized == PSOC_MEMFILE_SIGNATURE );
   PSO_INV_CONDITION( pMem->name[0] != '\0' );
   PSO_INV_CONDITION( pMem->length > 0 );
   PSO_PRE_CONDITION( pError != NULL );
   PSO_PRE_CONDITION( ( filePerms & 0600 ) == 0600 );
   
   /* Create the file with the right permissions */
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
   fd = _creat( pMem->name, _S_IREAD | _S_IWRITE );
#else
   fd = creat( pMem->name, filePerms );
#endif
   if ( fd < 0 ) {
      psocSetError( pError, PSOC_ERRNO_HANDLE, errno );      
      return false;
   }
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
   _close( fd );
#else
   close( fd );
#endif

   /**
    * \todo Replace the fseek with lseek (the call to creat to set the
    * file perms was an afterthough and now the code is a bit weird).
    * We should also test somewhere (with autoconf) which lseek we should
    * use, lseek or lseek64.
    */
   fp = fopen( pMem->name, "wb" );
   if ( fp == NULL ) {
      psocSetError( pError, PSOC_ERRNO_HANDLE, errno );      
      return false;
   }
   
   errcode = fseek( fp, pMem->length-1, SEEK_SET );
   if ( errcode == 0 ) {
      buf[0] = 0;
      numWritten = fwrite( buf, 1, 1, fp );
      if ( numWritten != 1 ) {
         psocSetError( pError, PSOC_ERRNO_HANDLE, errno );      
         ok = false;
      }
   }
   
   fclose( fp );
   
   return ok;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*!
 * On Win32, we need to use CreateFileMappingA and MapViewOfFileEx to
 * create our shared memory file. But these functions require a Win32 
 * HANDLE as the file handle (on NT/2000/XP, HANDLE is typedef to a void*). 
 * This means we cannot use regular ANSI calls but must use the Win32 API
 * to open the file, etc.
 *
 * \param[in] pMem   A pointer to the psocMemoryFile struct itself.
 * \param[out] ppAddr A pointer to the address where the file is mapped in
 *                    memory.
 * \param[in] pError  A pointer to the psocErrorHandler struct (used for 
 *                    handling errors from the OS).
 *
 * \retval true  on success
 * \retval false on error (use pError to retrieve the error)
 *
 * \pre \em pMem cannot be NULL.
 * \pre \em ppAddr cannot be NULL.
 * \pre \em pError cannot be NULL.
 * \invariant \em pMem->initialized must equal ::PSOC_MEMFILE_SIGNATURE.
 * \invariant \em pMem->name cannot be empty.
 * \invariant \em pMem->length must be positive.
 * \post \em *ppAddr cannot be ::PSO_MAP_FAILED (NULL on Win32, (void*)-1
 *           on many Posix-compliant systems).
 * \post \em pMem->baseAddr cannot be ::PSO_MAP_FAILED.
 * \post \em pMem->fileHandle cannot be ::PSO_INVALID_HANDLE.
 * \post (Win32 only) \em pMem->mapHandle cannot be ::PSO_INVALID_HANDLE.
 *
 */

bool psocOpenMemFile( psocMemoryFile   * pMem, 
                      void            ** ppAddr,
                      psocErrorHandler * pError )
{
   PSO_PRE_CONDITION( pMem != NULL );
   PSO_INV_CONDITION( pMem->initialized == PSOC_MEMFILE_SIGNATURE );
   PSO_INV_CONDITION( pMem->name[0] != '\0' );
   PSO_INV_CONDITION( pMem->length > 0 );
   PSO_PRE_CONDITION( ppAddr != NULL );
   PSO_PRE_CONDITION( pError != NULL );

   /**
    * \todo This section on Win32 is wrong!!!! We should setup the 
    * security attr. of the 
    * shared memory object to be those of the shared memory file
    * (using GetNamedSecurityInfo(), possibly). The initial sa of the
    * file must be initialized by the server.
    */
#if defined(WIN32)
   InitializeSecurityDescriptor(&pMem->sd, SECURITY_DESCRIPTOR_REVISION);
   SetSecurityDescriptorDacl(&pMem->sd, TRUE, NULL, FALSE);
   pMem->sa.nLength = sizeof(SECURITY_ATTRIBUTES);
   pMem->sa.lpSecurityDescriptor = &pMem->sd;
   pMem->sa.bInheritHandle       = TRUE;

   pMem->fileHandle = CreateFileA ( 
      pMem->name,
      GENERIC_READ | GENERIC_WRITE,
      FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
      &pMem->sa,
      OPEN_EXISTING,
      0,
      0 );
   if ( pMem->fileHandle == PSO_INVALID_HANDLE ) {
      psocSetError( pError, PSOC_WINERR_HANDLE, GetLastError() );
      return false;
   }
   
   pMem->mapHandle = CreateFileMappingA( pMem->fileHandle,
                                         &pMem->sa,
                                         PAGE_READWRITE,
                                         0,
                                         0,
                                         NULL );
   if ( pMem->mapHandle  == PSO_INVALID_HANDLE ) {
      psocSetError( pError, PSOC_WINERR_HANDLE, GetLastError() );
      return false;
   }

   pMem->baseAddr = MapViewOfFileEx( pMem->mapHandle,
                                     FILE_MAP_WRITE,
                                     0,
                                     0,
                                     0,
                                     NULL );

   if ( pMem->baseAddr == PSO_MAP_FAILED ) {
      psocSetError( pError, PSOC_WINERR_HANDLE, GetLastError() );
      return false;
   }

   *ppAddr = pMem->baseAddr;
   
#else /* WIN32 */

   pMem->fileHandle = open( pMem->name, O_RDWR );
   if ( pMem->fileHandle == PSO_INVALID_HANDLE ) {
      psocSetError( pError, PSOC_ERRNO_HANDLE, errno );
      return false;
   }

   pMem->baseAddr = mmap( NULL,
                          pMem->length,
                          PROT_READ | PROT_WRITE,
                          MAP_SHARED,
                          pMem->fileHandle,
                          0 );

   if ( pMem->baseAddr == PSO_MAP_FAILED ) {
      psocSetError( pError, PSOC_ERRNO_HANDLE, errno );
      return false;
   }

   *ppAddr = pMem->baseAddr;
   
#endif /* WIN32 */
   
   /* Just in case */
   PSO_POST_CONDITION( *ppAddr != PSO_MAP_FAILED );
   PSO_POST_CONDITION( pMem->baseAddr   != PSO_MAP_FAILED );
   PSO_POST_CONDITION( pMem->fileHandle != PSO_INVALID_HANDLE );
#if defined (WIN32)
   PSO_POST_CONDITION( pMem->mapHandle  != PSO_INVALID_HANDLE );
#endif   

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*!
 * \param[in] pMem   A pointer to the psocMemoryFile struct itself.
 * \param[in] pError  A pointer to the psocErrorHandler struct (used for 
 *                    handling errors from the OS).
 *
 * \pre \em pMem cannot be NULL.
 * \pre \em pError cannot be NULL.
 * \invariant \em pMem->initialized must equal ::PSOC_MEMFILE_SIGNATURE.
 *
 */
void psocCloseMemFile( psocMemoryFile   * pMem,
                       psocErrorHandler * pError )
{
   PSO_PRE_CONDITION( pMem != NULL );
   PSO_INV_CONDITION( pMem->initialized == PSOC_MEMFILE_SIGNATURE );
   PSO_PRE_CONDITION( pError != NULL );

   if ( pMem->baseAddr != PSO_MAP_FAILED ) {
      psocSyncMemFile( pMem, pError );
   }
   
#if defined (WIN32)
   if (pMem->baseAddr != PSO_MAP_FAILED) {
      UnmapViewOfFile( pMem->baseAddr );
   }
   if ( pMem->mapHandle != PSO_INVALID_HANDLE ) {
      CloseHandle( pMem->mapHandle );
   }
   if ( pMem->fileHandle != PSO_INVALID_HANDLE ) {
      CloseHandle( pMem->fileHandle );
   }
   pMem->mapHandle = PSO_INVALID_HANDLE;
#else
   if ( pMem->baseAddr != PSO_MAP_FAILED ) {
      munmap( pMem->baseAddr, pMem->length );
   }
   if ( pMem->fileHandle != PSO_INVALID_HANDLE ) {
      close( pMem->fileHandle );
   }
#endif

   pMem->fileHandle = PSO_INVALID_HANDLE;
   pMem->baseAddr = PSO_MAP_FAILED;
   pMem->length = 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*!
 * \param[in] pMem   A pointer to the psocMemoryFile struct itself.
 * \param[in] pError  A pointer to the psocErrorHandler struct (used for 
 *                    handling errors from the OS).
 *
 * \retval true  on success
 * \retval false on error (use pError to retrieve the error)
 *
 * \pre \em pMem cannot be NULL.
 * \pre \em pError cannot be NULL.
 * \invariant \em pMem->initialized must equal ::PSOC_MEMFILE_SIGNATURE.
 * \invariant \em pMem->baseAddr cannot be equal to ::PSO_MAP_FAILED.
 *
 */
bool psocSyncMemFile( psocMemoryFile   * pMem,
                      psocErrorHandler * pError )
{
   int errcode;
   bool ok = true;
   
   PSO_PRE_CONDITION( pMem != NULL );
   PSO_INV_CONDITION( pMem->initialized == PSOC_MEMFILE_SIGNATURE );
   PSO_INV_CONDITION( pMem->baseAddr != PSO_MAP_FAILED );
   PSO_PRE_CONDITION( pError != NULL );

#if defined (WIN32)

   errcode = FlushViewOfFile( pMem->baseAddr, 0 );
   if ( errcode == 0 ) {
      psocSetError( pError, PSOC_WINERR_HANDLE, GetLastError() );
      ok = false;
   }
#elif HAVE_MSYNC
   errcode = msync( pMem->baseAddr, pMem->length, MS_SYNC );
   if ( errcode != 0 ) ok = false;
#else
   errcode = fdatasync( pMem->fileHandle );
   if ( errcode != 0 ) ok = false;
#endif
   
   return ok;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

