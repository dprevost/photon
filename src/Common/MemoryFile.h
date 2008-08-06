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

#ifndef VDSC_MEMORY_FILE_H
#define VDSC_MEMORY_FILE_H

#include "Common.h"
#include "ErrorHandler.h"

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \defgroup vdscMemoryFile The Memory File module
 *
 * This module encapsulates the difference between mmap on Unix/Linux 
 * and CreateFileMappingA (and similar calls) on WIN32.
 *
 * It also provides additional functionality to manage the backstore 
 * file (checking its status and more), etc. Additional features (like
 * adding secure access through ACLs) could be added to this module 
 * eventually.
 *
 * \ingroup vdscCommon
 * @{
 */

/**
 * \file
 * Encapsulates the difference between mmap on Unix/Linux and 
 * CreateFileMappingA (and similar calls) on WIN32.
 *
 * It also provides additional functionality to manage the backstore 
 * file (checking its status and more), etc. Additional features (like
 * adding secure access through ACLs) could be added to this module 
 * eventually.
 */

/** Unique identifier for the vdscMemoryFile struct. */
#define VDSC_MEMFILE_SIGNATURE ((unsigned)0x438eb633)

/**
 * A simple structure used to get information on the status of the backstore.
 */
struct vdscMemoryFileStatus
{
   /** Set to one if the file exist. */
   int  fileExist;
   /** Set to one if the file is readable. */
   int  fileReadable;  
   /** Set to one if the file is writable. */
   int  fileWritable;  
   /** Set to one if the length matches the expectation. */
   int  lenghtOK;
   /** The length of the file. */
   size_t actualLLength;
};

typedef struct vdscMemoryFileStatus vdscMemoryFileStatus;

/** This struct holds all data needed for accessing the memory file. */
struct vdscMemoryFile
{
   /** Set to VDSC_MEMFILE_SIGNATURE at initialization. */
   unsigned int initialized;
   
   /**
    * Base address of the memory-mapped file.
    * The content of this variable is determined when the shared
    * memory segment is initialized (when mmap or the Win32 
    * equivalent is called).
    */
   void * baseAddr;

   /** Length of the mapped file in bytes. */
   size_t length;

   /** Name of the backstore file. */
   char name[PATH_MAX];
   
#if defined (WIN32)
   /** Win32 specific - the underlying file handle (a void*). */
   HANDLE fileHandle;
#else
   /** Unix/linux specific - the underlying file handle. */
   int fileHandle;
#endif

#if defined (WIN32)
   /** Win32 specific - the underlying map handle (a void*). */
   HANDLE mapHandle;
   
   /** Win32 specific - the security attribute of the map. */
   SECURITY_ATTRIBUTES sa;
   /** Win32 specific - the security descriptor of the map. */
   SECURITY_DESCRIPTOR sd;
#endif
   
};

typedef struct vdscMemoryFile vdscMemoryFile;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*! \brief Initialize a vdscMemoryFile struct. */
VDSF_COMMON_EXPORT
void vdscInitMemoryFile( vdscMemoryFile * pMem,
                         size_t           kblength, 
                         const char     * filename );

/*! \brief Terminate access to a vdscMemoryFile struct. */
VDSF_COMMON_EXPORT
void vdscFiniMemoryFile( vdscMemoryFile * pMem );

/*! \brief Return the status of the backstore file. */
VDSF_COMMON_EXPORT
void vdscBackStoreStatus( vdscMemoryFile       * pMem,
                          vdscMemoryFileStatus * pStatus );
   
/*! \brief Create the backstore file */
VDSF_COMMON_EXPORT
bool vdscCreateBackstore( vdscMemoryFile   * pMem,
                          int                filePerms,
                          vdscErrorHandler * pError );
   
/*! 
 *  \brief "Open" an access to the memory file (this might load the 
 *  backstore in memory).
 */
VDSF_COMMON_EXPORT
bool vdscOpenMemFile( vdscMemoryFile   * pMem,
                      void            ** ppAddr,
                      vdscErrorHandler * pError );
   
/*! \brief Create a copy of the backstore file */
VDSF_COMMON_EXPORT
bool vdscCopyBackstore( vdscMemoryFile   * pMem,
                        int                filePerms,
                        vdscErrorHandler * pError );

/*!
 *  \brief Close our access the memory file (possibly removing the memory
 *  file itself from memory).
 */
VDSF_COMMON_EXPORT
void vdscCloseMemFile( vdscMemoryFile   * pMem,
                       vdscErrorHandler * pError );

/*! \brief Synchronize the memory file to the backstore (disk) */
VDSF_COMMON_EXPORT
bool vdscSyncMemFile( vdscMemoryFile   * pMem,
                      vdscErrorHandler * pError );

/*! \brief Set the shared memory to read-only. */
static inline
bool vdscSetReadOnly( vdscMemoryFile   * pMem,
                      vdscErrorHandler * pError );

/*! \brief Set the shared memory to be writable. */
static inline
bool vdscSetReadWrite( vdscMemoryFile   * pMem,
                       vdscErrorHandler * pError );


/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * @}
 */

END_C_DECLS

#include "MemoryFile.inl"

#endif /* VDSC_MEMORY_FILE_H */
