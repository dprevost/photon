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

#include "ConfigFile.h"

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
 
int vdscReadConfig( const char*          cfgname,
                    struct ConfigParams* pConfig,
                    const char**         pMissing,
                    vdscErrorHandler*    pError  )
{
   char line[LINE_MAX_LEN];
   char throwIt[LINE_MAX_LEN];
   int i;
   FILE* fp = NULL;
   
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
   
   fp = fopen( cfgname, "r" );
   if ( fp == NULL ) {
      vdscSetError( pError, VDSC_ERRNO_HANDLE, errno );
      return -1;
   }
   
   for (;;) {
      memset( line, 0, LINE_MAX_LEN );

      /* We've reach the end of file */
      if ( fgets ( line, LINE_MAX_LEN, fp ) == NULL ) break;
      
      /* Comment delimiter */
      if ( line[0] == '#' ) continue;
      
      /*
       * We can't just do a loop on all parameters but have to
       * check them one by one - otherwise we would not be able
       * to do the assignment to the appropriate member of pconfig.
       */

      if ( strncmp( line, 
                    g_paramNames[eVDS_LOCATION], 
                    strlen(g_paramNames[eVDS_LOCATION]) ) == 0 ) {
         sscanf( line, "%s %s", throwIt, pConfig->wdLocation );         
         isPresent[eVDS_LOCATION] = 1;
         continue;
      }
      if ( strncmp( line, 
                    g_paramNames[eVDS_MEMSIZE], 
                    strlen(g_paramNames[eVDS_MEMSIZE]) ) == 0 ) {
         sscanf( line, "%s "VDSF_SIZE_T_FORMAT, throwIt, &pConfig->memorySizekb );         
         isPresent[eVDS_MEMSIZE] = 1;
         continue;
      }
      if ( strncmp( line, 
                    g_paramNames[VDS_WDADDRESS], 
                    strlen(g_paramNames[VDS_WDADDRESS]) ) == 0 ) {
         sscanf( line, "%s %s", throwIt, pConfig->wdAddress );
         isPresent[VDS_WDADDRESS] = 1;
         continue;
      }
      if ( strncmp( line, 
                    g_paramNames[eVDS_USE_LOG], 
                    strlen(g_paramNames[eVDS_USE_LOG]) ) == 0 ) {
         sscanf( line, "%s %d", throwIt, &pConfig->logOn );         
         isPresent[eVDS_USE_LOG] = 1;
         continue;
      }
      if ( strncmp( line, 
                    g_paramNames[eVDS_FILEPERMS], 
                    strlen(g_paramNames[eVDS_FILEPERMS]) ) == 0 ) {
         sscanf( line, "%s %i", throwIt, &pConfig->filePerms );         
         isPresent[eVDS_FILEPERMS] = 1;
         continue;
      }
      if ( strncmp( line, 
                    g_paramNames[eVDS_DIRPERMS], 
                    strlen(g_paramNames[eVDS_DIRPERMS]) ) == 0 ) {
         sscanf( line, "%s %i", throwIt, &pConfig->dirPerms );         
         isPresent[eVDS_DIRPERMS] = 1;
      }
   }   

   fclose( fp );

   for ( i = 0; i < eVDS_NUM_CFG_PARAMS; ++i ) {
      if ( isPresent[i] == 0 ) {
         *pMissing = g_paramNames[i];
         return -1;
      }
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

