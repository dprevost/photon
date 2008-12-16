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

typedef struct {
   PyObject_HEAD
   /* size_t -> on 64 bits OSes, this int will be 64 bits */
   size_t handle;
} Session;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static void
Session_dealloc( Session * self )
{
   if ( self->handle != 0 ) {
      psoExitSession( (PSO_HANDLE)self->handle );
   }
   
   self->ob_type->tp_free( (PyObject *)self );
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
Session_init( PyTypeObject * self, PyObject * args, PyObject *kwds )
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
   
   return Py_BuildValue("i", errcode);
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Session_CreateObject( Session * self, PyObject * args )
{
   int errcode;
   const char * objectName;
   PyObject * def, * l;
   psoBasicObjectDef definition;
   psoFieldDefinition  * fields = NULL;
   
   if ( !PyArg_ParseTuple(args, "soo", &objectName, &def, &l) ) {
      return NULL;
   }
   
//   return Py_BuildValue("i", errcode);
   return NULL;
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
   
   return Py_BuildValue("i", errcode);
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
   if ( errcode == 0 ) {
# if PY_MAJOR_VERSION == 2 && PY_MINOR_VERSION < 6
      str = PyString_FromString( message );
#else
      str = PyBytes_FromString( message );
#endif
      return str;
   }
   
   return Py_BuildValue("i", errcode);
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
Session_GetDefinition(  Session * self, PyObject * args );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Session_GetInfo( Session * self, PyObject * args );

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
   if ( objStatus == NULL ) return NULL;

   pStatusPy = (ObjStatus *)PyObject_New(ObjStatus, &ObjStatusType);
   if ( pStatusPy == NULL ) return NULL;
   
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
   
   return Py_BuildValue("i", errcode);
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
   (destructor)Session_dealloc, /*tp_dealloc*/
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
   0,		                     /* tp_traverse */
   0,		                     /* tp_clear */
   0,		                     /* tp_richcompare */
   0,		                     /* tp_weaklistoffset */
   0,		                     /* tp_iter */
   0,		                     /* tp_iternext */
   Session_methods,             /* tp_methods */
   Session_members,             /* tp_members */
   0,                           /* tp_getset */
   0,                           /* tp_base */
   0,                           /* tp_dict */
   0,                           /* tp_descr_get */
   0,                           /* tp_descr_set */
   0,                           /* tp_dictoffset */
   (initproc)Session_init,      /* tp_init */
   0,                           /* tp_alloc */
   Session_new,                 /* tp_new */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

