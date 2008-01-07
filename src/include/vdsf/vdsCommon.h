/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of vdsf (Virtual Data Space Framework) Library.
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

#ifndef VDS_COMMON_H
#define VDS_COMMON_H

#include <stdlib.h>

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \def VDSF_EXPORT
 * 
 * Uses to tell the VC++ compiler to export/import a function or variable on
 * Windows (the macro is empty on other platforms).
 */
#if defined(WIN32)
#  ifdef VDSF_BUILD
#    define VDSF_EXPORT __declspec ( dllexport )
#  else
#    define VDSF_EXPORT __declspec ( dllimport )
#  endif
#else
#  define VDSF_EXPORT
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
extern "C" {
#endif
 
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * VDS_HANDLE is an opaque data type used by the C API to reference 
 * objects created in the API module.
 */
typedef void* VDS_HANDLE;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Maximum number of characters (or bytes if not supporting i18n) 
 * of the name of a vds object (not counting the name of the parent
 * folder(s)).
 *
 * If the software was compiled with i18n, this maximum is the number
 * of wide characters (4 bytes). Otherwise it is the number of bytes
 * (which should equal the number of characters unless something funny
 * is going on like using UTF-8 as locale and using ---disable-i18n with
 * configure...).
 */ 
#define VDS_MAX_NAME_LENGTH 256

/** 
 * Maximum number of characters (or bytes if not supporting i18n) 
 * of the fully qualified name of a vds object (including the name(s)
 * of its parent folder(s)).
 *
 * If the software was compiled with i18n, this maximum is the number
 * of wide characters (4 bytes). Otherwise it is the number of bytes
 * (which should equal the number of characters unless something funny
 * is going on like using UTF-8 as locale and using ---disable-i18n with
 * configure...).
 *
 * Note: setting this value eliminates a possible loophole since some
 * heap memory must be allocated to hold the wide characters string 
 * for the duration of the operation (open, close, create or destroy).
 */ 
#define VDS_MAX_FULL_NAME_LENGTH 1024

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * The object type as seen from the API
 */
typedef enum vdsObjectType
{
   VDS_FOLDER   = 1,
   VDS_QUEUE    = 2,
   VDS_HASH_MAP = 3,
   VDS_LAST_OBJECT_TYPE
} vdsObjectType;

typedef enum vdsIteratorType
{
   VDS_FIRST = 1,
   VDS_NEXT  = 2
   
} vdsIteratorType;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* See the discussion on i18n. */
/** 
 * \ingroup Folder
 *
 * This data structure is used to iterate throught all objects in a folder.
 *
 * Note: the actual name of an object (and the length of this name) might 
 * vary if you are using different locales (internally, names are stored as
 * wide characters (4 bytes)).
 */
typedef struct vdsFolderEntry
{
   /** The object type */
   vdsObjectType type;
   
   /** Status (created but not committed, etc.) - not used in version 0.1 */
   int status;
   
   /** The actual length of the name of the object. */
   size_t nameLengthInBytes;
   
   /** The name of the object. */
   char name[VDS_MAX_NAME_LENGTH*4];

} vdsFolderEntry;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This data structure is used to retrieve the status of objects.
 */
typedef struct vdsObjStatus
{
   /** The object type. */
   vdsObjectType type;

   /** Status (created but not committed, etc.) - not used in version 0.1 */
   int status;

   /** The number of blocks allocated to this object. */
   size_t numBlocks;

   /** The number of groups of blocks allocated to this object. */
   size_t numBlockGroup;

   /** The number of data items in thisa object. */
   size_t numDataItem;
   
   /** The amount of free space available in the blocks allocated to this object. */
   size_t freeBytes;

} vdsObjStatus;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This data structure is used to retrieve the status of the virtual data space.
 */
typedef struct vdsInfo
{
   /** Total size of the virtual data space. */
   size_t totalSizeInBytes;
   
   /** Total size of the allocated blocks. */
   size_t allocatedSizeInBytes;
   
   /** Number of API objects in the vds (internal objects are not counted). */
   size_t numObjects;
   
   /** Total number of groups of blocks. */
   size_t numGroups;
   
   /** Number of calls to allocate groups of blocks. */
   size_t numMallocs;
   
   /** Number of calls to free groups of blocks. */
   size_t numFrees;
   
   /** Largest contiguous group of free blocks. */
   size_t largestFreeInBytes;
   
} vdsInfo;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
}
#endif
  
#endif /* VDS_COMMON_H */
