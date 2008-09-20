/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of Photon (photonsoftware.org).
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

#ifndef PSOA_API_H
#define PSOA_API_H

#include "Common/Common.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * The type of object (as seen from the API, not the engine).
 */
enum psoaObjetType
{
   PSOA_PROCESS = 0x111,
   PSOA_SESSION,
   PSOA_FOLDER,
   PSOA_HASH_MAP,
   PSOA_MAP,
   PSOA_QUEUE,
   PSOA_LIFO
};

typedef enum psoaObjetType psoaObjetType;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum psoaEditMode
{
   PSOA_READ_WRITE = 0,
   PSOA_READ_ONLY,
   PSOA_UPDATE_RO
};

typedef enum psoaEditMode psoaEditMode;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined(WIN32)
#  ifdef BUILD_PHOTON_API
#    define PHOTON_API_EXPORT __declspec ( dllexport )
#  else
#    define PHOTON_API_EXPORT __declspec ( dllimport )
#  endif
#else
#  define PHOTON_API_EXPORT
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** A simple struct to simplify arguments to a few functions. */
struct psoaDataEntry
{
   /** Length of the data in bytes. */
   size_t length;
   
   /** Pointer to the data. */
   const void * data;

};

typedef struct psoaDataEntry psoaDataEntry;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSOA_API_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

