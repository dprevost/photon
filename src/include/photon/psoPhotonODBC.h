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

/**
 * \file
 *
 * This file provides the basic data structures needed to implement 
 * the definition type PSO_DEF_PHOTON_ODBC_SIMPLE.
 */
 
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifndef PSO_PHOTON_ODBC_H
#define PSO_PHOTON_ODBC_H

#include <photon/psoCommon.h>

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
extern "C" {
#endif
 
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */


/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

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
 * Photon supported data types.
 */
enum psoFieldType
{
   PSO_CHAR = 101,
   PSO_VARCHAR,
   PSO_LONGVARCHAR,
   
   /** A one-byte integer. */
   PSO_TINYINT,
   
   /** A two-bytes integer. */
   PSO_SMALLINT,
   
   /** A four-bytes integer. */
   PSO_INTEGER,
   
   PSO_BIGINT,
   /** 
    * The decimal type should be mapped to the psoNumericStruct struct.
    */
   PSO_NUMERIC,
   
   PSO_REAL,
   PSO_DOUBLE,
   
   /** An opaque type of fixed length. */
   PSO_BINARY,

   /** A variable length opaque type with a maximum length. */
   PSO_VARBINARY,
   
   /**
    * A variable length opaque type with no maximum length constraint.
    *
    * The length of that field is constrained by the maximum length
    * of a data record.
    */
   /** Only valid for the last field of the data definition */
   PSO_LONGVARBINARY,
   
   PSO_DATE,
   PSO_TIME,
   PSO_TIMESTAMP
};

/**
 * Photon supported data types for keys.
 */
enum psoKeyType
{
   PSO_KEY_CHAR = 1001,
   PSO_KEY_VARCHAR,
   PSO_KEY_LONGVARCHAR,
   
   /** A four-bytes integer. */
   PSO_KEY_INTEGER,
   
   PSO_KEY_BIGINT,
   
   /** An opaque type of fixed length. */
   PSO_KEY_BINARY,

   /** A variable length opaque type with a maximum length. */
   PSO_KEY_VARBINARY,
   
   /**
    * A variable length opaque type with no maximum length constraint.
    *
    * The length of that field is constrained by the maximum length
    * of a data record.
    */
   /** Only valid for the last field of the data definition */
   PSO_KEY_LONGVARBINARY,
   
   PSO_KEY_DATE,
   PSO_KEY_TIME,
   PSO_KEY_TIMESTAMP
};

/**
 * Description of the structure of the hash map key.
 */
struct psoKeyFieldDefinition
{
   /** The name of a field of the key. */
   char name[PSO_MAX_FIELD_LENGTH];

   /** The data type of the key. */
   enum psoKeyType type;

   /** For some of the fixed and variable-length data types */
   psoUint32 length;

};

typedef struct psoKeyFieldDefinition psoKeyFieldDefinition;

/**
 * Description of the structure of the data.
 */
struct psoFieldDefinition
{
   /** The name of the field. */
   char name[PSO_MAX_FIELD_LENGTH];
   
   /** The data type of the field/ */
   enum psoFieldType type;
   
   union {
      /** For some of the fixed and variable-length data types */
      psoUint32 length;

      /** For numeric data */
      struct {
         /** Total number of digits in the decimal field. */
         psoUint16 precision;
         
         /** Number of digits following the decimal separator. */
         psoUint32 scale;

      } decimal;
      
   } vals;
};

typedef struct psoFieldDefinition psoFieldDefinition;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * The following structures should be compatible with the C data types
 * expected by ODBC drivers as described by the MSDN documentation.
 */
 
struct psoDateStruct {
   psoInt16  year;
   psoUint16 month;
   psoUint16 day;  
};

struct psoTimeStruct {
   psoUint16 hour;
   psoUint16 minute;
   psoUint16 second;
};

struct psoTimeStampStruct {
   psoInt16  year;
   psoUint16 month;
   psoUint16 day;
   psoUint16 hour;
   psoUint16 minute;
   psoUint16 second;
   psoUint32 fraction;
};

#define PSO_MAX_NUMERIC_LEN 8

struct psoNumericStruct {
   unsigned char precision;
   signed char   scale;
   unsigned char sign; // [g]
   unsigned char val[PSO_MAX_NUMERIC_LEN]; // [e], [f] 
};

#ifdef __cplusplus
}
#endif
  
#endif /* PSO_PHOTON_ODBC_H */
