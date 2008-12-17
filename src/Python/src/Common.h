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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

PyObject * GetFieldType( enum psoFieldType type )
{
   switch (type) {
      
   case PSO_INTEGER:
      return PyString_FromString( "Integer" );
   case PSO_BINARY:
      return PyString_FromString( "Binary" );
   case PSO_STRING:
      return PyString_FromString( "String" );
   case PSO_DECIMAL:
      return PyString_FromString( "Decimal" );
   case PSO_BOOLEAN:
      return PyString_FromString( "Boolean" );
   case PSO_VAR_BINARY:
      return PyString_FromString( "Variable Binary" );
   case PSO_VAR_STRING:
      return PyString_FromString( "Variable String" );
   }

   return PyString_FromString( "Unknown Type" );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

PyObject * GetKeyType( enum psoKeyType type )
{
   switch (type) {
      
   case PSO_KEY_INTEGER:
      return PyString_FromString( "Integer" );
   case PSO_KEY_BINARY:
      return PyString_FromString( "Binary" );
   case PSO_KEY_STRING:
      return PyString_FromString( "String" );
   case PSO_KEY_VAR_BINARY:
      return PyString_FromString( "Variable Binary" );
   case PSO_KEY_VAR_STRING:
      return PyString_FromString( "Variable String" );
   }

   return PyString_FromString( "Unknown Type" );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

PyObject * GetObjectType( enum psoObjectType objType )
{
   switch (objType) {
      
   case PSO_FOLDER:
      return PyString_FromString( "Folder" );
   case PSO_HASH_MAP:
      return PyString_FromString( "Hash Map" );
   case PSO_LIFO:
      return PyString_FromString( "LIFO Queue" );
   case PSO_FAST_MAP:
      return PyString_FromString( "Fast Hash Map" );
   case PSO_QUEUE:
      return PyString_FromString( "FIFO Queue" );
   case PSO_LAST_OBJECT_TYPE:
      return PyString_FromString( "Unknown Type" );
   }

   return PyString_FromString( "Unknown Type" );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

PyObject * GetObjectStatus( int status )
{
   /* From Nucleus/TxStatus.h:
      #define PSON_TXS_OK                   0x00
      #define PSON_TXS_DESTROYED            0x01
      #define PSON_TXS_ADDED                0x02
      #define PSON_TXS_EDIT                 0x04
      #define PSON_TXS_REPLACED             0x08
      #define PSON_TXS_DESTROYED_COMMITTED  0x10
      #define PSON_TXS_EDIT_COMMITTED       0x20
      
      So what follows is a hack until I defined an enum in psoCommon.h
   */
   
   if ( status == 0 ) {
      return PyString_FromString( "OK" );
   }
   if ( status & 0x01 ) {
      return PyString_FromString( "Destroyed" );
   }
   if ( status & 0x02 ) {
      return PyString_FromString( "Added" );
   }
   if ( status & 0x04 ) {
      return PyString_FromString( "Edit" );
   }
   if ( status & 0x08 ) {
      return PyString_FromString( "Replaced" );
   }
   if ( status & 0x10 ) {
      return PyString_FromString( "Destroyed and Committed" );
   }
   if ( status & 0x20 ) {
      return PyString_FromString( "Edited and Committed" );
   }

   return PyString_FromString( "Unknown Status" );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The C struct first */
#include "ObjStatus.h"
#include "KeyDefinition.h"
#include "BasicDef.h"
#include "FieldDefinition.h"
#include "FolderEntry.h"
#include "Info.h"

#include "Session.h"
#include "Folder.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

