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

#ifndef PSO_PY_SESSION_H
#define PSO_PY_SESSION_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Python.h"
#include "structmember.h"

#include <photon/photon.h>

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct {
   PyObject_HEAD
   /* size_t -> on 64 bits OSes, this int will be 64 bits */
   size_t handle;
} Session;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static void
Session_dealloc( PyObject * self )
{
   Session * s = (Session *)self;

   if ( s->handle != 0 ) {
      psoExitSession( (PSO_HANDLE)s->handle );
   }
   
   self->ob_type->tp_free( self );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Session_new( PyTypeObject * type, PyObject * args, PyObject * kwds )
{
   Session * self;

   self = (Session *)type->tp_alloc( type, 0 );
   if (self != NULL) {
      self->handle = 0;
   }

   return (PyObject *)self;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static int
Session_init( PyObject * self, PyObject * args, PyObject * kwds )
{
   int errcode;
   PSO_HANDLE h;
   Session * s = (Session *)self;
    
   errcode = psoInitSession( &h );
   if ( errcode == 0 ) {
      s->handle = (size_t)h;
      return 0;
   }
   
   PyErr_SetString( PhotonError, "Photon Error!" );
   return -1;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Session_Commit( Session * self )
{
   int errcode;
   
   errcode = psoCommit( (PSO_HANDLE)self->handle );
   if ( errcode != 0 ) {
      SetException( errcode );
      return NULL;
   }

   Py_INCREF(Py_None);
   return Py_None;   
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Session_CreateObject( Session * self, PyObject * args )
{
   int errcode;
   const char * objectName;
   PyObject * list = NULL;
   psoBasicObjectDef definition;
   psoFieldDefinition  * fields = NULL;
   BaseDef * baseDef;
   KeyDefinition * key;
   FieldDefinition * item = NULL;
   Py_ssize_t i;
   
   if ( !PyArg_ParseTuple(args, "sO|O", &objectName, &baseDef, &list) ) {
      return NULL;
   }

   if ( list == NULL ) {
      if ( baseDef->numFields != 0 ) {
         SetException( PSO_INVALID_NUM_FIELDS );
         return NULL;
      }
   }
   else {
      if ( (Py_ssize_t)baseDef->numFields != PyList_Size(list) ) {
         SetException( PSO_INVALID_NUM_FIELDS );
         return NULL;
      }
      
      fields = (psoFieldDefinition *) 
         PyMem_Malloc(baseDef->numFields*sizeof(psoFieldDefinition));
      if ( fields == NULL ) {
         return PyErr_NoMemory();
      }
      
      for ( i = 0; i < (Py_ssize_t)baseDef->numFields; ++i ) {

         item = (FieldDefinition *) PyList_GetItem( list, i );
         if ( item == NULL ) {
            PyMem_Free( fields );
            return NULL;
         }
         
         // Validate that it is a definition!
         if ( ! PyObject_TypeCheck(item, &FieldDefinitionType) ) {
            PyMem_Free( fields );
            PyErr_SetString( PyExc_TypeError, 
               "One item of the list is not a FieldDefinition" );
            return NULL;
         }

         if ( item->name == NULL ) {
            PyMem_Free( fields );
            SetException( PSO_INVALID_FIELD_NAME );
            return NULL;
         }
         strncpy( fields[i].name, PyString_AsString(item->name), 
            PSO_MAX_FIELD_LENGTH );
         fields[i].type = item->intType;
         fields[i].length = item->length;
         fields[i].minLength = item->minLength;
         fields[i].maxLength = item->maxLength;
         fields[i].precision = item->precision;
         fields[i].scale = item->scale;
      }
   }
   
   memset( &definition, 0, sizeof(psoBasicObjectDef) );
   definition.type = baseDef->intType;
   definition.numFields = baseDef->numFields;
   key = (KeyDefinition *) baseDef->keyDef;
   if ( key != NULL ) {
      definition.key.type      = key->intType;
      definition.key.length    = key->length;
      definition.key.minLength = key->minLength;
      definition.key.maxLength = key->maxLength;
   }
   errcode = psoCreateObject( (PSO_HANDLE)self->handle,
                              objectName,
                              (psoUint32)strlen(objectName),
                              &definition,
                              fields );
   if ( fields ) PyMem_Free( fields );
   if ( errcode != 0 ) {
      SetException( errcode );
      return NULL;
   }

   Py_INCREF(Py_None);
   return Py_None;   
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Session_DestroyObject( Session * self, PyObject * args )
{
   int errcode;
   const char * objectName;
   
   if ( !PyArg_ParseTuple(args, "s", &objectName) ) {
      return NULL;
   }
   
   errcode = psoDestroyObject( (PSO_HANDLE)self->handle,
                               objectName,
                               (psoUint32)strlen(objectName) );
   if ( errcode != 0 ) {
      SetException( errcode );
      return NULL;
   }

   Py_INCREF(Py_None);
   return Py_None;   
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Session_ErrorMsg( Session * self )
{
   int errcode;
   char message[1024];
   PyObject * str = NULL;
   
   errcode = psoErrorMsg( (PSO_HANDLE)self->handle,
                          message,
                          1024 );
   if ( errcode != 0 ) {
      SetException( errcode );
      return NULL;
   }

# if PY_MAJOR_VERSION == 2 && PY_MINOR_VERSION < 6
   str = PyString_FromString( message );
#else
   str = PyBytes_FromString( message );
#endif
   return str;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Session_ExitSession( Session * self )
{
   if ( self->handle != 0 ) {
      psoExitSession( (PSO_HANDLE)self->handle );
   }
   self->handle = 0;

   Py_INCREF(Py_None);
   return Py_None;   
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Session_GetDefinition( Session * self, PyObject * args )
{
   int errcode;
   const char * objectName;
   PyObject * list = NULL;
   psoBasicObjectDef definition;
   psoFieldDefinition  * fields = NULL;
   BaseDef * baseDef;
   KeyDefinition * key = NULL;
   
   if ( !PyArg_ParseTuple(args, "s", &objectName) ) {
      return NULL;
   }

   errcode = psoGetDefinition( (PSO_HANDLE)self->handle,
                               objectName,
                               (psoUint32)strlen(objectName),
                               &definition,
                               0,
                               NULL );
   if ( errcode != 0 ) {
      SetException( errcode );
      return NULL;
   }

   fields = (psoFieldDefinition *) 
      PyMem_Malloc(definition.numFields*sizeof(psoFieldDefinition));
   if ( fields == NULL ) return PyErr_NoMemory();

   memset( &definition, 0, sizeof(psoBasicObjectDef) );
   errcode = psoGetDefinition( (PSO_HANDLE)self->handle,
                               objectName,
                               (psoUint32)strlen(objectName),
                               &definition,
                               definition.numFields,
                               fields );
   if ( errcode != 0 ) {
      PyMem_Free( fields );
      SetException( errcode );
      return NULL;
   }

   /* Start building the key definition, if any */
   if ( definition.key.type != 0 ) {
      key = keyDefToObject( &definition.key );
      if ( key == NULL ) {
         PyMem_Free( fields );
         return NULL;
      }
   }
   baseDef = BaseDefToObject( &definition, (PyObject *)key );
   if ( baseDef == NULL ) {
      PyMem_Free( fields );
      return NULL;
   }
   
   list = FieldDefToList( definition.numFields, fields );
   if ( list == NULL ) {
      PyMem_Free( fields );
      return NULL;
   }

   return Py_BuildValue( "OO", baseDef, list );     
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Session_GetInfo( Session * self, PyObject * args )
{
   int errcode;
   Info * pInfo;
   psoInfo info;
   PyObject * compiler = NULL;
   PyObject * compilerVersion = NULL;
   PyObject * platform = NULL;
   PyObject * dllVersion = NULL;
   PyObject * quasarVersion = NULL;
   PyObject * creationTime = NULL;
   
   errcode = psoGetInfo( (PSO_HANDLE)self->handle, &info );
   if ( errcode != 0 ) {
      SetException( errcode );
      return NULL;
   }
   
   compiler        = GetString( info.compiler, 20 );
   if ( compiler == NULL ) goto cleanup;
   compilerVersion = GetString( info.compilerVersion, 10 );
   if ( compilerVersion == NULL ) goto cleanup;
   platform        = GetString( info.platform, 20 );
   if ( platform == NULL ) goto cleanup;
   dllVersion      = GetString( info.dllVersion, 10 );
   if ( dllVersion == NULL ) goto cleanup;
   quasarVersion   = GetString( info.quasarVersion, 10 );
   if ( quasarVersion == NULL ) goto cleanup;
   creationTime    = GetString( info.creationTime, 30 );
   if ( creationTime == NULL ) goto cleanup;

   pInfo = (Info *)PyObject_New(Info, &InfoType);
   if ( pInfo == NULL ) return NULL;

   pInfo->totalSizeInBytes = info.totalSizeInBytes;
   pInfo->allocatedSizeInBytes = info.allocatedSizeInBytes;
   pInfo->numObjects = info.numObjects;
   pInfo->numGroups = info.numGroups;
   pInfo->numMallocs = info.numMallocs;
   pInfo->numFrees = info.numFrees;
   pInfo->largestFreeInBytes = info.largestFreeInBytes;
   pInfo->memoryVersion = info.memoryVersion;
   pInfo->bigEndian = info.bigEndian;
   pInfo->compiler = compiler;
   pInfo->compilerVersion = compilerVersion;
   pInfo->platform = platform;
   pInfo->dllVersion = dllVersion;
   pInfo->quasarVersion = quasarVersion;
   pInfo->creationTime = creationTime;
   
   return (PyObject *)pInfo;
   
cleanup:
   Py_XDECREF( compiler );
   Py_XDECREF( compilerVersion );
   Py_XDECREF( platform );
   Py_XDECREF( dllVersion );
   Py_XDECREF( quasarVersion );
   Py_XDECREF( creationTime );

   return NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Session_GetStatus( Session * self, PyObject * args )
{
   int errcode;
   const char * objectName;
   ObjStatus * pStatusPy;
   psoObjStatus status;
   PyObject * objType = NULL, * objStatus = NULL;
   
   if ( !PyArg_ParseTuple(args, "s", &objectName) ) {
      return NULL;
   }

   errcode = psoGetStatus( (PSO_HANDLE)self->handle,
                           objectName,
                           (psoUint32)strlen(objectName),
                           &status );
   if ( errcode != 0 ) {
      SetException( errcode );
      return NULL;
   }
   
   objType = GetObjectType( status.type );
   if ( objType == NULL ) return NULL;

   objStatus = GetObjectStatus( status.status );
   if ( objStatus == NULL ) {
      Py_XDECREF(objType);
      return NULL;
   }
   
   pStatusPy = (ObjStatus *)PyObject_New(ObjStatus, &ObjStatusType);
   if ( pStatusPy == NULL ) {
      Py_XDECREF(objType);
      Py_XDECREF(objStatus);
      return NULL;
   }
   
   pStatusPy->objType       = objType;
   pStatusPy->status        = objStatus;
   pStatusPy->numBlocks     = status.numBlocks;
   pStatusPy->numBlockGroup = status.numBlockGroup;
   pStatusPy->numDataItem   = status.numDataItem;
   pStatusPy->freeBytes     = status.freeBytes;
   pStatusPy->maxDataLength = status.maxDataLength;
   pStatusPy->maxKeyLength  = status.maxKeyLength;

   return (PyObject *)pStatusPy;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Session_LastError( Session * self )
{
   int errcode;
   
   errcode = psoLastError( (PSO_HANDLE)self->handle );
   
   return Py_BuildValue("i", errcode);
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Session_Rollback( Session * self )
{
   int errcode;
   
   errcode = psoRollback( (PSO_HANDLE)self->handle );
   if ( errcode != 0 ) {
      SetException( errcode );
      return NULL;
   }

   Py_INCREF(Py_None);
   return Py_None;   
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyMemberDef Session_members[] = {
   {"handle", T_INT, offsetof(Session, handle), RO,
    "Session handle"},
   {NULL}  /* Sentinel */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyMethodDef Session_methods[] = {
   { "commit", (PyCFunction)Session_Commit, METH_NOARGS,
     "Commit the current session"
   },
   { "create_object", (PyCFunction)Session_CreateObject, METH_VARARGS,
     "Create a new photon object in shared memory"
   },
   { "destroy_object", (PyCFunction)Session_DestroyObject, METH_VARARGS,
     "Destroy an existing photon object in shared memory"
   },
   { "error_msg", (PyCFunction)Session_ErrorMsg, METH_NOARGS,
     "Return the messages associated with the last error"
   },
   { "exit", (PyCFunction)Session_ExitSession, METH_NOARGS,
     "Terminate the current session"
   },
   { "get_definition", (PyCFunction)Session_GetDefinition, METH_VARARGS,
     "Get the definition of a Photon object"
   },
   { "get_info", (PyCFunction)Session_GetInfo, METH_NOARGS,
     "Return information on the shared-memory system"
   },
   { "get_status", (PyCFunction)Session_GetStatus, METH_VARARGS,
     "Get the status of a Photon object"
   },
   { "last_error", (PyCFunction)Session_LastError, METH_NOARGS,
     "Return the error code of the last error"
   },
   { "rollback", (PyCFunction)Session_Rollback, METH_NOARGS,
     "Rollback the current session"
   },
   {NULL}  /* Sentinel */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyTypeObject SessionType = {
   PyObject_HEAD_INIT(NULL)
   0,                           /*ob_size*/
   "pso.Session",               /*tp_name*/
   sizeof(Session),             /*tp_basicsize*/
   0,                           /*tp_itemsize*/
   Session_dealloc,             /*tp_dealloc*/
   0,                           /*tp_print*/
   0,                           /*tp_getattr*/
   0,                           /*tp_setattr*/
   0,                           /*tp_compare*/
   0,                           /*tp_repr*/
   0,                           /*tp_as_number*/
   0,                           /*tp_as_sequence*/
   0,                           /*tp_as_mapping*/
   0,                           /*tp_hash */
   0,                           /*tp_call*/
   0,                           /*tp_str*/
   0,                           /*tp_getattro*/
   0,                           /*tp_setattro*/
   0,                           /*tp_as_buffer*/
   Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
   "Session objects",           /* tp_doc */
   0,		                       /* tp_traverse */
   0,		                       /* tp_clear */
   0,		                       /* tp_richcompare */
   0,		                       /* tp_weaklistoffset */
   0,		                       /* tp_iter */
   0,		                       /* tp_iternext */
   Session_methods,             /* tp_methods */
   Session_members,             /* tp_members */
   0,                           /* tp_getset */
   0,                           /* tp_base */
   0,                           /* tp_dict */
   0,                           /* tp_descr_get */
   0,                           /* tp_descr_set */
   0,                           /* tp_dictoffset */
   Session_init,                /* tp_init */
   0,                           /* tp_alloc */
   Session_new,                 /* tp_new */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSO_PY_SESSION_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

