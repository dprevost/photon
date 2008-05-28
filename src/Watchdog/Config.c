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

#include "Config.h"
#include <libxml/xmlschemas.h>
#include <libxml/xmlschemastypes.h>

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

/*!
 * \todo Eliminate this. It would be better to read the file until an EOL
 *       is found.
 */
#define LINE_MAX_LEN (2*PATH_MAX+2)

const char g_paramNames[eVDS_NUM_CFG_PARAMS][64] = { 
   "WatchdogAddress", 
   "VDSLocation", 
   "MemorySize", 
   "LogTransaction", 
   "FilePermissions", 
   "DirectoryPermissions" };

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*!
 *
 * \param[in]  cfgname  The name of the configuration file.
 * \param[out] pConfig  A pointer to the struct holding the value of the 
 *                      parameters (once read).
 * \param[out] pMissing A pointer to the name of the first missing parameter
 *                      (if one or more are missing). NULL if all is ok.
 * \param[in,out] pError A pointer to a vdscErrorHandler struct.
 *
 * \retval 0 on success
 * \retval -1 on error (use pError to retrieve the error(s))
 *
 * \pre \em cfgname cannot be NULL.
 * \pre \em pConfig cannot be NULL.
 * \pre \em pMissing cannot be NULL.
 * \pre \em pError cannot be NULL.
 
 */
 
int vdswReadConfig( const char*          cfgname,
                    struct ConfigParams* pConfig,
                    const char**         pMissing,
                    vdscErrorHandler*    pError  )
{
   xmlSchemaPtr schema = NULL;
   xmlSchemaValidCtxtPtr  validCtxt = NULL;
   xmlSchemaParserCtxtPtr parserCtxt = NULL;
   xmlNode * root = NULL, * node;
   xmlDoc  * doc;
   xmlChar * prop;
   int i;
   
   /* These are to make sure we have read all parameters */
   int isPresent[eVDS_NUM_CFG_PARAMS];
   
   VDS_PRE_CONDITION( cfgname  != NULL );
   VDS_PRE_CONDITION( pConfig  != NULL );
   VDS_PRE_CONDITION( pMissing != NULL );
   VDS_PRE_CONDITION( pError    != NULL );

   memset( pConfig, 0, sizeof(struct ConfigParams) );

   *pMissing = NULL;

   for ( i = 0; i < eVDS_NUM_CFG_PARAMS; ++i ) {
      isPresent[i] = 0;
   }

   doc = xmlReadFile( cfgname, NULL, 0 );
   if ( doc == NULL ) return -1;
   
   root = xmlDocGetRootElement( doc );
   prop = xmlGetProp( root, BAD_CAST "schemaLocation" );
   if ( prop == NULL ) return -1;
   
   for ( i = 0; i < xmlStrlen(prop)-1; ++i ) {
      if ( prop[i] == ' ' ) parserCtxt = xmlSchemaNewParserCtxt( (char*)&prop[i+1] );
   }
   xmlFree( prop );
   if ( parserCtxt == NULL ) return -1;
   
   schema = xmlSchemaParse( parserCtxt );
   xmlSchemaFreeParserCtxt( parserCtxt );

   validCtxt = xmlSchemaNewValidCtxt( schema );

   xmlSchemaSetValidErrors( validCtxt,
                            (xmlSchemaValidityErrorFunc) fprintf,
                            (xmlSchemaValidityWarningFunc) fprintf,
                            stderr );
   if ( xmlSchemaValidateDoc( validCtxt, doc ) != 0 ) {
      fprintf( stderr, "Error: document validation (for config) failed\n" );
      return -1;
   }
   
   root = xmlDocGetRootElement( doc );
   /* With schemas, the root element might not be what we expect. Check it. */
   if ( xmlStrcmp( root->name, BAD_CAST "vdsf_config") != 0 ) {
      fprintf( stderr, "Error: missing <vdsf_config> in config file\n" );
      return -1;
   }
   
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
               fprintf( stderr, "Error: missing \"size\" in config file\n" );
               return -1;
            }
            pConfig->memorySizekb = atoi((char*)prop);
            xmlFree(prop);

            prop = xmlGetProp( node, BAD_CAST "units" );
            if ( prop == NULL ) {
               fprintf( stderr, "Error: missing \"units\" in config file\n" );
               return -1;
            }
            if ( xmlStrcmp( prop, BAD_CAST "mb") == 0 ) {
               pConfig->memorySizekb *= 1024;
            }
            if ( xmlStrcmp( prop, BAD_CAST "gb") == 0 ) {
               pConfig->memorySizekb *= 1024*1024;
            }
            xmlFree(prop);

            node = node->next;
            break;
         }
         fprintf( stderr, "Error: missing <mem_size> in config file\n" );
         return -1;
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
         fprintf( stderr, "Error: missing <watchdog_address> in config file\n" );
         return -1;
      }
      node = node->next;
   }

   while ( node != NULL ) {
      if ( node->type == XML_ELEMENT_NODE ) {
         if ( xmlStrcmp( node->name, BAD_CAST "file_access") == 0 ) {
            prop = xmlGetProp( node, BAD_CAST "access" );
            if ( prop == NULL ) {
               fprintf( stderr, "Error: missing \"access\" in config file\n" );
               return -1;
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
            
            node = node->next;
            break;
         }
         fprintf( stderr, "Error: missing <file_access> in config file\n" );
         return -1;
      }
      node = node->next;
   }

   xmlSchemaFreeValidCtxt( validCtxt );
   xmlFreeDoc( doc );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

