/*
 * Copyright (C) 2006 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef VDSC_CONFIG_FILE_H
#define VDSC_CONFIG_FILE_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Putting the routines to read the VDS config file in their own
 * class might look like 
 * overkill but in practice it is used in many places (including some
 * of the "white-box" tests). Therefore it makes more sense to get
 * them out of the ProcessHook class...
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Common.h"
#include "ErrorHandler.h"

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \defgroup vdscConfigFile The config file module
 *
 * Reads the config file needed to start vdsf.
 *
 * This module is very vdsf specific (contrary to most modules in the
 * Common directory). Shoud this be changed?
 *
 * \todo Rewrite ConfigFile to be generic (maybe?).
 *
 * \ingroup vdscCommon
 * @{
 */

/**
 * \file
 * Reads the config file needed to start vdsf.
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This struct holds all the parameters set the config file. Note that 
 * they may also be read from the registry if we are running as a NT
 * service.
 *
 * For the watchdog:
 *
 * If you modify (add/modify/remove) a parameter, you'll need to update, at 
 * a minimum, the following code:
 *   - ReadConfig()
 *   - ReadRegistry()
 *   - Install()
 *   - Uninstall()
 */

struct ConfigParams
{
   /** Address where the wd listen to incoming requests from application */
   char wdAddress[PATH_MAX];

   /** Location of the backstore file for the VDS */
   char wdLocation[PATH_MAX];

   /** The size of the VDS in kilobytes */
   size_t memorySizekb;

   /** Whether we log transactions to disk or not */
   int logOn;
   
   /** Permissions for files */
   int filePerms;
   
   /** Permissions for directories */
   int dirPerms;

};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*! \brief Read the configuration file from disk. */
VDSF_COMMON_EXPORT
int vdscReadConfig( const char*          cfgname,
                    struct ConfigParams* pConfig,
                    const char**         pMissing,
                    vdscErrorHandler*    pError  );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * @}
 */

END_C_DECLS

#endif /* VDSC_CONFIG_FILE_H */
