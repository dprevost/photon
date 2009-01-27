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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifndef QSR_CONFIG_H
#define QSR_CONFIG_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlstring.h>

#include "Common/Common.h"
#include "Common/ErrorHandler.h"

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \defgroup qsrConfigFile The config file module
 *
 * Reads the config file needed to start quasar.
 *
 * This module reads the configuration of the shared memory written in
 * XML.
 *
 * @{
 */

/**
 * \file
 * Reads the config file needed to start quasar.
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This struct holds all the parameters set the config file.
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
   /** Address where the server listen to incoming requests from application */
   char qsrAddress[PATH_MAX];

   /** Location of the backstore file for the shared memory */
   char memLocation[PATH_MAX];

   /** The size of the shared memory in kilobytes */
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
bool qsrReadConfig( const char          * cfgname,
                     struct ConfigParams * pConfig,
                     int                   debug,
                     psocErrorHandler    * pError  );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * @}
 */

END_C_DECLS

#endif /* QSR_CONFIG_H */

