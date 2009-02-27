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
 * the definition type PSO_DEF_PHOTON_ODBC.
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

   /** The number of fields in the definition. */
//   psoUint32 numFields;

/**
 * Description of the structure of the data.
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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
}
#endif
  
#endif /* PSO_PHOTON_ODBC_H */
