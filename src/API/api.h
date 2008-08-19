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

#ifndef PSA_API_H
#define PSA_API_H

#include "Common/Common.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * The type of object (as seen from the API, not the engine).
 */
enum psaObjetType
{
   PSA_PROCESS = 0x111,
   PSA_SESSION,
   PSA_FOLDER,
   PSA_HASH_MAP,
   PSA_MAP,
   PSA_QUEUE,
   PSA_LIFO
};

typedef enum psaObjetType psaObjetType;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum psaEditMode
{
   PSA_READ_WRITE = 0,
   PSA_READ_ONLY,
   PSA_UPDATE_RO
};

typedef enum psaEditMode psaEditMode;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined(WIN32)
#  ifdef BUILD_VDSF_API
#    define VDSF_API_EXPORT __declspec ( dllexport )
#  else
#    define VDSF_API_EXPORT __declspec ( dllimport )
#  endif
#else
#  define VDSF_API_EXPORT
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** A simple struct to simplify arguments to a few functions. */
struct psaDataEntry
{
   /** Length of the data in bytes. */
   size_t length;
   
   /** Pointer to the data. */
   const void * data;

};

typedef struct psaDataEntry psaDataEntry;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSA_API_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

