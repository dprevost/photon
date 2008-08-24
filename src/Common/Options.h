/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of Photon (photonsoftware.org).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation and appearing in the file COPYING included in the
 * packaging of this library.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifndef PSC_OPTIONS_H
#define PSC_OPTIONS_H

#include "Common.h"

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * \defgroup pscOptions Module to handle options for programs.
 *
 * This module provides a simple way (for programs) to handle options.
 * For example: quasar --config filename
 * 
 * I know that this looks like reinventing the wheel... but I've look
 * at alternatives and was not satisfied. They were either too complex,
 * too simple, not portable enough (VDSF on Windows should not depend
 * on cygwin or the gnu utilities to be present), etc.
 *
 * \note The help option (-? -h --help) is always present.
 *  
 * \note Also, as always, options are case sensitive.
 *
 * \warning This module places limits on the lenght of some fields. 
 *     - Long options are limited to ::PSC_OPT_LONG_OPT_LENGTH characters.
 *     - Comments on the options are limited to ::PSC_OPT_COMMENT_LENGTH 
 *          characters.
 *     - names for arguments to options (to be used in the message 
 *       "usage: ..." are limited to ::PSC_OPT_ARGUMENT_MSG_LENGTH characters.
 *     .
 *     I don't know if these restrictions make sense... they do 
 *          make the programming job easier...
 *
 * \ingroup pscCommon
 * @{
 */

/** Maximum length of a long option, */
#define PSC_OPT_LONG_OPT_LENGTH  30
/** Maximum length of the display name of a argument. */
#define PSC_OPT_ARGUMENT_MSG_LENGTH 20
/** Maximum length of the comment field (to be displayed). */
#define PSC_OPT_COMMENT_LENGTH   80

/**
 * This struct is used to define which options will be supported
 * by the program. 
 */
struct pscOptStruct
{
   /** Single character option. */
   char shortOpt;
   /** Multi-character option. */
   char longOpt[PSC_OPT_LONG_OPT_LENGTH];
   /** Is the option mandatory or optional? */
   int  isOptionel;
   /** Display name of the argument - use "" if the option takes no argument */
   char argumentMessage[PSC_OPT_ARGUMENT_MSG_LENGTH];
   /** Comment to be displayed in "usage: ..." */
   char comment[PSC_OPT_COMMENT_LENGTH];
   
};

typedef struct pscOptStruct pscOptStruct;

/** An opaque handle to our internal struct. */
typedef void * pscOptionHandle;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*! \brief Populate the internal arrays of all supported options. */
VDSF_COMMON_EXPORT
bool pscSetSupportedOptions( int                   numOpts, 
                             struct pscOptStruct * opts,
                             pscOptionHandle     * pHandle );

/*!
 * \brief Unset the internal arrays of all supported options (and free
 * the allocated memory, if needed).
 */
VDSF_COMMON_EXPORT
void pscUnsetSupportedOptions( pscOptionHandle handle );

/*! \brief Verify the validity of the options passed in by the end-user. */
VDSF_COMMON_EXPORT
int pscValidateUserOptions( pscOptionHandle   handle,
                            int               argc, 
                            char            * argv[], 
                            int               printError );

/*! \brief Print usage information. */
VDSF_COMMON_EXPORT
void pscShowUsage( pscOptionHandle   handle,
                   char            * progName,
                   char            * addArguments );

/*! \brief Retrieves the argument associated with a given option. */
VDSF_COMMON_EXPORT
bool pscGetShortOptArgument( pscOptionHandle    handle,
                             const char         opt, 
                             char            ** argument );

/*! \brief Retrieves the argument associated with a given option. */
VDSF_COMMON_EXPORT
bool pscGetLongOptArgument( pscOptionHandle    handle,
                            const char       * opt, 
                            char            ** argument );

/*! \brief Verify if the option is present. */
VDSF_COMMON_EXPORT
bool pscIsLongOptPresent( pscOptionHandle   handle,
                          const char      * opt );

/*! \brief Verify if the option is present. */
VDSF_COMMON_EXPORT
bool pscIsShortOptPresent( pscOptionHandle handle,
                           const char      opt );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * @}
 */

END_C_DECLS

#endif /* PSC_OPTIONS_H */

