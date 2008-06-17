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
   int i, j;
   
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

#define VDS_XML_READ_ERROR 1001
#define VDS_XML_INVALID_ROOT 1002
#define VDS_XML_VALIDATION_FAILED 1003
#define VDS_XML_NO_SCHEMA_LOCATION 1004
#define VDS_XML_PARSER_CONTEXT_FAILED 1005
#define VDS_XML_PARSE_SCHEMA_FAILED 1006
#define VDS_XML_VALID_CONTEXT_FAILED  1007

int vdsaXmlToDefinition( const char           * xmlBuffer,
                         size_t                 lengthInBytes,
                         vdsObjectDefinition ** ppDefinition,
                         char                ** objectName,
                         size_t               * nameLengthInBytes )
{
   xmlSchemaPtr schema = NULL;
   xmlSchemaValidCtxtPtr  validCtxt = NULL;
   xmlSchemaParserCtxtPtr parserCtxt = NULL;
   xmlNode * root = NULL, * node;
   xmlDoc  * doc = NULL;
   xmlChar * prop = NULL;
   int errcode = 0;
   int i, j, separator = -1;

//   if ( debug ) {
      doc = xmlReadMemory( xmlBuffer, lengthInBytes, NULL, NULL, 0 );
//   }
//   else {
//      doc = xmlReadMemory( buf, i, NULL, NULL, XML_PARSE_NOERROR | XML_PARSE_NOWARNING );
//   }
   if ( doc == NULL ) {
      errcode = VDS_XML_READ_ERROR;
      goto cleanup;
   }
   
   root = xmlDocGetRootElement( doc );
   if ( root == NULL ) {
      errcode = VDS_XML_INVALID_ROOT;
      goto cleanup;
   }
   if ( xmlStrcmp( root->name, BAD_CAST "folder") == 0 ) {
      fprintf( stderr, "got folder\n" );
   }
   else {
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
   
//   if ( debug ) {
      xmlSchemaSetValidErrors( validCtxt,
                               (xmlSchemaValidityErrorFunc) fprintf,
                               (xmlSchemaValidityWarningFunc) fprintf,
                               stderr );
//   }
//   else {
//      xmlSchemaSetValidErrors( validCtxt,
//                               (xmlSchemaValidityErrorFunc) dummyErrorFunc,
//                               (xmlSchemaValidityWarningFunc) dummyErrorFunc,
//                               stderr );
//   }
   
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
#if 0   
   /* All is well - start the extraction of our data */   
   node = root->children;

   while ( node != NULL ) {
      if ( node->type == XML_ELEMENT_NODE ) {
         if ( xmlStrcmp( node->name, BAD_CAST "vds_location") == 0 ) {
            /* 
             * The schema should normally resolved this by imposing a 
             * limit which is less than PATH_MAX on most systems but...
             * (to test for this we would need to read the pattern restriction
             * on the schema itself - more work than just checking the length
             * of the provided string).
             */
            if ( xmlStrlen(node->children->content) < PATH_MAX ) {
               strcpy( pConfig->wdLocation, (char*)node->children->content );
               node = node->next;
               break;
            }
            errcode = VDSW_CFG_VDS_LOCATION_TOO_LONG;
            goto cleanup;
            fprintf( stderr, "Error: vds_location is too long\n" );
            return -1;
         }
         fprintf( stderr, "Error: missing <vds_location> in config file\n" );
         return -1;
      }
      node = node->next;
   }

   while ( node != NULL ) {
      if ( node->type == XML_ELEMENT_NODE ) {
         if ( xmlStrcmp( node->name, BAD_CAST "mem_size") == 0 ) {
            prop = xmlGetProp( node, BAD_CAST "size" );
            if ( prop == NULL ) {
               errcode = VDSW_CFG_SIZE_IS_MISSING;
               goto cleanup;
            }
            pConfig->memorySizekb = atoi((char*)prop);
            xmlFree(prop);

            prop = xmlGetProp( node, BAD_CAST "units" );
            if ( prop == NULL ) {
               errcode = VDSW_CFG_UNITS_IS_MISSING;
               goto cleanup;
            }
            if ( xmlStrcmp( prop, BAD_CAST "mb") == 0 ) {
               pConfig->memorySizekb *= 1024;
            }
            if ( xmlStrcmp( prop, BAD_CAST "gb") == 0 ) {
               pConfig->memorySizekb *= 1024*1024;
            }
            xmlFree(prop);
            prop = NULL;
            
            node = node->next;
            break;
         }
         errcode = VDSW_CFG_MEM_SIZE_IS_MISSING;
         goto cleanup;
      }
      node = node->next;
   }

   while ( node != NULL ) {
      if ( node->type == XML_ELEMENT_NODE ) {
         if ( xmlStrcmp( node->name, BAD_CAST "watchdog_address") == 0 ) {
            strcpy( pConfig->wdAddress, (char*)node->children->content );
            node = node->next;
            break;
         }
         errcode = VDSW_CFG_WATCHDOG_ADDRESS_IS_MISSING;
         goto cleanup;
      }
      node = node->next;
   }

   while ( node != NULL ) {
      if ( node->type == XML_ELEMENT_NODE ) {
         if ( xmlStrcmp( node->name, BAD_CAST "file_access") == 0 ) {
            prop = xmlGetProp( node, BAD_CAST "access" );
            if ( prop == NULL ) {
               errcode = VDSW_CFG_ACCESS_IS_MISSING;
               goto cleanup;
            }
            
            if ( xmlStrcmp( prop, BAD_CAST "group") == 0 ) {
               pConfig->filePerms = 0660;
               pConfig->dirPerms = 0770;
            }
            else if ( xmlStrcmp( prop, BAD_CAST "world") == 0 ) {
               pConfig->filePerms = 0666;
               pConfig->dirPerms = 0777;
            }
            else {
               pConfig->filePerms = 0600;
               pConfig->dirPerms = 0700;
            }
            xmlFree(prop);
            prop = NULL;
            
            node = node->next;
            break;
         }
         errcode = VDSW_CFG_FILE_ACCESS_IS_MISSING;
         goto cleanup;
      }
      node = node->next;
   }
#endif

cleanup:
   
   if ( parserCtxt ) xmlSchemaFreeParserCtxt( parserCtxt );
   if ( schema ) xmlSchemaFree( schema );
   if ( validCtxt ) xmlSchemaFreeValidCtxt( validCtxt );
   if ( prop ) xmlFree( prop );
   if ( doc ) xmlFreeDoc( doc );

//   if ( errcode != VDSW_OK ) {
//      vdscSetError( pError, g_wdErrorHandle, errcode );
//      return -1;
//   }
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

