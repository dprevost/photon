/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#include <libxml/xmlschemas.h>
#include <libxml/xmlschemastypes.h>

#include "Watchdog/Config.h"
#include "Watchdog/wdErrors.h"

extern pscErrMsgHandle g_wdErrorHandle;

enum ECFG_PARAMS
{
   VDS_WDADDRESS = 0,
   eVDS_LOCATION,
   eVDS_MEMSIZE,
   eVDS_USE_LOG,
   eVDS_FILEPERMS,
   eVDS_DIRPERMS,
   eVDS_NUM_CFG_PARAMS
};

const char g_paramNames[eVDS_NUM_CFG_PARAMS][64] = { 
   "WatchdogAddress", 
   "VDSLocation", 
   "MemorySize", 
   "LogTransaction", 
   "FilePermissions", 
   "DirectoryPermissions" };

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static void dummyErrorFunc( void * ctx, const char * msg, ...)
{
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*!
 *
 * \param[in]  cfgname  The name of the configuration file.
 * \param[out] pConfig  A pointer to the struct holding the value of the 
 *                      parameters (once read).
 * \param[in]  debug    Set to one to get additional debug information on 
 *                      the terminal. Or zero for no additional information.
 * \param[in,out] pError A pointer to a pscErrorHandler struct.
 *
 * \retval true on success
 * \retval false on error (use pError to retrieve the error(s))
 *
 * \pre \em cfgname cannot be NULL.
 * \pre \em pConfig cannot be NULL.
 * \pre \em pError cannot be NULL.
 
 */
 
bool vdswReadConfig( const char          * cfgname,
                     struct ConfigParams * pConfig,
                     int                   debug,
                     pscErrorHandler    * pError )
{
   xmlSchemaPtr schema = NULL;
   xmlSchemaValidCtxtPtr  validCtxt = NULL;
   xmlSchemaParserCtxtPtr parserCtxt = NULL;
   xmlNode * root = NULL, * node;
   xmlDoc  * doc = NULL;
   xmlChar * prop = NULL;
   int i, j, fd = -1, separator = -1;
   enum vdswErrors errcode = VDSW_OK;
   char buf[10000];

   /* These are to make sure we have read all parameters */
   int isPresent[eVDS_NUM_CFG_PARAMS];
   
   VDS_PRE_CONDITION( cfgname  != NULL );
   VDS_PRE_CONDITION( pConfig  != NULL );
   VDS_PRE_CONDITION( pError   != NULL );

   memset( pConfig, 0, sizeof(struct ConfigParams) );

   for ( i = 0; i < eVDS_NUM_CFG_PARAMS; ++i ) {
      isPresent[i] = 0;
   }

   fd = open( cfgname, O_RDONLY );
   if ( fd == -1 ) {
      pscSetError( pError, PSC_ERRNO_HANDLE, errno );
      return false;
   }
   i = read( fd, buf, 10000 );
   if ( i < 1 ) {
      if ( i == -1 ) pscSetError( pError, PSC_ERRNO_HANDLE, errno );
      return false;
   }

   if ( debug ) {
      doc = xmlReadMemory( buf, i, NULL, NULL, 0 );
   }
   else {
      doc = xmlReadMemory( buf, i, NULL, NULL, XML_PARSE_NOERROR | XML_PARSE_NOWARNING );
   }
   if ( doc == NULL ) {
      errcode = VDSW_XML_READ_ERROR;
      goto cleanup;
   }
   
   root = xmlDocGetRootElement( doc );
   if ( root == NULL ) {
      errcode = VDSW_XML_NO_ROOT;
      goto cleanup;
   }
   if ( xmlStrcmp( root->name, BAD_CAST "vdsf_config") != 0 ) {
      errcode = VDSW_XML_INVALID_ROOT;
      goto cleanup;
   }
   
   prop = xmlGetProp( root, BAD_CAST "schemaLocation" );
   if ( prop == NULL ) {
      errcode = VDSW_XML_NO_SCHEMA_LOCATION;
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
      errcode = VDSW_XML_NO_SCHEMA_LOCATION;
      goto cleanup;
   }
   
   parserCtxt = xmlSchemaNewParserCtxt( (char*)&prop[separator] );
   if ( parserCtxt == NULL ) {
      errcode = VDSW_XML_PARSER_CONTEXT_FAILED;
      goto cleanup;
   }
   
   schema = xmlSchemaParse( parserCtxt );
   if ( schema == NULL ) {
      errcode = VDSW_XML_PARSE_SCHEMA_FAILED;
      goto cleanup;
   }
   
   xmlFree( prop );
   prop = NULL;

   validCtxt = xmlSchemaNewValidCtxt( schema );
   if ( validCtxt == NULL ) {
      errcode = VDSW_XML_VALID_CONTEXT_FAILED;
      goto cleanup;
   }
   
   if ( debug ) {
      xmlSchemaSetValidErrors( validCtxt,
                               (xmlSchemaValidityErrorFunc) fprintf,
                               (xmlSchemaValidityWarningFunc) fprintf,
                               stderr );
   }
   else {
      xmlSchemaSetValidErrors( validCtxt,
                               (xmlSchemaValidityErrorFunc) dummyErrorFunc,
                               (xmlSchemaValidityWarningFunc) dummyErrorFunc,
                               stderr );
   }
   
   if ( xmlSchemaValidateDoc( validCtxt, doc ) != 0 ) {
      errcode = VDSW_XML_VALIDATION_FAILED;
      goto cleanup;
   }
   
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
         }
         errcode = VDSW_CFG_VDS_LOCATION_IS_MISSING;
         goto cleanup;
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

cleanup:
   
   if ( parserCtxt ) xmlSchemaFreeParserCtxt( parserCtxt );
   if ( schema ) xmlSchemaFree( schema );
   if ( validCtxt ) xmlSchemaFreeValidCtxt( validCtxt );
   if ( prop ) xmlFree( prop );
   if ( doc ) xmlFreeDoc( doc );

   if ( errcode != VDSW_OK ) {
      pscSetError( pError, g_wdErrorHandle, errcode );
      return false;
   }
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

