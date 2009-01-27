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

#ifndef PSOC_MEMORY_FILE_H
#define PSOC_MEMORY_FILE_H

#include "Common.h"
#include "ErrorHandler.h"

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \defgroup psocMemoryFile The Memory File module
 *
 * This module encapsulates the difference between mmap on Unix/Linux 
 * and CreateFileMappingA (and similar calls) on WIN32.
 *
 * It also provides additional functionality to manage the backstore 
 * file (checking its status and more), etc. Additional features (like
 * adding secure access through ACLs) could be added to this module 
 * eventually.
 *
 * \ingroup psocCommon
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

/** Unique identifier for the psocMemoryFile struct. */
#define PSOC_MEMFILE_SIGNATURE ((unsigned)0x438eb633)

/**
 * A simple structure used to get information on the status of the backstore.
 */
struct psocMemoryFileStatus
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

typedef struct psocMemoryFileStatus psocMemoryFileStatus;

/** This struct holds all data needed for accessing the memory file. */
struct psocMemoryFile
{
   /** Set to PSOC_MEMFILE_SIGNATURE at initialization. */
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

typedef struct psocMemoryFile psocMemoryFile;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*! \brief Initialize a psocMemoryFile struct. */
PHOTON_COMMON_EXPORT
void psocInitMemoryFile( psocMemoryFile * pMem,
                         size_t           kblength, 
                         const char     * filename );

/*! \brief Terminate access to a psocMemoryFile struct. */
PHOTON_COMMON_EXPORT
void psocFiniMemoryFile( psocMemoryFile * pMem );

/*! \brief Return the status of the backstore file. */
PHOTON_COMMON_EXPORT
void psocBackStoreStatus( psocMemoryFile       * pMem,
                          psocMemoryFileStatus * pStatus );
   
/*! \brief Create the backstore file */
PHOTON_COMMON_EXPORT
bool psocCreateBackstore( psocMemoryFile   * pMem,
                          int                filePerms,
                          psocErrorHandler * pError );
   
/*! 
 *  \brief "Open" an access to the memory file (this might load the 
 *  backstore in memory).
 */
PHOTON_COMMON_EXPORT
bool psocOpenMemFile( psocMemoryFile   * pMem,
                      void            ** ppAddr,
                      psocErrorHandler * pError );
   
/*! \brief Create a copy of the backstore file */
PHOTON_COMMON_EXPORT
bool psocCopyBackstore( psocMemoryFile   * pMem,
                        int                filePerms,
                        psocErrorHandler * pError );

/*!
 *  \brief Close our access the memory file (possibly removing the memory
 *  file itself from memory).
 */
PHOTON_COMMON_EXPORT
void psocCloseMemFile( psocMemoryFile   * pMem,
                       psocErrorHandler * pError );

/*! \brief Synchronize the memory file to the backstore (disk) */
PHOTON_COMMON_EXPORT
bool psocSyncMemFile( psocMemoryFile   * pMem,
                      psocErrorHandler * pError );

/*! \brief Set the shared memory to read-only. */
static inline
bool psocSetReadOnly( psocMemoryFile   * pMem,
                      psocErrorHandler * pError );

/*! \brief Set the shared memory to be writable. */
static inline
bool psocSetReadWrite( psocMemoryFile   * pMem,
                       psocErrorHandler * pError );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * @}
 */

END_C_DECLS

#include "MemoryFile.inl"

#endif /* PSOC_MEMORY_FILE_H */
