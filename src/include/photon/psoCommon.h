/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@photonsoftware.org>
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

#ifndef PSO_COMMON_H
#define PSO_COMMON_H

#include <stdlib.h>

/*
 * We use our own type instead of one of the defined types to avoid
 * redefinition of the same symbol, either in building Photon itself
 * and possibly client code using Photon.
 */
#ifdef _MSC_VER
typedef unsigned __int32 psoUint32;
#else
#  include <stdint.h>
typedef uint32_t psoUint32;
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \def PHOTON_EXPORT
 * 
 * Uses to tell the VC++ compiler to export/import a function or variable on
 * Windows (the macro is empty on other platforms).
 */
#if defined(WIN32)
#  ifdef BUILD_PHOTON
#    define PHOTON_EXPORT __declspec ( dllexport )
#  else
#    define PHOTON_EXPORT __declspec ( dllimport )
#  endif
#else
#  define PHOTON_EXPORT
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
extern "C" {
#endif
 
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * PSO_HANDLE is an opaque data type used by the C API to reference 
 * objects created in the API module.
 */
typedef void * PSO_HANDLE;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Maximum number of bytes of the name of a pso object (not counting the 
 * name of the parent folder(s)).
 */ 
#define PSO_MAX_NAME_LENGTH 256

/** 
 * Maximum number of bytes of the fully qualified name of a pso object 
 * (including the name(s) of its parent folder(s)).
 *
 * Note: setting this value eliminates a possible loophole since some
 * heap memory must be allocated to hold the wide characters string 
 * for the duration of the operation (open, close, create or destroy).
 */ 
#define PSO_MAX_FULL_NAME_LENGTH 1024

/** 
 * Maximum number of bytes of the name of a field of a pso object.
 */
#define PSO_MAX_FIELD_LENGTH 32

/** 
 * Maximum number of fields (including the last one).
 */
#define PSO_MAX_FIELDS 65535

/**
 * Maximum size of a decimal field.
 */
#define PSO_FIELD_MAX_PRECISION 30

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * The object type as seen from the API
 */
enum psoObjectType
{
   PSO_FOLDER   = 1,
   PSO_HASH_MAP = 2,
   PSO_LIFO     = 3,  /* A LIFO queue aka a stack */
   PSO_FAST_MAP = 4,  /* A read-only hash map */
   PSO_QUEUE    = 5,  /* A FIFO queue */
   PSO_LAST_OBJECT_TYPE
};

typedef enum psoObjectType psoObjectType;

enum psoIteratorType
{
   PSO_FIRST = 1,
   PSO_NEXT  = 2
};

typedef enum psoIteratorType psoIteratorType;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Photon supported data types.
 */
enum psoFieldType
{
   PSO_INTEGER = 1,
   PSO_BINARY,
   PSO_STRING,
   /** The decimal type should be mapped to an array of bytes of length
    *  precision + 2 (optional sign and the decimal separator).
    */
   PSO_DECIMAL,
   /** The boolean type should be mapped to a single byte in a C struct. */
   PSO_BOOLEAN,
   /** Only valid for the last field of the data definition */
   PSO_VAR_BINARY,
   /** Only valid for the last field of the data definition */
   PSO_VAR_STRING
};

/**
 * Photon supported data types for keys.
 */
enum psoKeyType
{
   PSO_KEY_INTEGER = 101,
   PSO_KEY_BINARY,
   PSO_KEY_STRING,
   /** Only valid for the last field of the data definition */
   PSO_KEY_VAR_BINARY,
   /** Only valid for the last field of the data definition */
   PSO_KEY_VAR_STRING
};

/**
 * Description of the structure of the hash map key.
 */
struct psoKeyDefinition
{
   /** The data type of the key. */
   enum psoKeyType type;

   /** For fixed-length data types */
   psoUint32 length;

   /** For variable-length data types */
   psoUint32 minLength;

   /** For variable-length data types */
   psoUint32 maxLength;
};

typedef struct psoKeyDefinition psoKeyDefinition;

/**
 * Description of the structure of the data (if any).
 *
 * This structure is aligned in such a way that you can do:
 *
 *    malloc( offsetof(psoObjectDefinition, fields) +
 *            numFields * sizeof(psoFieldDefinition) );
 *
 */
struct psoFieldDefinition
{
   /** The name of the field. */
   char name[PSO_MAX_FIELD_LENGTH];
   
   /** The data type of the field/ */
   enum psoFieldType type;
   
   /** For fixed-length data types */
   psoUint32 length;

   /** For variable-length data types */
   psoUint32 minLength;

   /** For variable-length data types */
   psoUint32 maxLength;

   /** Total number of digits in the decimal field. */
   psoUint32 precision;

   /** Number of digits following the decimal separator. */
   psoUint32 scale;
};

typedef struct psoFieldDefinition psoFieldDefinition;

/**
 * This struct has a variable length.
 */
struct psoObjectDefinition
{
   /** The object type. */
   enum psoObjectType type;

   /** The number of fields in the definition. */
   psoUint32 numFields;
   
   /** The data definition of the key (hash map/fast map only) */
   psoKeyDefinition key;

   /** The data definition of the fields */
   psoFieldDefinition fields[1];
};

typedef struct psoObjectDefinition psoObjectDefinition;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * \ingroup psoFolder_c
 *
 * This data structure is used to iterate throught all objects in a folder.
 */
struct psoFolderEntry
{
   /** The name of the object. */
   char name[PSO_MAX_NAME_LENGTH];

   /** The object type */
   psoObjectType type;
   
   /**
    * Status of the object.
    *
    * For example, created but not committed, etc.
    */
   int status;
   
   /** The actual length (in bytes) of the name of the object. */
   psoUint32 nameLengthInBytes;
   
};

typedef struct psoFolderEntry psoFolderEntry;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This data structure is used to retrieve the status of objects.
 */
struct psoObjStatus
{
   /** The object type. */
   psoObjectType type;

   /**
    * Status of the object.
    *
    * For example, created but not committed, etc.
    */
   int status;

   /** The number of blocks allocated to this object. */
   size_t numBlocks;

   /** The number of groups of blocks allocated to this object. */
   size_t numBlockGroup;

   /** The number of data items in this object. */
   size_t numDataItem;
   
   /** The amount of free space available in the blocks allocated to this object. */
   size_t freeBytes;

   /** Maximum data length (in bytes). */
   psoUint32 maxDataLength;
   
   /** Maximum key length (in bytes) if keys are supported - zero otherwise */
   psoUint32 maxKeyLength;

};

typedef struct psoObjStatus psoObjStatus;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This data structure is used to retrieve the status of the Photon shared memory.
 */
struct psoInfo
{
   /** Total size of the virtual data space. */
   size_t totalSizeInBytes;
   
   /** Total size of the allocated blocks. */
   size_t allocatedSizeInBytes;
   
   /** Number of API objects in the pso (internal objects are not counted). */
   size_t numObjects;
   
   /** Total number of groups of blocks. */
   size_t numGroups;
   
   /** Number of calls to allocate groups of blocks. */
   size_t numMallocs;
   
   /** Number of calls to free groups of blocks. */
   size_t numFrees;
   
   /** Largest contiguous group of free blocks. */
   size_t largestFreeInBytes;
   
   /** Shared-memory version. */
   int memoryVersion;

   /** Endianess (0 for little endian, 1 for big endian). */
   int bigEndian;
   
   /** Compiler name. */
   char compiler[20];

   /** Compiler version (if available). */
   char compilerVersion[10];
   
   /** Platform. */
   char platform[20];
   
   /** Shared lib version. */
   char dllVersion[10];
   
   /** The server version (of the pso creator). */
   char quasarVersion[10];
   
   /** Timestamp of creation of the shared memory. */
   char creationTime[30];
};

typedef struct psoInfo psoInfo;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
}
#endif
  
#endif /* PSO_COMMON_H */
