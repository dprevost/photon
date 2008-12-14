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
   PSO_HANDLE handle;
} Session;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static void
Session_dealloc( Session * self )
{
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

static PyObject *
Session_commit( Session * self )
{
   int errcode;
   
   errcode = psoCommit( (PSO_HANDLE)self->handle );
   
   return Py_BuildValue("i", errcode);
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Session_createObject(Session * self, PyObject * args )
{
   return Py_None;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyMemberDef Session_members[] = {
   {"handle", T_OBJECT_EX, offsetof(Session, handle), RO,
    "Session handle"},
   {NULL}  /* Sentinel */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyMethodDef Session_methods[] = {
   { "commit", (PyCFunction)Session_commit, METH_NOARGS,
     "Commit the current session"
   },
   { "create_object", (PyCFunction)Session_createObject, METH_VARARGS,
     "Create a new photon object in shared memory"
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
   0,                           /* tp_init */
   0,                           /* tp_alloc */
   Session_new,                 /* tp_new */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

