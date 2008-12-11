/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@photonsoftware.org>
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

#include "Common/Common.h"
#include "API/DataDefinition.h"

#include <libxml/xmlschemas.h>
#include <libxml/xmlschemastypes.h>

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static void dummyErrorFunc( void * ctx, const char * msg, ...)
{
   ctx = ctx;
   msg = msg;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* 
 * Note: the type of object must be filled by the caller.
 */
int psoaGetDefinition( psonFieldDef       * pInternalDef,
                       uint16_t             numFields,
                       psoFieldDefinition * pFields )
{
   unsigned int i;
   
   PSO_PRE_CONDITION( pInternalDef != NULL );
   PSO_PRE_CONDITION( pFields      != NULL );
   PSO_PRE_CONDITION( numFields > 0 );

   for ( i = 0; i < numFields; ++i ) {

      pFields[i].type = pInternalDef[i].type;
      memcpy( pFields[i].name, pInternalDef[i].name, PSO_MAX_FIELD_LENGTH );

      switch( pInternalDef[i].type ) {

      case PSO_BINARY:
      case PSO_STRING:
      case PSO_INTEGER:
         pFields[i].length = pInternalDef[i].length1;
         
         break;

      case PSO_DECIMAL:
         pFields[i].precision = pInternalDef[i].length1;
         pFields[i].scale     = pInternalDef[i].length2;

         break;

      case PSO_BOOLEAN:

         break;

      case PSO_VAR_BINARY:
      case PSO_VAR_STRING:

         pFields[i].minLength = pInternalDef[i].length1;
         pFields[i].maxLength = pInternalDef[i].length2;

         break;
      }
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psoaGetKeyLimits( psoKeyDefinition * pKeyDef,
                       uint32_t         * pMinLength,
                       uint32_t         * pMaxLength )
{
   PSO_PRE_CONDITION( pKeyDef    != NULL );
   PSO_PRE_CONDITION( pMinLength != NULL );
   PSO_PRE_CONDITION( pMaxLength != NULL );

   if ( pKeyDef->type == PSO_KEY_INTEGER ||
      pKeyDef->type == PSO_KEY_BINARY ||
      pKeyDef->type == PSO_KEY_STRING ) {
      *pMinLength = *pMaxLength = pKeyDef->length;
   }
   else {
      /* PSO_KEY_VAR_BINARY || PSO_VAR_STRING */
      *pMinLength = pKeyDef->minLength;
      *pMaxLength = pKeyDef->maxLength;
      if ( *pMaxLength == 0 ) *pMaxLength = (uint32_t) -1;
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psoaGetLimits( psonFieldDef * pDefinition,
                    uint16_t       numFields,
                    uint32_t     * pMinLength,
                    uint32_t     * pMaxLength )
{
   unsigned int i;
   uint32_t minLength = 0, maxLength = 0;

   PSO_PRE_CONDITION( pDefinition != NULL );
   PSO_PRE_CONDITION( pMinLength  != NULL );
   PSO_PRE_CONDITION( pMaxLength  != NULL );
   PSO_PRE_CONDITION( numFields > 0 );
   
   /*
    * The first field is special - the alignment offset is always zero
    * since we just started.
    */
   switch( pDefinition[0].type ) {

   case PSO_INTEGER:
      minLength = pDefinition[0].length1;
      break;

   case PSO_BINARY:
   case PSO_STRING:
      minLength = pDefinition[0].length1;
      break;

   case PSO_DECIMAL:
      minLength = pDefinition[0].length1 + 2;
      break;

   case PSO_BOOLEAN:
      minLength = sizeof(bool);
      break;

   case PSO_VAR_BINARY:
   case PSO_VAR_STRING:
      minLength = pDefinition[0].length1;
      maxLength = pDefinition[0].length2;
      if ( maxLength == 0 ) maxLength = (uint32_t) -1;
      break;
   }
   
   for ( i = 1; i < numFields; ++i ) {

      switch( pDefinition[i].type ) {

      case PSO_INTEGER:
         if ( pDefinition[i].length1 == 1 ) {
            minLength = ((minLength-1)/PSOC_ALIGNMENT_CHAR + 1)*PSOC_ALIGNMENT_CHAR;
         }
         else if ( pDefinition[i].length1 == 2 ) {
            minLength = ((minLength-1)/PSOC_ALIGNMENT_INT16 + 1)*PSOC_ALIGNMENT_INT16;
         }
         else if ( pDefinition[i].length1 == 4 ) {
            minLength = ((minLength-1)/PSOC_ALIGNMENT_INT32 + 1)*PSOC_ALIGNMENT_INT32;
         }
         else {
            minLength = ((minLength-1)/PSOC_ALIGNMENT_INT64 + 1)*PSOC_ALIGNMENT_INT64;
         }
         
         minLength += pDefinition[i].length1;

         break;

      case PSO_BINARY:
      case PSO_STRING:
         minLength = ((minLength-1)/PSOC_ALIGNMENT_CHAR + 1)*PSOC_ALIGNMENT_CHAR;
         minLength += pDefinition[i].length1;

         break;

      case PSO_DECIMAL:
         minLength = ((minLength-1)/PSOC_ALIGNMENT_CHAR + 1)*PSOC_ALIGNMENT_CHAR;
         minLength += pDefinition[i].length1 + 2;

         break;

      case PSO_BOOLEAN:
         minLength = ((minLength-1)/PSOC_ALIGNMENT_BOOL + 1)*PSOC_ALIGNMENT_BOOL;
         minLength += sizeof(bool);
         break;

      case PSO_VAR_BINARY:
      case PSO_VAR_STRING:
         minLength = ((minLength-1)/PSOC_ALIGNMENT_CHAR + 1)*PSOC_ALIGNMENT_CHAR;
         minLength += pDefinition[i].length1;

         if ( pDefinition[i].length2 == 0 ||
            pDefinition[i].length2 >= ((uint32_t) -1) - minLength ) {
            maxLength = (uint32_t) -1;
         }
         else {
            maxLength = minLength + pDefinition[i].length2;
         }
         
         break;
      }
   }
   
   if ( maxLength == 0 ) maxLength = minLength;
   *pMinLength = minLength;
   *pMaxLength = maxLength;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psoaGetOffsets( psoObjectDefinition * pDefinition,
                     psoFieldDefinition  * pFields,
                     uint32_t            * pOffsets )
{
   unsigned int i;
   uint32_t minLength = 0;

   PSO_PRE_CONDITION( pDefinition != NULL );
   PSO_PRE_CONDITION( pFields     != NULL );
   PSO_PRE_CONDITION( pOffsets    != NULL );
   
   /*
    * The first field is special - the alignment offset is always zero
    * since we just started.
    */
   pOffsets[0] = 0;

   switch( pFields[0].type ) {

   case PSO_INTEGER:
   case PSO_BINARY:
   case PSO_STRING:
      minLength = pFields[0].length;
      break;

   case PSO_DECIMAL:
      minLength = pFields[0].precision + 2;
      break;

   case PSO_BOOLEAN:
      minLength = sizeof(bool);
      break;

   case PSO_VAR_BINARY:
   case PSO_VAR_STRING:
      /* A single field - a single offset ! */
      break;
   }
   
   for ( i = 1; i < pDefinition->numFields; ++i ) {

      switch( pFields[i].type ) {

      case PSO_INTEGER:
         if ( pFields[i].length == 1 ) {
            minLength = ((minLength-1)/PSOC_ALIGNMENT_CHAR + 1)*PSOC_ALIGNMENT_CHAR;
         }
         else if ( pFields[i].length == 2 ) {
            minLength = ((minLength-1)/PSOC_ALIGNMENT_INT16 + 1)*PSOC_ALIGNMENT_INT16;
         }
         else if ( pFields[i].length == 4 ) {
            minLength = ((minLength-1)/PSOC_ALIGNMENT_INT32 + 1)*PSOC_ALIGNMENT_INT32;
         }
         else {
            minLength = ((minLength-1)/PSOC_ALIGNMENT_INT64 + 1)*PSOC_ALIGNMENT_INT64;
         }
         pOffsets[i] = minLength;
         
         minLength += pFields[i].length;

         break;

      case PSO_BINARY:
      case PSO_STRING:
         minLength = ((minLength-1)/PSOC_ALIGNMENT_CHAR + 1)*PSOC_ALIGNMENT_CHAR;
         pOffsets[i] = minLength;

         minLength += pFields[i].length;

         break;

      case PSO_DECIMAL:
         minLength = ((minLength-1)/PSOC_ALIGNMENT_CHAR + 1)*PSOC_ALIGNMENT_CHAR;
         pOffsets[i] = minLength;

         minLength += pFields[i].precision + 2;

         break;

      case PSO_BOOLEAN:
         minLength = ((minLength-1)/PSOC_ALIGNMENT_BOOL + 1)*PSOC_ALIGNMENT_BOOL;
         pOffsets[i] = minLength;

         minLength += sizeof(bool);
         break;

      case PSO_VAR_BINARY:
      case PSO_VAR_STRING:
         minLength = ((minLength-1)/PSOC_ALIGNMENT_CHAR + 1)*PSOC_ALIGNMENT_CHAR;
         pOffsets[i] = minLength;

         break;
      }
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaValidateDefinition( psoObjectDefinition * pDefinition,
                            psoFieldDefinition  * pFields )
{
   unsigned int i, j;
   
   PSO_PRE_CONDITION( pDefinition != NULL );
   if ( pDefinition->type != PSO_FOLDER ) {
      PSO_PRE_CONDITION( pFields  != NULL );
   }
   
   switch( pDefinition->type ) {

   case PSO_FOLDER:
      return 0;

   case PSO_HASH_MAP:
   case PSO_FAST_MAP:

      /* We do the key and let the queue case test the rest */
      switch( pDefinition->key.type ) {

      case PSO_KEY_INTEGER:
         if ( pDefinition->key.length != 1 &&
              pDefinition->key.length != 2 &&
              pDefinition->key.length != 4 &&
              pDefinition->key.length != 8 ) {
            return PSO_INVALID_FIELD_LENGTH_INT;
         }
         break;

      case PSO_KEY_BINARY:
      case PSO_KEY_STRING:
         if ( pDefinition->key.length == 0 ) {
            return PSO_INVALID_FIELD_LENGTH;
         }
         break;

      case PSO_KEY_VAR_BINARY:
      case PSO_KEY_VAR_STRING:
         if ( pDefinition->key.minLength == 0 ) {
            return PSO_INVALID_FIELD_LENGTH;
         }
         if ( pDefinition->key.maxLength != 0 ) {
            if ( pDefinition->key.minLength > 
                 pDefinition->key.maxLength ) {
               return PSO_INVALID_FIELD_LENGTH;
            }
         }
         break;

      default:
         return PSO_INVALID_FIELD_TYPE;
      }

   case PSO_QUEUE:
   case PSO_LIFO:
      if ( pDefinition->numFields <= 0 || 
           pDefinition->numFields > PSO_MAX_FIELDS ) {
         return PSO_INVALID_NUM_FIELDS;
      }

      for ( i = 0; i < pDefinition->numFields; ++i ) {
         switch( pFields[i].type ) {

         case PSO_INTEGER:
            if ( pFields[i].length != 1 &&
                 pFields[i].length != 2 &&
                 pFields[i].length != 4 &&
                 pFields[i].length != 8 ) {
               return PSO_INVALID_FIELD_LENGTH_INT;
            }
            break;

         case PSO_BINARY:
         case PSO_STRING:
            if ( pFields[i].length == 0 ) {
               return PSO_INVALID_FIELD_LENGTH;
            }
            break;

         case PSO_DECIMAL:
            if ( pFields[i].precision == 0 ||
               pFields[i].precision > PSO_FIELD_MAX_PRECISION ) {
               return PSO_INVALID_PRECISION;
            }
            if ( pFields[i].scale > 
                 pFields[i].precision ) {
               return PSO_INVALID_SCALE;
            }
            break;

         case PSO_BOOLEAN:
            break;

         case PSO_VAR_BINARY:
         case PSO_VAR_STRING:
            /* These 2 types are only valid for the last field. */
            if ( i != (pDefinition->numFields-1) ) return PSO_INVALID_FIELD_TYPE;

            /* BIG WARNING: this rule is not captured by the XML schema */
            if ( pDefinition->numFields == 1 && 
                 pFields[i].minLength == 0 ) {
               return PSO_INVALID_FIELD_LENGTH;
            }
            /*
             * Reminder: maxLength set to zero indicates the maximum value
             * allowed which is 4294967295.
             */
            if ( pFields[i].maxLength != 0 ) {
               if ( pFields[i].minLength > pFields[i].maxLength ) {
                  return PSO_INVALID_FIELD_LENGTH;
               }
            }
            break;

         default:
            return PSO_INVALID_FIELD_TYPE;
         }
         /*
          * Field names must only allow characters that can be used to build 
          * c/c++ structs: they must start with a letter, and be limited to 
          * ASCII alphanumerics or '_'.
          *
          * The C99 standard allows for more than this (part of unicode 
          * defined in ISO 10646) but not all compilers implement it yet, for 
          * example VS2008:
          *     http://msdn.microsoft.com/en-us/library/565w213d.aspx.
          */
         if ( ! isalpha(pFields[i].name[0]) ) {
            return PSO_INVALID_FIELD_NAME;
         }
         for ( j = 1; j < PSO_MAX_FIELD_LENGTH; ++j ) {
            if ( pFields[i].name[j] == 0 ) break;
            if ( isalnum(pFields[i].name[j]) ) continue;
            if ( pFields[i].name[j] == '_' ) continue;
            
            return PSO_INVALID_FIELD_NAME;
         }

         /* We must also make sure that field names are not duplicate. */
         for ( j = 0; j < i; ++j ) {
            if ( strncmp( pFields[i].name,
                          pFields[j].name,
                          PSO_MAX_FIELD_LENGTH ) == 0 ) {
               return PSO_DUPLICATE_FIELD_NAME;
            }
         }
      }
      return 0;
      
   default:
      return PSO_WRONG_OBJECT_TYPE;
   }

}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaXmlToDefinition( const char           * xmlBuffer,
                         uint32_t               lengthInBytes,
                         psoObjectDefinition  * pDefinition,
                         psoFieldDefinition  ** ppFields,                         
                         char                ** objectName,
                         uint32_t             * nameLengthInBytes )
{
   xmlSchemaPtr schema = NULL;
   xmlSchemaValidCtxtPtr  validCtxt = NULL;
   xmlSchemaParserCtxtPtr parserCtxt = NULL;
   xmlNode * root = NULL, * nodeField, * nodeType, *nodeKey = NULL;
   xmlDoc  * doc = NULL;
   xmlChar * prop = NULL;
   int errcode = 0;
   int i, j, separator = -1;
   uint16_t numFields;
   bool dynamicMode;
   
   PSO_PRE_CONDITION( xmlBuffer         != NULL );
   PSO_PRE_CONDITION( pDefinition       != NULL );
   PSO_PRE_CONDITION( ppFields          != NULL );
   PSO_PRE_CONDITION( objectName        != NULL );
   PSO_PRE_CONDITION( nameLengthInBytes != NULL );

   *ppFields = NULL;
   
   /*
    * for debugging, I could use this instead:
    * doc = xmlReadMemory( buf, i, NULL, NULL, 0 );
    */
   doc = xmlReadMemory( xmlBuffer, lengthInBytes, NULL, NULL, 
                        XML_PARSE_NOERROR | XML_PARSE_NOWARNING );
   if ( doc == NULL ) {
      errcode = PSO_XML_READ_ERROR;
      goto cleanup;
   }
   
   root = xmlDocGetRootElement( doc );
   if ( root == NULL ) {
      errcode = PSO_XML_INVALID_ROOT;
      goto cleanup;
   }
   if ( (xmlStrcmp(root->name, BAD_CAST "folder")  != 0) &&
        (xmlStrcmp(root->name, BAD_CAST "queue")   != 0) &&
        (xmlStrcmp(root->name, BAD_CAST "lifo")    != 0) &&
        (xmlStrcmp(root->name, BAD_CAST "hashmap") != 0) ) {
      errcode = PSO_XML_INVALID_ROOT;
      goto cleanup;
   }

   prop = xmlGetProp( root, BAD_CAST "schemaLocation" );
   if ( prop == NULL ) {
      errcode = PSO_XML_NO_SCHEMA_LOCATION;
      goto cleanup;
   }
   
   for ( i = 0; i < xmlStrlen(prop)-1; ++i ) {
      if ( isspace(prop[i]) ) {
         for ( j = i+1; j < xmlStrlen(prop)-1; ++j ) {
            if ( isspace(prop[j]) == 0 ) {
               separator = j;
               break;
            }
         }
         break;
      }
   }
   if ( separator == -1 ) {
      errcode = PSO_XML_NO_SCHEMA_LOCATION;
      goto cleanup;
   }
   
   parserCtxt = xmlSchemaNewParserCtxt( (char*)&prop[separator] );
   if ( parserCtxt == NULL ) {
      errcode = PSO_XML_PARSER_CONTEXT_FAILED;
      goto cleanup;
   }
   
   schema = xmlSchemaParse( parserCtxt );
   if ( schema == NULL ) {
      errcode = PSO_XML_PARSE_SCHEMA_FAILED;
      goto cleanup;
   }
   
   xmlFree( prop );
   prop = NULL;

   validCtxt = xmlSchemaNewValidCtxt( schema );
   if ( validCtxt == NULL ) {
      errcode = PSO_XML_VALID_CONTEXT_FAILED;
      goto cleanup;
   }
   
   /*
    * for debugging, I could use this instead:
    *
    *  xmlSchemaSetValidErrors( validCtxt,
    *                           (xmlSchemaValidityErrorFunc) fprintf,
    *                           (xmlSchemaValidityWarningFunc) fprintf,
    *                           stderr );
    */
   xmlSchemaSetValidErrors( validCtxt,
                            (xmlSchemaValidityErrorFunc) dummyErrorFunc,
                            (xmlSchemaValidityWarningFunc) dummyErrorFunc,
                            stderr );
   
   if ( xmlSchemaValidateDoc( validCtxt, doc ) != 0 ) {
      errcode = PSO_XML_VALIDATION_FAILED;
      goto cleanup;
   }

   prop = xmlGetProp( root, BAD_CAST "objName" );
   if ( prop == NULL ) {
      errcode = PSO_INVALID_OBJECT_NAME;
      goto cleanup;
   }
   *objectName = (char*)malloc( xmlStrlen(prop) + 1 );
   if ( *objectName == NULL ) {
      errcode = PSO_NOT_ENOUGH_HEAP_MEMORY;
      goto cleanup;
   }
   strcpy( *objectName, (char *)prop );
   *nameLengthInBytes = strlen( *objectName );
   xmlFree( prop );
   prop = NULL;
   
   if ( xmlStrcmp( root->name, BAD_CAST "folder") == 0 ) {
      pDefinition->type = PSO_FOLDER;
      /* No fields to process. Go directly to the exit. */
      goto cleanup;
   }

   /* A dynamic or static hash map ? */
   dynamicMode = false;
   if ( xmlStrcmp( root->name, BAD_CAST "hashmap") == 0 ) {
      prop = xmlGetProp( root, BAD_CAST "mode" );
      if ( prop == NULL ) {
         errcode = PSO_WRONG_OBJECT_TYPE;
         goto cleanup;
      }
      if ( xmlStrcmp( prop, BAD_CAST "dynamic" ) == 0 ) {
         dynamicMode = true;
      }
      xmlFree( prop );
      prop = NULL;
   }
   
   /* All seems ok with validation - start the extraction of our data */   
   nodeField = root->children;

   /*
    * Hash map definition starts with a single key field at the beginning 
    * but we have to wait until the definition is allocated before we can 
    * process it.
    */
   if ( xmlStrcmp( root->name, BAD_CAST "hashmap") == 0 ) {
      while ( nodeField != NULL ) {
         if ( nodeField->type == XML_ELEMENT_NODE ) {
            if ( xmlStrcmp( nodeField->name, BAD_CAST "key") == 0 ) {
               nodeKey = nodeField;
               /* Advance one so that the key is not counted in numFields */
               nodeField = nodeField->next;
            }
            else {
               errcode = PSO_INVALID_KEY_DEF;
               goto cleanup;
            }
            break;
         }
         nodeField = nodeField->next;
      }
   }

   /* Prepare the memory allocation */
   numFields = 0;
   while ( nodeField != NULL ) {
      if ( nodeField->type == XML_ELEMENT_NODE ) numFields++;
      nodeField = nodeField->next;
   }

   *ppFields = (psoFieldDefinition *)
      calloc( numFields * sizeof(psoFieldDefinition), 1 );
   if (*ppFields == NULL ) {
      errcode = PSO_NOT_ENOUGH_HEAP_MEMORY;
      goto cleanup;
   }
   pDefinition->numFields = numFields;

   /* Extract the key, if any */
   if ( nodeKey != NULL ) {
      pDefinition->type = PSO_FAST_MAP;
      if ( dynamicMode ) pDefinition->type = PSO_HASH_MAP;
      nodeType = nodeKey->children;
      while ( nodeType != NULL ) {
         if ( nodeType->type == XML_ELEMENT_NODE ) {
            if ( xmlStrcmp( nodeType->name, BAD_CAST "integer") == 0 ) {
               prop = xmlGetProp( nodeType, BAD_CAST "size" );
               if ( prop == NULL ) {
                  errcode = PSO_INVALID_KEY_DEF;
                  goto cleanup;
               }
               pDefinition->key.type = PSO_KEY_INTEGER;
               sscanf( (char*)prop, "%ud", &pDefinition->key.length );
               xmlFree(prop);
               prop = NULL;
            }
            else if ( xmlStrcmp( nodeType->name, BAD_CAST "string") == 0 ) {
               prop = xmlGetProp( nodeType, BAD_CAST "length" );
               if ( prop == NULL ) {
                  errcode = PSO_INVALID_KEY_DEF;
                  goto cleanup;
               }
               pDefinition->key.type = PSO_KEY_STRING;
               sscanf( (char*)prop, "%ud", &pDefinition->key.length );
               xmlFree(prop);
               prop = NULL;
            }
            else if ( xmlStrcmp( nodeType->name, BAD_CAST "binary") == 0 ) {
               prop = xmlGetProp( nodeType, BAD_CAST "length" );
               if ( prop == NULL ) {
                  errcode = PSO_INVALID_KEY_DEF;
                  goto cleanup;
               }
               pDefinition->key.type = PSO_KEY_BINARY;
               sscanf( (char*)prop, "%ud", &pDefinition->key.length );
               xmlFree(prop);
               prop = NULL;
            }
            else if ( xmlStrcmp( nodeType->name, BAD_CAST "varString") == 0 ) {
               prop = xmlGetProp( nodeType, BAD_CAST "minLength" );
               if ( prop == NULL ) {
                  errcode = PSO_INVALID_KEY_DEF;
                  goto cleanup;
               }
               sscanf( (char*)prop, "%ud", &pDefinition->key.minLength );
               xmlFree(prop);
               prop = xmlGetProp( nodeType, BAD_CAST "maxLength" );
               if ( prop == NULL ) {
                  errcode = PSO_INVALID_KEY_DEF;
                  goto cleanup;
               }
               sscanf( (char*)prop, "%ud", &pDefinition->key.maxLength );
               xmlFree(prop);               
               prop = NULL;
               pDefinition->key.type = PSO_KEY_VAR_STRING;
            }
            else if ( xmlStrcmp( nodeType->name, BAD_CAST "varBinary") == 0 ) {
               prop = xmlGetProp( nodeType, BAD_CAST "minLength" );
               if ( prop == NULL ) {
                  errcode = PSO_INVALID_KEY_DEF;
                  goto cleanup;
               }
               sscanf( (char*)prop, "%ud", &pDefinition->key.minLength );
               xmlFree(prop);
               prop = xmlGetProp( nodeType, BAD_CAST "maxLength" );
               if ( prop == NULL ) {
                  errcode = PSO_INVALID_KEY_DEF;
                  goto cleanup;
               }
               sscanf( (char*)prop, "%ud", &pDefinition->key.maxLength );
               xmlFree(prop);               
               prop = NULL;
               pDefinition->key.type = PSO_KEY_VAR_BINARY;
            }
            else {
               errcode = PSO_INVALID_KEY_DEF;
               goto cleanup;
            }
            
            break;
         }
         nodeType = nodeType->next;
      }
      nodeField = nodeKey->next;
   }
   else {
      nodeField = root->children;
      if ( xmlStrcmp( root->name, BAD_CAST "queue") == 0 ) {
         pDefinition->type = PSO_QUEUE;
      }
      else if ( xmlStrcmp( root->name, BAD_CAST "lifo") == 0 ) {
         pDefinition->type = PSO_LIFO;
      }
      else {
         errcode = PSO_WRONG_OBJECT_TYPE;
         goto cleanup;
      }
   }

   numFields = 0;
   while ( nodeField != NULL ) {
      if ( nodeField->type == XML_ELEMENT_NODE ) {
         prop = xmlGetProp( nodeField, BAD_CAST "name" );
         if ( prop == NULL ) {
            errcode = PSO_INVALID_FIELD_NAME;
            goto cleanup;
         }
         strncpy( (*ppFields)[numFields].name, 
            (char*)prop, PSO_MAX_FIELD_LENGTH );
         xmlFree(prop);
         prop = NULL;
         
         nodeType = nodeField->children;
         while ( nodeType != NULL ) {
            if ( nodeType->type == XML_ELEMENT_NODE ) {
               
               if ( xmlStrcmp( nodeType->name, BAD_CAST "integer") == 0 ) {
                  prop = xmlGetProp( nodeType, BAD_CAST "size" );
                  if ( prop == NULL ) {
                     errcode = PSO_INVALID_FIELD_LENGTH_INT;
                     goto cleanup;
                  }
                  sscanf( (char*)prop, "%ud", 
                     &(*ppFields)[numFields].length );
                  xmlFree(prop);
                  prop = NULL;

                  (*ppFields)[numFields].type = PSO_INTEGER;
               }
               else if ( xmlStrcmp( nodeType->name, BAD_CAST "boolean") == 0 ) {
                  (*ppFields)[numFields].type = PSO_BOOLEAN;
               }
               else if ( (xmlStrcmp(nodeType->name, BAD_CAST "string") == 0) ||
                         (xmlStrcmp(nodeType->name, BAD_CAST "binary") == 0) ) {
                  prop = xmlGetProp( nodeType, BAD_CAST "length" );
                  if ( prop == NULL ) {
                     errcode = PSO_INVALID_FIELD_LENGTH;
                     goto cleanup;
                  }
                  sscanf( (char*)prop, "%ud", 
                     &(*ppFields)[numFields].length );
                  xmlFree(prop);
                  prop = NULL;
                  
                  if ( xmlStrcmp( nodeType->name, BAD_CAST "string") == 0 ) {
                     (*ppFields)[numFields].type = PSO_STRING;
                  }
                  else {
                     (*ppFields)[numFields].type = PSO_BINARY;
                  }
               }
               else if ( (xmlStrcmp(nodeType->name, BAD_CAST "varString") == 0) ||
                         (xmlStrcmp(nodeType->name, BAD_CAST "varBinary") == 0) ) {
                  if ( xmlStrcmp(nodeField->name, BAD_CAST "lastField") != 0 ) {
                     errcode = PSO_INVALID_FIELD_TYPE;
                     goto cleanup;
                  }
                  prop = xmlGetProp( nodeType, BAD_CAST "minLength" );
                  if ( prop == NULL ) {
                     errcode = PSO_INVALID_FIELD_LENGTH;
                     goto cleanup;
                  }
                  sscanf( (char*)prop, "%ud", 
                     &(*ppFields)[numFields].minLength );
                  xmlFree(prop);
                  prop = xmlGetProp( nodeType, BAD_CAST "maxLength" );
                  if ( prop == NULL ) {
                     errcode = PSO_INVALID_FIELD_LENGTH;
                     goto cleanup;
                  }
                  sscanf( (char*)prop, "%ud", 
                     &(*ppFields)[numFields].maxLength );
                  xmlFree(prop);
                  prop = NULL;
                  
                  if ( xmlStrcmp( nodeType->name, BAD_CAST "varString") == 0 ) {
                     (*ppFields)[numFields].type = PSO_VAR_STRING;
                  }
                  else {
                     (*ppFields)[numFields].type = PSO_VAR_BINARY;
                  }
               }
               else if ( xmlStrcmp(nodeType->name, BAD_CAST "decimal") == 0 ) {
                  prop = xmlGetProp( nodeType, BAD_CAST "precision" );
                  if ( prop == NULL ) {
                     errcode = PSO_INVALID_PRECISION;
                     goto cleanup;
                  }
                  sscanf( (char*)prop, "%ud", 
                     &(*ppFields)[numFields].precision );
                  xmlFree(prop);
                  prop = xmlGetProp( nodeType, BAD_CAST "scale" );
                  if ( prop == NULL ) {
                     errcode = PSO_INVALID_SCALE;
                     goto cleanup;
                  }
                  sscanf( (char*)prop, "%ud", 
                     &(*ppFields)[numFields].scale );
                  xmlFree(prop);
                  prop = NULL;
                  
                  (*ppFields)[numFields].type = PSO_DECIMAL;
               }
               else {
                  errcode = PSO_INVALID_FIELD_TYPE;
                  goto cleanup;
               }
               
               numFields++;
               break;
            }
            nodeType = nodeType->next;
         }

      }
      nodeField = nodeField->next;
   }

cleanup:
   
   if ( parserCtxt ) xmlSchemaFreeParserCtxt( parserCtxt );
   if ( schema ) xmlSchemaFree( schema );
   if ( validCtxt ) xmlSchemaFreeValidCtxt( validCtxt );
   if ( prop ) xmlFree( prop );
   if ( doc ) xmlFreeDoc( doc );

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

