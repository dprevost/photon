/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#include "Common/Common.h"
#include "API/DataDefinition.h"

#include <libxml/xmlschemas.h>
#include <libxml/xmlschemastypes.h>

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static void dummyErrorFunc( void * ctx, const char * msg, ...)
{
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* 
 * Note: the type of object must be filled by the caller.
 */
int vdsaGetDefinition( vdseFieldDef         * pInternalDef,
                       uint16_t               numFields,
                       vdsObjectDefinition ** ppDefinition )
{
   unsigned int i;
   vdsObjectDefinition * ptr;
   
   ptr = (vdsObjectDefinition *)calloc( offsetof(vdsObjectDefinition,fields) +
      numFields * sizeof(vdsFieldDefinition), 1 );
   if ( ptr == NULL ) return VDS_NOT_ENOUGH_HEAP_MEMORY;

   ptr->numFields = numFields;

   for ( i = 0; i < numFields; ++i ) {

      ptr->fields[i].type = pInternalDef[i].type;
      memcpy( ptr->fields[i].name, pInternalDef[i].name, VDS_MAX_FIELD_LENGTH );

      switch( pInternalDef[i].type ) {

      case VDS_BINARY:
      case VDS_STRING:
      case VDS_INTEGER:
         ptr->fields[i].length = pInternalDef[i].length1;
         
         break;

      case VDS_DECIMAL:
         ptr->fields[i].precision = pInternalDef[i].length1;
         ptr->fields[i].scale     = pInternalDef[i].length2;

         break;

      case VDS_BOOLEAN:

         break;

      case VDS_VAR_BINARY:
      case VDS_VAR_STRING:

         ptr->fields[i].minLength = pInternalDef[i].length1;
         ptr->fields[i].maxLength = pInternalDef[i].length2;

         break;
      }
   }

   *ppDefinition = ptr;
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdsaGetKeyLimits( vdsKeyDefinition * pKeyDef,
                       size_t           * pMinLength,
                       size_t           * pMaxLength )
{
   if ( pKeyDef->type == VDS_KEY_INTEGER ||
      pKeyDef->type == VDS_KEY_BINARY ||
      pKeyDef->type == VDS_KEY_STRING ) {
      *pMinLength = *pMaxLength = pKeyDef->length;
   }
   else {
      /* VDS_KEY_VAR_BINARY || VDS_VAR_STRING */
      *pMinLength = pKeyDef->minLength;
      *pMaxLength = pKeyDef->maxLength;
      if ( *pMaxLength == 0 ) *pMaxLength = 4294967295UL;
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdsaGetLimits( vdseFieldDef * pDefinition,
                    uint16_t       numFields,
                    size_t       * pMinLength,
                    size_t       * pMaxLength )
{
   unsigned int i;
   size_t minLength = 0, maxLength = 0;
   
   /*
    * The first field is special - the alignment offset is always zero
    * since we just started.
    */
   switch( pDefinition[0].type ) {

   case VDS_INTEGER:
      minLength = pDefinition[0].length1;
      break;

   case VDS_BINARY:
   case VDS_STRING:
      minLength = pDefinition[0].length1;
      break;

   case VDS_DECIMAL:
      minLength = pDefinition[0].length1 + 2;
      break;

   case VDS_BOOLEAN:
      minLength = sizeof(bool);
      break;

   case VDS_VAR_BINARY:
   case VDS_VAR_STRING:
      minLength = pDefinition[0].length1;
      maxLength = pDefinition[0].length2;
      if ( maxLength == 0 ) maxLength = 4294967295UL;
      break;
   }
   
   for ( i = 1; i < numFields; ++i ) {

      switch( pDefinition[i].type ) {

      case VDS_INTEGER:
         if ( pDefinition[i].length1 == 1 ) {
            minLength = ((minLength-1)/VDSC_ALIGNMENT_CHAR + 1)*VDSC_ALIGNMENT_CHAR;
         }
         else if ( pDefinition[i].length1 == 2 ) {
            minLength = ((minLength-1)/VDSC_ALIGNMENT_INT16 + 1)*VDSC_ALIGNMENT_INT16;
         }
         else if ( pDefinition[i].length1 == 4 ) {
            minLength = ((minLength-1)/VDSC_ALIGNMENT_INT32 + 1)*VDSC_ALIGNMENT_INT32;
         }
         else {
            minLength = ((minLength-1)/VDSC_ALIGNMENT_INT64 + 1)*VDSC_ALIGNMENT_INT64;
         }
         
         minLength += pDefinition[i].length1;

         break;

      case VDS_BINARY:
      case VDS_STRING:
         minLength = ((minLength-1)/VDSC_ALIGNMENT_CHAR + 1)*VDSC_ALIGNMENT_CHAR;
         minLength += pDefinition[i].length1;

         break;

      case VDS_DECIMAL:
         minLength = ((minLength-1)/VDSC_ALIGNMENT_CHAR + 1)*VDSC_ALIGNMENT_CHAR;
         minLength += pDefinition[i].length1 + 2;

         break;

      case VDS_BOOLEAN:
         minLength = ((minLength-1)/VDSC_ALIGNMENT_BOOL + 1)*VDSC_ALIGNMENT_BOOL;
         minLength += sizeof(bool);
         break;

      case VDS_VAR_BINARY:
      case VDS_VAR_STRING:
         minLength = ((minLength-1)/VDSC_ALIGNMENT_CHAR + 1)*VDSC_ALIGNMENT_CHAR;
         minLength += pDefinition[i].length1;

         if ( pDefinition[i].length2 == 0 ||
            pDefinition[i].length2 >= 4294967295UL - minLength ) {
            maxLength = 4294967295UL;
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

int vdsaValidateDefinition( vdsObjectDefinition * pDefinition )
{
   unsigned int i, j;
   
   switch( pDefinition->type ) {

   case VDS_FOLDER:
      return 0;

   case VDS_HASH_MAP:
      
      /* We do the key and let the queue case test the rest */
      switch( pDefinition->key.type ) {

      case VDS_KEY_INTEGER:
         if ( pDefinition->key.length != 1 &&
              pDefinition->key.length != 2 &&
              pDefinition->key.length != 4 &&
              pDefinition->key.length != 8 ) {
            return VDS_INVALID_FIELD_LENGTH_INT;
         }
         break;

      case VDS_KEY_BINARY:
      case VDS_KEY_STRING:
         if ( pDefinition->key.length == 0 ) {
            return VDS_INVALID_FIELD_LENGTH;
         }
#if SIZEOF_VOID_P > 4
         /* For 64 bits processors */
         if ( pDefinition->key.length > 4294967295 ) {
            return VDS_INVALID_FIELD_LENGTH;
         }
#endif
         break;

      case VDS_KEY_VAR_BINARY:
      case VDS_KEY_VAR_STRING:
         if ( pDefinition->key.minLength == 0 ) {
            return VDS_INVALID_FIELD_LENGTH;
         }
         /*
          * Reminder: maxLength set to zero indicates the maximum value
          * allowed which is 4294967295.
          */
#if SIZEOF_VOID_P > 4
         /* For 64 bits processors */
         if ( pDefinition->key.maxLength > 4294967295 ) {
            return VDS_INVALID_FIELD_LENGTH;
         }
         /* in case maxLength = 0 */
         if ( pDefinition->key.minLength > 4294967295 ) {
            return VDS_INVALID_FIELD_LENGTH;
         }
#endif
         if ( pDefinition->key.maxLength != 0 ) {
            if ( pDefinition->key.minLength > 
                 pDefinition->key.maxLength ) {
               return VDS_INVALID_FIELD_LENGTH;
            }
         }
         break;

      default:
         return VDS_INVALID_FIELD_TYPE;
      }

   case VDS_QUEUE:
      if ( pDefinition->numFields < 0 || 
           pDefinition->numFields > VDS_MAX_FIELDS ) {
         return VDS_INVALID_NUM_FIELDS;
      }

      for ( i = 0; i < pDefinition->numFields; ++i ) {
         switch( pDefinition->fields[i].type ) {

         case VDS_INTEGER:
            if ( pDefinition->fields[i].length != 1 &&
                 pDefinition->fields[i].length != 2 &&
                 pDefinition->fields[i].length != 4 &&
                 pDefinition->fields[i].length != 8 ) {
               return VDS_INVALID_FIELD_LENGTH_INT;
            }
            break;

         case VDS_BINARY:
         case VDS_STRING:
            if ( pDefinition->fields[i].length == 0 ) {
               return VDS_INVALID_FIELD_LENGTH;
            }
#if SIZEOF_VOID_P > 4
            /* For 64 bits processors */
            if ( pDefinition->fields[i].length > 4294967295 ) {
               return VDS_INVALID_FIELD_LENGTH;
            }
#endif
            break;

         case VDS_DECIMAL:
            if ( pDefinition->fields[i].precision == 0 ||
               pDefinition->fields[i].precision > VDS_FIELD_MAX_PRECISION ) {
               return VDS_INVALID_PRECISION;
            }
            if ( pDefinition->fields[i].scale > 
                 pDefinition->fields[i].precision ) {
               return VDS_INVALID_SCALE;
            }
            break;

         case VDS_BOOLEAN:
            break;

         case VDS_VAR_BINARY:
         case VDS_VAR_STRING:
            /* These 2 types are only valid for the last field. */
            if ( i != (pDefinition->numFields-1) ) return VDS_INVALID_FIELD_TYPE;

            /* BIG WARNING: this rule is not captured by the XML schema */
            if ( pDefinition->numFields == 1 && 
                 pDefinition->fields[i].minLength == 0 ) {
               return VDS_INVALID_FIELD_LENGTH;
            }
            /*
             * Reminder: maxLength set to zero indicates the maximum value
             * allowed which is 4294967295.
             */
#if SIZEOF_VOID_P > 4
            /* For 64 bits processors */
            if ( pDefinition->fields[i].maxLength > 4294967295 ) {
               return VDS_INVALID_FIELD_LENGTH;
            }
            /* in case maxLength = 0 */
            if ( pDefinition->fields[i].minLength > 4294967295 ) {
               return VDS_INVALID_FIELD_LENGTH;
            }
#endif
            if ( pDefinition->fields[i].maxLength != 0 ) {
               if ( pDefinition->fields[i].minLength > 
                    pDefinition->fields[i].maxLength ) {
                  return VDS_INVALID_FIELD_LENGTH;
               }
            }
            break;

         default:
            return VDS_INVALID_FIELD_TYPE;
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
         if ( ! isalpha(pDefinition->fields[i].name[0]) ) {
            return VDS_INVALID_FIELD_NAME;
         }
         for ( j = 1; j < VDS_MAX_FIELD_LENGTH; ++j ) {
            if ( pDefinition->fields[i].name[j] == 0 ) break;
            if ( isalnum(pDefinition->fields[i].name[j]) ) continue;
            if ( pDefinition->fields[i].name[j] == '_' ) continue;
            
            return VDS_INVALID_FIELD_NAME;
         }

         /* We must also make sure that field names are not duplicate. */
         for ( j = 0; j < i; ++j ) {
            if ( strncmp( pDefinition->fields[i].name,
                          pDefinition->fields[j].name,
                          VDS_MAX_FIELD_LENGTH ) == 0 ) {
               return VDS_DUPLICATE_FIELD_NAME;
            }
         }
      }
      return 0;
      
   default:
      return VDS_WRONG_OBJECT_TYPE;
   }

}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsaXmlToDefinition( const char           * xmlBuffer,
                         size_t                 lengthInBytes,
                         vdsObjectDefinition ** ppDefinition,
                         char                ** objectName,
                         size_t               * nameLengthInBytes )
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
   
   /*
    * for debugging, I could use this instead:
    * doc = xmlReadMemory( buf, i, NULL, NULL, 0 );
    */
   doc = xmlReadMemory( xmlBuffer, lengthInBytes, NULL, NULL, 
                        XML_PARSE_NOERROR | XML_PARSE_NOWARNING );
   if ( doc == NULL ) {
      errcode = VDS_XML_READ_ERROR;
      goto cleanup;
   }
   
   root = xmlDocGetRootElement( doc );
   if ( root == NULL ) {
      errcode = VDS_XML_INVALID_ROOT;
      goto cleanup;
   }
   if ( (xmlStrcmp(root->name, BAD_CAST "folder")  != 0) &&
        (xmlStrcmp(root->name, BAD_CAST "queue")   != 0) &&
        (xmlStrcmp(root->name, BAD_CAST "hashmap") != 0) ) {
      errcode = VDS_XML_INVALID_ROOT;
      goto cleanup;
   }

   prop = xmlGetProp( root, BAD_CAST "schemaLocation" );
   if ( prop == NULL ) {
      errcode = VDS_XML_NO_SCHEMA_LOCATION;
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
      errcode = VDS_XML_NO_SCHEMA_LOCATION;
      goto cleanup;
   }
   
   parserCtxt = xmlSchemaNewParserCtxt( (char*)&prop[separator] );
   if ( parserCtxt == NULL ) {
      errcode = VDS_XML_PARSER_CONTEXT_FAILED;
      goto cleanup;
   }
   
   schema = xmlSchemaParse( parserCtxt );
   if ( schema == NULL ) {
      errcode = VDS_XML_PARSE_SCHEMA_FAILED;
      goto cleanup;
   }
   
   xmlFree( prop );
   prop = NULL;

   validCtxt = xmlSchemaNewValidCtxt( schema );
   if ( validCtxt == NULL ) {
      errcode = VDS_XML_VALID_CONTEXT_FAILED;
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
      errcode = VDS_XML_VALIDATION_FAILED;
      goto cleanup;
   }

   prop = xmlGetProp( root, BAD_CAST "objName" );
   if ( prop == NULL ) {
      errcode = VDS_INVALID_OBJECT_NAME;
      goto cleanup;
   }
   *objectName = (char*)malloc( xmlStrlen(prop) + 1 );
   if ( *objectName == NULL ) {
      errcode = VDS_NOT_ENOUGH_HEAP_MEMORY;
      goto cleanup;
   }
   strcpy( *objectName, (char *)prop );
   *nameLengthInBytes = strlen( *objectName );
   xmlFree( prop );
   prop = NULL;
   
   if ( xmlStrcmp( root->name, BAD_CAST "folder") == 0 ) {
      *ppDefinition = (vdsObjectDefinition *)
                      calloc( sizeof(vdsObjectDefinition), 1 );
      if (*ppDefinition == NULL ) {
         errcode = VDS_NOT_ENOUGH_HEAP_MEMORY;
         goto cleanup;
      }
      (*ppDefinition)->type = VDS_FOLDER;
      /* No fields to process. Go directly to the exit, error or not. */
      goto cleanup;
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
               errcode = VDS_INVALID_KEY_DEF;
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

   *ppDefinition = (vdsObjectDefinition *)
      calloc( offsetof(vdsObjectDefinition,fields) +
              numFields * sizeof(vdsFieldDefinition), 1 );
   if (*ppDefinition == NULL ) {
      errcode = VDS_NOT_ENOUGH_HEAP_MEMORY;
      goto cleanup;
   }
   (*ppDefinition)->numFields = numFields;

   /* Extract the key, if any */
   if ( nodeKey != NULL ) {
      (*ppDefinition)->type = VDS_HASH_MAP;
      nodeType = nodeKey->children;
      while ( nodeType != NULL ) {
         if ( nodeType->type == XML_ELEMENT_NODE ) {
            if ( xmlStrcmp( nodeType->name, BAD_CAST "integer") == 0 ) {
               prop = xmlGetProp( nodeType, BAD_CAST "size" );
               if ( prop == NULL ) {
                  errcode = VDS_INVALID_KEY_DEF;
                  goto cleanup;
               }
               (*ppDefinition)->key.type = VDS_KEY_INTEGER;
               sscanf( (char*)prop, "%ud", &(*ppDefinition)->key.length );
               xmlFree(prop);
               prop = NULL;
            }
            else if ( xmlStrcmp( nodeType->name, BAD_CAST "string") == 0 ) {
               prop = xmlGetProp( nodeType, BAD_CAST "length" );
               if ( prop == NULL ) {
                  errcode = VDS_INVALID_KEY_DEF;
                  goto cleanup;
               }
               (*ppDefinition)->key.type = VDS_KEY_STRING;
               sscanf( (char*)prop, "%ud", &(*ppDefinition)->key.length );
               xmlFree(prop);
               prop = NULL;
            }
            else if ( xmlStrcmp( nodeType->name, BAD_CAST "binary") == 0 ) {
               prop = xmlGetProp( nodeType, BAD_CAST "length" );
               if ( prop == NULL ) {
                  errcode = VDS_INVALID_KEY_DEF;
                  goto cleanup;
               }
               (*ppDefinition)->key.type = VDS_KEY_BINARY;
               sscanf( (char*)prop, "%ud", &(*ppDefinition)->key.length );
               xmlFree(prop);
               prop = NULL;
            }
            else if ( xmlStrcmp( nodeType->name, BAD_CAST "varString") == 0 ) {
               prop = xmlGetProp( nodeType, BAD_CAST "minLength" );
               if ( prop == NULL ) {
                  errcode = VDS_INVALID_KEY_DEF;
                  goto cleanup;
               }
               sscanf( (char*)prop, "%ud", &(*ppDefinition)->key.minLength );
               xmlFree(prop);
               prop = xmlGetProp( nodeType, BAD_CAST "maxLength" );
               if ( prop == NULL ) {
                  errcode = VDS_INVALID_KEY_DEF;
                  goto cleanup;
               }
               sscanf( (char*)prop, "%ud", &(*ppDefinition)->key.maxLength );
               xmlFree(prop);               
               prop = NULL;
               (*ppDefinition)->key.type = VDS_KEY_VAR_STRING;
            }
            else if ( xmlStrcmp( nodeType->name, BAD_CAST "varBinary") == 0 ) {
               prop = xmlGetProp( nodeType, BAD_CAST "minLength" );
               if ( prop == NULL ) {
                  errcode = VDS_INVALID_KEY_DEF;
                  goto cleanup;
               }
               sscanf( (char*)prop, "%ud", &(*ppDefinition)->key.minLength );
               xmlFree(prop);
               prop = xmlGetProp( nodeType, BAD_CAST "maxLength" );
               if ( prop == NULL ) {
                  errcode = VDS_INVALID_KEY_DEF;
                  goto cleanup;
               }
               sscanf( (char*)prop, "%ud", &(*ppDefinition)->key.maxLength );
               xmlFree(prop);               
               prop = NULL;
               (*ppDefinition)->key.type = VDS_KEY_VAR_BINARY;
            }
            else {
               errcode = VDS_INVALID_KEY_DEF;
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
         (*ppDefinition)->type = VDS_QUEUE;
      }
      else {
         errcode = VDS_WRONG_OBJECT_TYPE;
         goto cleanup;
      }
   }

   numFields = 0;
   while ( nodeField != NULL ) {
      if ( nodeField->type == XML_ELEMENT_NODE ) {
         prop = xmlGetProp( nodeField, BAD_CAST "name" );
         if ( prop == NULL ) {
            errcode = VDS_INVALID_FIELD_NAME;
            goto cleanup;
         }
         strncpy( (*ppDefinition)->fields[numFields].name, 
            (char*)prop, VDS_MAX_FIELD_LENGTH );
         xmlFree(prop);
         prop = NULL;
         
         nodeType = nodeField->children;
         while ( nodeType != NULL ) {
            if ( nodeType->type == XML_ELEMENT_NODE ) {
               
               if ( xmlStrcmp( nodeType->name, BAD_CAST "integer") == 0 ) {
                  prop = xmlGetProp( nodeType, BAD_CAST "size" );
                  if ( prop == NULL ) {
                     errcode = VDS_INVALID_FIELD_LENGTH_INT;
                     goto cleanup;
                  }
                  sscanf( (char*)prop, "%ud", 
                     &(*ppDefinition)->fields[numFields].length );
                  xmlFree(prop);
                  prop = NULL;

                  (*ppDefinition)->fields[numFields].type = VDS_INTEGER;
               }
               else if ( xmlStrcmp( nodeType->name, BAD_CAST "boolean") == 0 ) {
                  (*ppDefinition)->fields[numFields].type = VDS_BOOLEAN;
               }
               else if ( (xmlStrcmp(nodeType->name, BAD_CAST "string") == 0) ||
                         (xmlStrcmp(nodeType->name, BAD_CAST "binary") == 0) ) {
                  prop = xmlGetProp( nodeType, BAD_CAST "length" );
                  if ( prop == NULL ) {
                     errcode = VDS_INVALID_FIELD_LENGTH;
                     goto cleanup;
                  }
                  sscanf( (char*)prop, "%ud", 
                     &(*ppDefinition)->fields[numFields].length );
                  xmlFree(prop);
                  prop = NULL;
                  
                  if ( xmlStrcmp( nodeType->name, BAD_CAST "string") == 0 ) {
                     (*ppDefinition)->fields[numFields].type = VDS_STRING;
                  }
                  else {
                     (*ppDefinition)->fields[numFields].type = VDS_BINARY;
                  }
               }
               else if ( (xmlStrcmp(nodeType->name, BAD_CAST "varString") == 0) ||
                         (xmlStrcmp(nodeType->name, BAD_CAST "varBinary") == 0) ) {
                  if ( xmlStrcmp(nodeField->name, BAD_CAST "lastField") != 0 ) {
                     errcode = VDS_INVALID_FIELD_TYPE;
                     goto cleanup;
                  }
                  prop = xmlGetProp( nodeType, BAD_CAST "minLength" );
                  if ( prop == NULL ) {
                     errcode = VDS_INVALID_FIELD_LENGTH;
                     goto cleanup;
                  }
                  sscanf( (char*)prop, "%ud", 
                     &(*ppDefinition)->fields[numFields].minLength );
                  xmlFree(prop);
                  prop = xmlGetProp( nodeType, BAD_CAST "maxLength" );
                  if ( prop == NULL ) {
                     errcode = VDS_INVALID_FIELD_LENGTH;
                     goto cleanup;
                  }
                  sscanf( (char*)prop, "%ud", 
                     &(*ppDefinition)->fields[numFields].maxLength );
                  xmlFree(prop);
                  prop = NULL;
                  
                  if ( xmlStrcmp( nodeType->name, BAD_CAST "varString") == 0 ) {
                     (*ppDefinition)->fields[numFields].type = VDS_VAR_STRING;
                  }
                  else {
                     (*ppDefinition)->fields[numFields].type = VDS_VAR_BINARY;
                  }
               }
               else if ( xmlStrcmp(nodeType->name, BAD_CAST "decimal") == 0 ) {
                  prop = xmlGetProp( nodeType, BAD_CAST "precision" );
                  if ( prop == NULL ) {
                     errcode = VDS_INVALID_PRECISION;
                     goto cleanup;
                  }
                  sscanf( (char*)prop, "%ud", 
                     &(*ppDefinition)->fields[numFields].precision );
                  xmlFree(prop);
                  prop = xmlGetProp( nodeType, BAD_CAST "scale" );
                  if ( prop == NULL ) {
                     errcode = VDS_INVALID_SCALE;
                     goto cleanup;
                  }
                  sscanf( (char*)prop, "%ud", 
                     &(*ppDefinition)->fields[numFields].scale );
                  xmlFree(prop);
                  prop = NULL;
                  
                  (*ppDefinition)->fields[numFields].type = VDS_DECIMAL;
               }
               else {
                  errcode = VDS_INVALID_FIELD_TYPE;
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

