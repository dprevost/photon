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
#  ifdef BUILD_VDSF
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
 * Maximum number of bytes of the name of a vds object (not counting the 
 * name of the parent folder(s)).
 */ 
#define VDS_MAX_NAME_LENGTH 256

/** 
 * Maximum number of bytes of the fully qualified name of a vds object 
 * (including the name(s) of its parent folder(s)).
 *
 * Note: setting this value eliminates a possible loophole since some
 * heap memory must be allocated to hold the wide characters string 
 * for the duration of the operation (open, close, create or destroy).
 */ 
#define VDS_MAX_FULL_NAME_LENGTH 1024

/** 
 * Maximum number of bytes of the name of a field of a vds object.
 */
#define VDS_MAX_FIELD_LENGTH 32

/** 
 * Maximum number of fields (including the last one).
 */
#define VDS_MAX_FIELDS 65535

#define VDS_FIELD_MAX_PRECISION 30

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * The object type as seen from the API
 */
enum vdsObjectType
{
   VDS_FOLDER   = 1,
   VDS_HASH_MAP = 2,
   VDS_LIFO     = 3,  /* A LIFO queue aka a stack */
   VDS_FAST_MAP = 4,  /* A read-only hash map */
   VDS_QUEUE    = 5,  /* A FIFO queue */
   VDS_LAST_OBJECT_TYPE
};

typedef enum vdsObjectType vdsObjectType;

enum vdsIteratorType
{
   VDS_FIRST = 1,
   VDS_NEXT  = 2
};

typedef enum vdsIteratorType vdsIteratorType;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * VDSF supported data types.
 */
enum vdsFieldType
{
   VDS_INTEGER = 1,
   VDS_BINARY,
   VDS_STRING,
   VDS_DECIMAL,
   VDS_BOOLEAN,
   /** Only valid for the last field of the data definition */
   VDS_VAR_BINARY,
   /** Only valid for the last field of the data definition */
   VDS_VAR_STRING
};

/**
 * VDSF supported data types for keys.
 */
enum vdsKeyType
{
   VDS_KEY_INTEGER = 101,
   VDS_KEY_BINARY,
   VDS_KEY_STRING,
   /** Only valid for the last field of the data definition */
   VDS_KEY_VAR_BINARY,
   /** Only valid for the last field of the data definition */
   VDS_KEY_VAR_STRING
};

/**
 * Description of the structure of the hash map key.
 */
struct vdsKeyDefinition
{
   enum vdsKeyType type;
   size_t length;
   size_t minLength;
   size_t maxLength;
};

typedef struct vdsKeyDefinition vdsKeyDefinition;

/**
 * Description of the structure of the data (if any).
 *
 * This structure is aligned in such a way that you can do:
 *
 *    malloc( offsetof(vdsObjectDefinition, fields) +
 *            numFields * sizeof(vdsFieldDefinition) );
 *
 */
struct vdsFieldDefinition
{
   char name[VDS_MAX_FIELD_LENGTH];
   enum vdsFieldType type;
   size_t length;
   size_t minLength;
   size_t maxLength;
   size_t precision;
   size_t scale;
};

typedef struct vdsFieldDefinition vdsFieldDefinition;

/**
 * This struct has a variable length.
 */
struct vdsObjectDefinition
{
   enum vdsObjectType type;

   unsigned int numFields;
   
   /** The data definition of the key (hash map only) */
   vdsKeyDefinition key;

   /** The data definition of the fields */
   vdsFieldDefinition fields[1];
};

typedef struct vdsObjectDefinition vdsObjectDefinition;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * \ingroup Folder
 *
 * This data structure is used to iterate throught all objects in a folder.
 *
 * Note: the actual name of an object (and the length of this name) might 
 * vary if you are using different locales (internally, names are stored as
 * wide characters (4 bytes)).
 */
struct vdsFolderEntry
{
   /** The object type */
   vdsObjectType type;
   
   /** Status (created but not committed, etc.) - not used in version 0.1 */
   int status;
   
   /** The actual length of the name of the object. */
   size_t nameLengthInBytes;
   
   /** The name of the object. */
   char name[VDS_MAX_NAME_LENGTH];
};

typedef struct vdsFolderEntry vdsFolderEntry;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This data structure is used to retrieve the status of objects.
 */
struct vdsObjStatus
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

   /** Maximum data length (in bytes). */
   size_t maxDataLength;
   
   /** Maximum key length (in bytes) if keys are supported - zero otherwise */
   size_t maxKeyLength;

};

typedef struct vdsObjStatus vdsObjStatus;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This data structure is used to retrieve the status of the virtual data space.
 */
struct vdsInfo
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
   
   /** VDS version */
   int memoryVersion;

   /** Endianess (0 for little endian, 1 for big endian) */
   int bigEndian;
   
   /** Compiler name */
   char compiler[20];

   /** Compiler version (if available) */
   char compilerVersion[10];
   
   /** Platform */
   char platform[20];
   
   /** Shared lib version */
   char dllVersion[10];
   
   /** The watchdog version (of the vds creator) */
   char watchdogVersion[10];
   
   /** Timestamp of creation of VDS */
   char creationTime[30];
};

typedef struct vdsInfo vdsInfo;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
}
#endif
  
#endif /* VDS_COMMON_H */
