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

#ifndef VDSC_DIR_ACCESS_H
#define VDSC_DIR_ACCESS_H

#include "Common.h"
#include "ErrorHandler.h"

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \defgroup vdscDirAccess The Directory Iterator module
 *
 * This module encapsulates the differences between the different 
 * OSes when it comes to iterate through all files in a directory.
 *
 * The logic of this module (Open/Close/etc.) follows
 * the standard POSIX way of doing things although the code itself 
 * is not using the POSIX calls on some platforms (Windows, so far).
 *
 * \ingroup vdscCommon
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

/** Unique identifier for the vdscDirIterator struct. */
#define VDSC_DIR_ACCESS_SIGNATURE ((unsigned)0xca3dd33e)

/**
 * \cond NO_DIR_DEFINES_PLEASE
 * This should stop doxygen from including the define below.
 */
#if ! defined (WIN32)

#  if HAVE_DIRENT_H
#    include <dirent.h>
#    define NAMLEN(dirent) strlen((dirent)->d_name)
#  else
#    define dirent direct
#    define NAMLEN(dirent) (dirent)->d_namlen
#    if HAVE_SYS_NDIR_H
#      include <sys/ndir.h>
#    endif
#    if HAVE_SYS_DIR_H
#      include <sys/dir.h>
#    endif
#    if HAVE_NDIR_H
#      include <ndir.h>
#    endif
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

struct vdscDirIterator
{
   /** Set to VDSC_DIR_ACCESS_SIGNATURE at initialization. */
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

typedef struct vdscDirIterator vdscDirIterator;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*! \brief Initialize the struct vdscDirIterator. */
void vdscInitDir( vdscDirIterator * pIterator );

/*! \brief Terminate access to the struct vdscDirIterator. */
void vdscFiniDir( vdscDirIterator * pIterator );

/*! \brief Open the directory. */
int vdscOpenDir( vdscDirIterator*  pIterator,
                 const char*       dirName,
                 vdscErrorHandler* pError );

/*! \brief Get the next file name. */
const char* vdscDirGetNextFileName( vdscDirIterator*  pIterator,
                                    vdscErrorHandler* pError );

/*! \brief Close the directory. */
void vdscCloseDir( vdscDirIterator * pIterator );


/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * @}
 */

END_C_DECLS

#endif /* VDSC_DIR_ACCESS_H */

