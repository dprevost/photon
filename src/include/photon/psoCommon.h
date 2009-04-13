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

#ifndef PSO_COMMON_H
#define PSO_COMMON_H

#include <stdlib.h>

/*
 * We use our own type instead of one of the defined types to avoid
 * redefinition of the same symbol.
 *
 * In other words, typedef unsigned __int32 uint32_t; on windows might 
 * lead to compilation errors if uint32_t is already defined by the
 * client program.
 */
#ifdef _MSC_VER
typedef          __int16 psoInt16;
typedef unsigned __int16 psoUint16;
typedef unsigned __int32 psoUint32;
#else
#  include <stdint.h>
typedef  int16_t psoInt16;
typedef uint16_t psoUint16;
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
 */ 
#define PSO_MAX_FULL_NAME_LENGTH 1024

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * The object type as seen from the API
 */
enum psoObjectType
{
   PSO_FOLDER   = 1,
   PSO_LIFO     = 2,  /* A LIFO queue aka a stack */
   PSO_QUEUE    = 3,  /* A FIFO queue */
   PSO_HASH_MAP = 4,
   PSO_FAST_MAP = 5,  /* A read-only hash map */
   PSO_LAST_OBJECT_TYPE
};

typedef enum psoObjectType psoObjectType;

enum psoIteratorType
{
   PSO_FIRST = 1,
   PSO_NEXT  = 2
};

typedef enum psoIteratorType psoIteratorType;

#define PSO_MULTIPLE_DATA_DEFINITIONS 0x0001

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Photon supported definitions for both keys and data fields.
 *
 * Definitions are not used by the core engine - the engine manipulates both
 * the keys and the data records as arrays of bytes. 
 *
 * They are however essential at the interface level, mainly for object 
 * oriented languages, to transform your objects to an array of bytes and back.
 *
 * Definitions can be divided in two categories, one where all the data fields
 * have a fixed length (the last field can vary in size). This is ideal for
 * mapping a data record (or key) to C and COBOL and can also be used by C

 and one where the
 * length of a data field is unknown (a Java string, for example). 
 *
 * Photon fully supports the first type natively by providing 
 */
enum psoDefinitionType
{
   /** First type (for boundary checking) */
   PSO_DEF_FIRST_TYPE = 1,

   /** No type (no key type for queues, for example) */
   PSO_DEF_NONE,
   
   /** User-defined */
   PSO_DEF_USER_DEFINED,
   /**
    * A simplified version of ODBC. 
    * 
    * The fields must all have a fixed length except for the last one. 
    * This condition makes it easy to map the data record with a C struct.
    */
   PSO_DEF_PHOTON_ODBC_SIMPLE,
   /** Google Protocol Buffer */
   PSO_DEF_PROTO_BUF,
   /** Last type (for boundary checking) */
   PSO_DEF_LAST_TYPE
};

/**
 * This struct contains the basic elements defining an object (type, 
 * number of fields, etc).
 *
 * This struct will be expanded in the near future to add hints to the
 * framework for better utilization of resources (cpu, RAM).
 *
 * For exmple: the expected minimal number of data records can be used 
 * to preallocate the internal hash array (avoiding unneeded Resize() and
 * potential memory fragmentation).
 */
struct psoObjectDefinition
{
   /** The object type. */
   enum psoObjectType type;

   /**
    * Flags defining the properties of the object.
    *
    * Currently defined flags:
    *     - PSO_MULTIPLE_DATA_DEFINITIONS
    *
    * Flags should be ORed.
    */
   psoUint32 flags;
   
   /**
    * Optimization feature - not implemented yet.
    *
    * The expected minimum number of data records. This is used internally
    * to avoid shrinking the internal "holder" of the data beyond what is
    * needed to hold this minimum number of data records.
    */
   size_t minNumOfDataRecords;
   
   /**
    * Optimization feature - not implemented yet
    *
    * The expected minimum number of blocks. This is used internally
    * to avoid shrinking the shared-memory object beyond a certain predefined
    * minimum size. 
    *
    * Potential issue: the amount of overhead used by Photon will vary;
    * some potential factors includes the type of object, the number of 
    * data records, the length of the data records (and keys, if used).
    *
    * You might want to retrieve the status of the object and evaluate
    * the minimum number of blocks needed from it..
    */
   size_t minNumBlocks;
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
 * This data structure is used to retrieve the status of the Photon 
 * shared memory.
 */
struct psoInfo
{
   /** Total size of the virtual data space. */
   size_t totalSizeInBytes;
   
   /** Total size of the allocated blocks. */
   size_t allocatedSizeInBytes;
   
   /** 
    * Number of API objects in the Photon shared memory.
    *
    * Internal objects are not counted. 
    */
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
   
   /** The version of quasar used to create the shared memory. */
   char quasarVersion[10];
   
   /** Timestamp of the creation of the shared memory. */
   char creationTime[30];
};

typedef struct psoInfo psoInfo;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
}
#endif
  
#endif /* PSO_COMMON_H */
