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
} Process;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static void
Process_dealloc( Process * self )
{
   self->ob_type->tp_free( (PyObject*)self );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Process_new( PyTypeObject * type, PyObject * args, PyObject * kwds )
{
   Process * self;

   self = (Process *)type->tp_alloc( type, 0 );

   return (PyObject *)self;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Process_exit( Process * self )
{
   psoExit();
   
   return Py_None;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Process_init( Process * self, PyObject * args )
{
   int errcode;
   const char * quasarAddress;
   int protectionNeeded;

   if ( !PyArg_ParseTuple(args, "si", &quasarAddress, &protectionNeeded) ) {
      return NULL;
   }
   errcode = psoInit( quasarAddress, protectionNeeded );
   
   return Py_BuildValue( "i", errcode );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyMethodDef Process_methods[] = {
   { "init", (PyCFunction)Process_init, METH_VARARGS,
     "Initialize the photon library"
   },
   { "exit", (PyCFunction)Process_exit, METH_NOARGS,
     "Terminates access to the photon library"
   },
   {NULL}  /* Sentinel */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyTypeObject ProcessType = {
   PyObject_HEAD_INIT(NULL)
   0,                           /*ob_size*/
   "photon.Process",            /*tp_name*/
   sizeof(Process),             /*tp_basicsize*/
   0,                           /*tp_itemsize*/
   (destructor)Process_dealloc, /*tp_dealloc*/
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
   "Photon process object",     /* tp_doc */
   0,		                     /* tp_traverse */
   0,		                     /* tp_clear */
   0,		                     /* tp_richcompare */
   0,		                     /* tp_weaklistoffset */
   0,		                     /* tp_iter */
   0,		                     /* tp_iternext */
   Process_methods,             /* tp_methods */
   0,                           /* tp_members */
   0,                           /* tp_getset */
   0,                           /* tp_base */
   0,                           /* tp_dict */
   0,                           /* tp_descr_get */
   0,                           /* tp_descr_set */
   0,                           /* tp_dictoffset */
   0,                           /* tp_init */
   0,                           /* tp_alloc */
   Process_new,                 /* tp_new */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
#if 0

static PyMethodDef module_methods[] = {
   {NULL}  /* Sentinel */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC
initProcess(void) 
{
   PyObject * m;

   if (PyType_Ready(&ProcessType) < 0) return;

   m = Py_InitModule3( "Process", 
                       module_methods,
                       "The Process module for Photon.");

   if (m == NULL) return;

   Py_INCREF( &ProcessType );
   PyModule_AddObject( m, "Process", (PyObject *)&ProcessType );
}
#endif
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

