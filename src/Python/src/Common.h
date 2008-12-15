/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@photonsoftware.org>
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

#include "Python.h"
#include "structmember.h"

#include <photon/photon.h>

const char * pson_ErrorMessage( int );

/*
 * We define our pointer to our exception and helper function before 
 * including the rest of the code so that they can be used by all.
 */
static PyObject * PhotonError;

static void SetException( int errcode )
{
   const char * msg;
   PyObject * err = NULL;
   
   msg = pson_ErrorMessage( errcode );
   if ( msg == NULL ) {
      err = Py_BuildValue("si", "Unknown Photon Error!", errcode);
   }
   else {
      err = Py_BuildValue("si", msg, errcode);
   }

   if ( err != NULL ) {
      PyErr_SetObject( PhotonError, err );
   }
}

/* The C struct first */
#include "ObjStatus.h"
#include "BasicDef.h"
#include "FolderEntry.h"

#include "Session.h"
#include "Folder.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * This array allows us to convert a psoObjectType into a 
 * human-readable string.
 */
const char * g_ObjectType[PSO_LAST_OBJECT_TYPE-1] = {
   "Folder",
   "Hash Map",
   "LIFO Queue",
   "Fast Hash Map",
   "FIFO Queue" 
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
