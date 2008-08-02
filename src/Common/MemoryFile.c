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

#include "MemoryFile.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*! 
 * All data members are initialized, including the \em initialized field
 * (to ::VDSC_MEMFILE_SIGNATURE).
 *
 * \param[in] pMem      A pointer to the vdscMemoryFile struct itself.
 * \param[in] kblength  The length of the shared memory file. It is 
 *                      be provided in kilobytes (although we store it 
 *                      internally in bytes).
 * \param[in] filename  The filename of the backstore file to be used.

 * \pre \em pMem cannot be NULL
 * \pre \em kblength must be positive
 * \pre \em filename cannot be NULL or the empty string ("")
 */
void vdscInitMemoryFile( vdscMemoryFile* pMem,
                         size_t          kblength,
                         const char *    filename )
{
   VDS_PRE_CONDITION( pMem != NULL );
   VDS_PRE_CONDITION( kblength > 0 );
   VDS_PRE_CONDITION( filename != NULL );
   VDS_PRE_CONDITION( filename[0] != '\0' );
   
   pMem->baseAddr = VDS_MAP_FAILED;
   pMem->length   = kblength*1024;
   pMem->fileHandle = VDS_INVALID_HANDLE;
   memset( pMem->name, 0, PATH_MAX );
   strncpy( pMem->name, filename, PATH_MAX );

   pMem->initialized = VDSC_MEMFILE_SIGNATURE;
   
#if defined (WIN32)
   pMem->mapHandle = VDS_INVALID_HANDLE;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*!
 * \param[in] pMem      A pointer to the vdscMemoryFile struct itself.
 *
 * \pre \em pMem cannot be NULL.
 * \invariant \em pMem->initialized must equal ::VDSC_MEMFILE_SIGNATURE.
 */
void vdscFiniMemoryFile( vdscMemoryFile* pMem )
{
   VDS_PRE_CONDITION( pMem != NULL );
   VDS_INV_CONDITION( pMem->initialized == VDSC_MEMFILE_SIGNATURE );
   
   pMem->baseAddr = VDS_MAP_FAILED;
   pMem->length   = 0;
   pMem->fileHandle = VDS_INVALID_HANDLE;
   memset( pMem->name, 0, PATH_MAX );

   pMem->initialized = 0;
   
#if defined (WIN32)
   pMem->mapHandle = VDS_INVALID_HANDLE;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*!
 * Tests in that function look a bit redundant - if the file is not
 * readable... we won't be able to use it anyway. However by trying
 * all combinations, we will hopefully provide better diagnostics to 
 * correct the problems if any.
 *
 * \param[in] pMem      A pointer to the vdscMemoryFile struct itself.
 * \param[in,out] pStatus A pointer to the vdscMemoryFileStatus struct. The
 *                        pointer itself is not modified but the content of
 *                        the struct is.
 *
 * \pre \em pMem cannot be NULL.
 * \pre \em pStatus cannot be NULL.
 * \invariant \em pMem->initialized must equal ::VDSC_MEMFILE_SIGNATURE.
 * \invariant \em pMem->name cannot be empty.
 */

void
vdscBackStoreStatus( vdscMemoryFile*       pMem,
                     vdscMemoryFileStatus* pStatus )
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

   VDS_PRE_CONDITION( pMem    != NULL );
   VDS_INV_CONDITION( pMem->initialized == VDSC_MEMFILE_SIGNATURE );
   VDS_INV_CONDITION( pMem->name[0] != '\0' );
   VDS_PRE_CONDITION( pStatus != NULL );

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
 * \param[in] pMem   A pointer to the vdscMemoryFile struct itself.
 * \param[in] pError  A pointer to the vdscErrorHandler struct (used for 
 *                    handling errors from the OS).
 *
 * \retval 0 on success
 * \retval -1 on error (use pError to retrieve the error)
 *
 * \pre \em pMem cannot be NULL.
 * \pre \em pError cannot be NULL.
 * \invariant \em pMem->initialized must equal ::VDSC_MEMFILE_SIGNATURE.
 * \invariant \em pMem->name cannot be empty.
 * \invariant \em pMem->length must be positive.
 *
 */

int vdscCopyBackstore( vdscMemoryFile*   pMem,
                       int               filePerms,
                       vdscErrorHandler* pError )
{
   int fdIn = -1, fdOut = -1;
   char bckName [PATH_MAX];
   size_t length = 1024*1024, leftToCopy;
   unsigned char * buffer = NULL;
   ssize_t i, j, k;
   
   VDS_PRE_CONDITION( pMem    != NULL );
   VDS_INV_CONDITION( pMem->initialized == VDSC_MEMFILE_SIGNATURE );
   VDS_INV_CONDITION( pMem->name[0] != '\0' );
   VDS_INV_CONDITION( pMem->length > 0 );
   VDS_PRE_CONDITION( pError != NULL );
   VDS_PRE_CONDITION( ( filePerms & 0600 ) == 0600 );
   
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
   
   fdIn = open( pMem->name, O_RDONLY );
   if ( fdIn < 0 ) goto error;
   fdOut = open( bckName, O_WRONLY | O_CREAT | O_TRUNC, filePerms );
   if ( fdOut < 0 ) goto error;
   
   /* 
    * The code below is probably too complex - I rarely saw a case where
    * read/write did not complete when reading from a file (a socket or 
    * pipe, yes, a file?). But...
    */
   while ( leftToCopy > 0 ) {
      i = read( fdIn, buffer, length );
      if ( i < 0 ) goto error;
      if ( i == 0 ) break;
      leftToCopy -= i;
      if ( leftToCopy < length ) length = leftToCopy;
      k = 0;
      do {
         j = write( fdOut, &buffer[k], i );
         if ( j < 0 ) goto error;
         i -= j;
         k += j;
      } while ( i > 0 );
   }
   
   free( buffer );
   close( fdIn );
   close( fdOut );

   return 0;
   
error:
   if ( buffer ) free( buffer );
   if ( fdIn  > -1 ) close( fdIn );
   if ( fdOut > -1 ) close( fdOut );

   vdscSetError( pError, VDSC_ERRNO_HANDLE, errno );
   
   return -1;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
/*!
 *
 * \param[in] pMem   A pointer to the vdscMemoryFile struct itself.
 * \param[in] filePerms The file permissions for the soon to be created 
 *                      backstore file.
 * \param[in] pError  A pointer to the vdscErrorHandler struct (used for 
 *                    handling errors from the OS).
 *
 * \retval 0 on success
 * \retval -1 on error (use pError to retrieve the error)
 *
 * \pre \em pMem cannot be NULL.
 * \pre \em pError cannot be NULL.
 * \pre \em filePerms must have at least the read and write bits for the owner
 *          set on (0600). Otherwise we would not be able to access the
 *          file once created.
 * \invariant \em pMem->initialized must equal ::VDSC_MEMFILE_SIGNATURE.
 * \invariant \em pMem->name cannot be empty.
 * \invariant \em pMem->length must be positive.
 *
 */

int 
vdscCreateBackstore( vdscMemoryFile*   pMem,
                     int               filePerms,
                     vdscErrorHandler* pError )
{
   FILE* fp;
   size_t numWritten;
   int errcode = 0, fd;
   char buf[1];
   
   VDS_PRE_CONDITION( pMem    != NULL );
   VDS_INV_CONDITION( pMem->initialized == VDSC_MEMFILE_SIGNATURE );
   VDS_INV_CONDITION( pMem->name[0] != '\0' );
   VDS_INV_CONDITION( pMem->length > 0 );
   VDS_PRE_CONDITION( pError != NULL );
   VDS_PRE_CONDITION( ( filePerms & 0600 ) == 0600 );
   
   
   /* Create the file with the right permissions */
   fd = creat( pMem->name, filePerms );
   if ( fd < 0 ) {
      vdscSetError( pError, VDSC_ERRNO_HANDLE, errno );      
      return -1;
   }
   close( fd );

   /**
    * \todo Replace the fseek with lseek (the call to creat to set the
    * file perms was an afterthough and now the code is a bit weird).
    * We should also test somewhere (with autoconf) which lseek we should
    * use, lseek or lseek64.
    */
   fp = fopen( pMem->name, "wb" );
   if ( fp == NULL ) {
      vdscSetError( pError, VDSC_ERRNO_HANDLE, errno );      
      return -1;
   }
   
   errcode = fseek( fp, pMem->length-1, SEEK_SET );
   if ( errcode == 0 ) {
      buf[0] = 0;
      numWritten = fwrite( buf, 1, 1, fp );
      if ( numWritten != 1 ) errcode = -1;
   }
   
   fclose( fp );
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*!
 * On Win32, we need to use CreateFileMappingA and MapViewOfFileEx to
 * create our shared memory file. But these functions require a Win32 
 * HANDLE as the file handle (on NT/2000/XP, HANDLE is typedef to a void*). 
 * This means we cannot use regular ANSI calls but must use the Win32 API
 * to open the file, etc.
 *
 * \param[in] pMem   A pointer to the vdscMemoryFile struct itself.
 * \param[out] ppAddr A pointer to the address where the file is mapped in
 *                    memory.
 * \param[in] pError  A pointer to the vdscErrorHandler struct (used for 
 *                    handling errors from the OS).
 *
 * \retval 0 on success
 * \retval -1 on error (use pError to retrieve the error)
 *
 * \pre \em pMem cannot be NULL.
 * \pre \em ppAddr cannot be NULL.
 * \pre \em pError cannot be NULL.
 * \invariant \em pMem->initialized must equal ::VDSC_MEMFILE_SIGNATURE.
 * \invariant \em pMem->name cannot be empty.
 * \invariant \em pMem->length must be positive.
 * \post \em *ppAddr cannot be ::VDS_MAP_FAILED (NULL on Win32, (void*)-1
 *           on many Posix-compliant systems).
 * \post \em pMem->baseAddr cannot be ::VDS_MAP_FAILED.
 * \post \em pMem->fileHandle cannot be ::VDS_INVALID_HANDLE.
 * \post (Win32 only) \em pMem->mapHandle cannot be ::VDS_INVALID_HANDLE.
 *
 */

int 
vdscOpenMemFile( vdscMemoryFile*   pMem, 
                 void**            ppAddr,
                 vdscErrorHandler* pError )
{
   VDS_PRE_CONDITION( pMem != NULL );
   VDS_INV_CONDITION( pMem->initialized == VDSC_MEMFILE_SIGNATURE );
   VDS_INV_CONDITION( pMem->name[0] != '\0' );
   VDS_INV_CONDITION( pMem->length > 0 );
   VDS_PRE_CONDITION( ppAddr != NULL );
   VDS_PRE_CONDITION( pError != NULL );

   /**
    * \todo This section on Win32 is wrong!!!! We should setup the 
    * security attr. of the 
    * shared memory object to be those of the shared memory file
    * (using GetNamedSecurityInfo(), possibly). The initial sa of the
    * file must be initialized by the watchdog.
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
   if ( pMem->fileHandle == VDS_INVALID_HANDLE ) {
      vdscSetError( pError, VDSC_WINERR_HANDLE, GetLastError() );
      return -1;
   }
   
   pMem->mapHandle = CreateFileMappingA( pMem->fileHandle,
                                         &pMem->sa,
                                         PAGE_READWRITE,
                                         0,
                                         0,
                                         NULL );
   if ( pMem->mapHandle  == VDS_INVALID_HANDLE ) {
      vdscSetError( pError, VDSC_WINERR_HANDLE, GetLastError() );
      return -1;
   }

   pMem->baseAddr = MapViewOfFileEx( pMem->mapHandle,
                                     FILE_MAP_WRITE,
                                     0,
                                     0,
                                     0,
                                     NULL );

   if ( pMem->baseAddr == VDS_MAP_FAILED ) {
      vdscSetError( pError, VDSC_WINERR_HANDLE, GetLastError() );
      return -1;
   }
   else {
      *ppAddr = pMem->baseAddr;
   }
   
#else /* WIN32 */

   pMem->fileHandle = open( pMem->name, O_RDWR );
   if ( pMem->fileHandle == VDS_INVALID_HANDLE ) {
      vdscSetError( pError, VDSC_ERRNO_HANDLE, errno );
      return -1;
   }

   pMem->baseAddr = mmap( NULL,
                          pMem->length,
                          PROT_READ | PROT_WRITE,
                          MAP_SHARED,
                          pMem->fileHandle,
                          0 );

   if ( pMem->baseAddr == VDS_MAP_FAILED ) {
      vdscSetError( pError, VDSC_ERRNO_HANDLE, errno );
      return -1;
   }
   else {
      *ppAddr = pMem->baseAddr;
   }
   
#endif /* WIN32 */
   
   /* Just in case */
   VDS_POST_CONDITION( *ppAddr != VDS_MAP_FAILED );
   VDS_POST_CONDITION( pMem->baseAddr   != VDS_MAP_FAILED );
   VDS_POST_CONDITION( pMem->fileHandle != VDS_INVALID_HANDLE );
#if defined (WIN32)
   VDS_POST_CONDITION( pMem->mapHandle  != VDS_INVALID_HANDLE );
#endif   

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*!
 * \param[in] pMem   A pointer to the vdscMemoryFile struct itself.
 * \param[in] pError  A pointer to the vdscErrorHandler struct (used for 
 *                    handling errors from the OS).
 *
 * \pre \em pMem cannot be NULL.
 * \pre \em pError cannot be NULL.
 * \invariant \em pMem->initialized must equal ::VDSC_MEMFILE_SIGNATURE.
 *
 */
void
vdscCloseMemFile( vdscMemoryFile*   pMem,
                  vdscErrorHandler* pError )
{
   VDS_PRE_CONDITION( pMem != NULL );
   VDS_INV_CONDITION( pMem->initialized == VDSC_MEMFILE_SIGNATURE );
   VDS_PRE_CONDITION( pError != NULL );

   if ( pMem->baseAddr != VDS_MAP_FAILED ) {
      vdscSyncMemFile( pMem, pError );
   }
   
#if defined (WIN32)
   if (pMem->baseAddr != VDS_MAP_FAILED) {
      UnmapViewOfFile( pMem->baseAddr );
   }
   if ( pMem->mapHandle != VDS_INVALID_HANDLE ) {
      CloseHandle( pMem->mapHandle );
   }
   if ( pMem->fileHandle != VDS_INVALID_HANDLE ) {
      CloseHandle( pMem->fileHandle );
   }
   pMem->mapHandle = VDS_INVALID_HANDLE;
#else
   if ( pMem->baseAddr != VDS_MAP_FAILED ) {
      munmap( pMem->baseAddr, pMem->length );
   }
   if ( pMem->fileHandle != VDS_INVALID_HANDLE ) {
      close( pMem->fileHandle );
   }
#endif

   pMem->fileHandle = VDS_INVALID_HANDLE;
   pMem->baseAddr = VDS_MAP_FAILED;
   pMem->length = 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*!
 * \param[in] pMem   A pointer to the vdscMemoryFile struct itself.
 * \param[in] pError  A pointer to the vdscErrorHandler struct (used for 
 *                    handling errors from the OS).
 *
 * \retval 0 on success
 * \retval -1 on error (use pError to retrieve the error)
 *
 * \pre \em pMem cannot be NULL.
 * \pre \em pError cannot be NULL.
 * \invariant \em pMem->initialized must equal ::VDSC_MEMFILE_SIGNATURE.
 * \invariant \em pMem->baseAddr cannot be equal to ::VDS_MAP_FAILED.
 *
 */
int
vdscSyncMemFile( vdscMemoryFile*   pMem,
                 vdscErrorHandler* pError )
{
   int errcode;

   VDS_PRE_CONDITION( pMem != NULL );
   VDS_INV_CONDITION( pMem->initialized == VDSC_MEMFILE_SIGNATURE );
   VDS_INV_CONDITION( pMem->baseAddr != VDS_MAP_FAILED );
   VDS_PRE_CONDITION( pError != NULL );

#if defined (WIN32)

   errcode = FlushViewOfFile( pMem->baseAddr, 0 );
   if ( errcode == 0 ) {
      vdscSetError( pError, VDSC_WINERR_HANDLE, GetLastError() );
      errcode = -1;
   }
   else {
      errcode = 0;
   }
#elif HAVE_MSYNC
   errcode = msync( pMem->baseAddr, pMem->length, MS_SYNC );
#else
   errcode = fdatasync( pMem->fileHandle );
#endif

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

