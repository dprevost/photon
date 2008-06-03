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

#ifndef VDSW_CONFIG_H
#define VDSW_CONFIG_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlstring.h>

#include "Common/Common.h"
#include "Common/ErrorHandler.h"

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \defgroup vdswConfigFile The config file module
 *
 * Reads the config file needed to start vdsf.
 *
 * This module reads the configuration of the shared memory written in
 * XML.
 *
 * @{
 */

/**
 * \file
 * Reads the config file needed to start vdsf.
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This struct holds all the parameters set the config file.
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
int vdswReadConfig( const char*          cfgname,
                    struct ConfigParams* pConfig,
                    int                  debug,
                    vdscErrorHandler*    pError  );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * @}
 */

END_C_DECLS

#endif /* VDSW_CONFIG_H */

