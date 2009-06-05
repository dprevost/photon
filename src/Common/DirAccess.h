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

#ifndef PSOC_DIR_ACCESS_H
#define PSOC_DIR_ACCESS_H

#include "Common.h"
#include "ErrorHandler.h"

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \defgroup psocDirAccess The Directory Iterator module
 *
 * This module encapsulates the differences between the different 
 * OSes when it comes to iterate through all files in a directory.
 *
 * The logic of this module (Open/Close/etc.) follows
 * the standard POSIX way of doing things although the code itself 
 * is not using the POSIX calls on some platforms (Windows, so far).
 *
 * \ingroup psocCommon
 * @{
 */

/**
 * \file
 * This module encapsulates the differences between the different 
 * OSes when it comes to iterate through all files in a directory.
 *
 * The logic of this module (Open/Close/etc.) follows
 * the standard POSIX way of doing things although the code itself 
 * is not using the POSIX calls on some platforms (Windows, so far).
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** Unique identifier for the psocDirIterator struct. */
#define PSOC_DIR_ACCESS_SIGNATURE ((unsigned)0xca3dd33e)

/**
 * \cond NO_DIR_DEFINES_PLEASE
 * This should stop doxygen from including the define below.
 */
#if ! defined (WIN32)

#  if HAVE_DIRENT_H
#    include <dirent.h>
#    define NAMLEN(dirent) strlen((dirent)->d_name)
#  endif

#endif /* WIN32 */
/**
 * \endcond
 */
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Iterates through a directory. The logic of this module (Open/Close/etc.) 
 * follows the standard POSIX way of doing things although the code itself 
 * is not using the POSIX calls on some platforms (Windows, so far).
 */

struct psocDirIterator
{
   /** Set to PSOC_DIR_ACCESS_SIGNATURE at initialization. */
   unsigned int initialized;

#if defined ( WIN32 )
   /** Win32 specific - the underlying file handle (a void*). */
   HANDLE          handle;
   /** Win32 specific - the underlying data structure for the iterator. */
   WIN32_FIND_DATA data;
   /** The name of the directory. */
   char            dirName[PATH_MAX];
#else
   /** The directory stream */
   DIR* pDir;
#endif

};

typedef struct psocDirIterator psocDirIterator;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*! \brief Initialize the struct psocDirIterator. */
PHOTON_COMMON_EXPORT
void psocInitDir( psocDirIterator * pIterator );

/*! \brief Terminate access to the struct psocDirIterator. */
PHOTON_COMMON_EXPORT
void psocFiniDir( psocDirIterator * pIterator );

/*! \brief Open the directory. */
PHOTON_COMMON_EXPORT
bool psocOpenDir( psocDirIterator  * pIterator,
                  const char       * dirName,
                  psocErrorHandler * pError );

/*! \brief Get the next file name. */
PHOTON_COMMON_EXPORT
const char * psocDirGetNextFileName( psocDirIterator  * pIterator,
                                     psocErrorHandler * pError );

/*! \brief Close the directory. */
PHOTON_COMMON_EXPORT
void psocCloseDir( psocDirIterator * pIterator );


/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * @}
 */

END_C_DECLS

#endif /* PSOC_DIR_ACCESS_H */

