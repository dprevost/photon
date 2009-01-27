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

#ifndef PSOC_OPTIONS_H
#define PSOC_OPTIONS_H

#include "Common.h"

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \defgroup psocOptions Module to handle options for programs.
 *
 * This module provides a simple way (for programs) to handle options.
 * For example: quasar --config filename
 * 
 * I know that this looks like reinventing the wheel... but I've look
 * at alternatives and was not satisfied. They were either too complex,
 * too simple, not portable enough (Photon on Windows should not depend
 * on cygwin or the gnu utilities to be present), etc.
 *
 * \note The help option (-? -h --help) is always present.
 *  
 * \note Also, as always, options are case sensitive.
 *
 * \warning This module places limits on the lenght of some fields. 
 *     - Long options are limited to ::PSOC_OPT_LONG_OPT_LENGTH characters.
 *     - Comments on the options are limited to ::PSOC_OPT_COMMENT_LENGTH 
 *          characters.
 *     - names for arguments to options (to be used in the message 
 *       "usage: ..." are limited to ::PSOC_OPT_ARGUMENT_MSG_LENGTH characters.
 *     .
 *     I don't know if these restrictions make sense... they do 
 *          make the programming job easier...
 *
 * \ingroup psocCommon
 * @{
 */

/** Maximum length of a long option, */
#define PSOC_OPT_LONG_OPT_LENGTH  30
/** Maximum length of the display name of a argument. */
#define PSOC_OPT_ARGUMENT_MSG_LENGTH 20
/** Maximum length of the comment field (to be displayed). */
#define PSOC_OPT_COMMENT_LENGTH   80

/**
 * This struct is used to define which options will be supported
 * by the program. 
 */
struct psocOptStruct
{
   /** Single character option. */
   char shortOpt;
   /** Multi-character option. */
   char longOpt[PSOC_OPT_LONG_OPT_LENGTH];
   /** Is the option mandatory or optional? */
   int  isOptionel;
   /** Display name of the argument - use "" if the option takes no argument */
   char argumentMessage[PSOC_OPT_ARGUMENT_MSG_LENGTH];
   /** Comment to be displayed in "usage: ..." */
   char comment[PSOC_OPT_COMMENT_LENGTH];
   
};

typedef struct psocOptStruct psocOptStruct;

/** An opaque handle to our internal struct. */
typedef void * psocOptionHandle;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*! \brief Populate the internal arrays of all supported options. */
PHOTON_COMMON_EXPORT
bool psocSetSupportedOptions( int                    numOpts, 
                              struct psocOptStruct * opts,
                              psocOptionHandle     * pHandle );

/*!
 * \brief Unset the internal arrays of all supported options (and free
 * the allocated memory, if needed).
 */
PHOTON_COMMON_EXPORT
void psocUnsetSupportedOptions( psocOptionHandle handle );

/*! \brief Verify the validity of the options passed in by the end-user. */
PHOTON_COMMON_EXPORT
int psocValidateUserOptions( psocOptionHandle   handle,
                             int                argc, 
                             char             * argv[], 
                             int                printError );

/*! \brief Print usage information. */
PHOTON_COMMON_EXPORT
void psocShowUsage( psocOptionHandle   handle,
                    const char       * progName,
                    const char       * addArguments );

/*! \brief Retrieves the argument associated with a given option. */
PHOTON_COMMON_EXPORT
bool psocGetShortOptArgument( psocOptionHandle    handle,
                              const char          opt, 
                              char             ** argument );

/*! \brief Retrieves the argument associated with a given option. */
PHOTON_COMMON_EXPORT
bool psocGetLongOptArgument( psocOptionHandle    handle,
                             const char        * opt, 
                             char             ** argument );

/*! \brief Verify if the option is present. */
PHOTON_COMMON_EXPORT
bool psocIsLongOptPresent( psocOptionHandle   handle,
                           const char       * opt );

/*! \brief Verify if the option is present. */
PHOTON_COMMON_EXPORT
bool psocIsShortOptPresent( psocOptionHandle handle,
                            const char       opt );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * @}
 */

END_C_DECLS

#endif /* PSOC_OPTIONS_H */

