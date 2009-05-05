/*
 * Copyright (C) 2008-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#ifndef PSO_PY_COMMON_H
#define PSO_PY_COMMON_H

#include "Python.h"
#include "structmember.h"

#include <photon/photon.h>
#include "config.h"

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

/*
 * This function always return a new reference.
 */
PyObject * GetDefinitionType( enum psoDefinitionType type )
{
   switch (type) {
      
   case PSO_DEF_USER_DEFINED:
      return PyString_FromString( "User defined" );
   case PSO_DEF_PHOTON_ODBC_SIMPLE:
      return PyString_FromString( "ODBC Simple" );
   default:
      return PyString_FromString( "Unknown definition type" );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if 0

PyObject * GetFieldType( enum psoFieldType type )
{
   switch (type) {
      
   case PSO_INTEGER:
      return PyString_FromString( "Integer" );
   case PSO_BINARY:
      return PyString_FromString( "Binary" );
   case PSO_CHAR:
      return PyString_FromString( "String" );
   case PSO_DECIMAL:
      return PyString_FromString( "Decimal" );
   case PSO_TINYINT:
      return PyString_FromString( "Boolean" );
   case PSO_VARBINARY:
      return PyString_FromString( "Variable Binary" );
   case PSO_VARCHAR:
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
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * This function always return a new reference.
 */
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

/* 
 * Some of the photon string might not be NULL-terminated. This function 
 * goes around this potential issue.
 */
static PyObject*
GetString( const char * s, Py_ssize_t len)
{
   Py_ssize_t i;
    
   for ( i = 0; i < len; ++i ) {
      if ( s[i] == 0 ) return PyString_FromString(s);
   }

   return PyString_FromStringAndSize(s, len);
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "errors.h"

#include "ForwardDeclare.h"

/* The C struct first */
#include "ObjStatus.h"
#include "DataDefinition.h"
#include "KeyDefinition.h"
#include "ObjectDefinition.h"
#include "FolderEntry.h"
#include "Info.h"
#include "DataDefBuilderODBC.h"
#include "DataDefBuilderUser.h"
#include "KeyDefBuilderODBC.h"
#include "KeyDefBuilderUser.h"

#include "Session.h"
#include "Folder.h"
#include "Queue.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSO_PY_COMMON_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

