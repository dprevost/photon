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

   PyObject * name;
   PyObject * objType;
   int        status;
   int        nameLength;
} FolderEntry;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
FolderEntry_str( FolderEntry * obj )
{
   if ( obj->name && obj->objType ) {
      return PyString_FromFormat( 
         "FolderEntry{name:\%s, obj_type:\%s, status:\%d, nameLength:\%d}",
         PyString_AsString(obj->name),
         PyString_AsString(obj->objType),
         obj->status,
         obj->nameLength );
   }
   return PyString_FromString("FolderEntry is not set");
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyMemberDef FolderEntry_members[] = {
   { "name", T_OBJECT_EX, offsetof(FolderEntry, name), RO,
     "The type of the object"},
   { "obj_type", T_OBJECT_EX, offsetof(FolderEntry, objType), RO,
     "Status of the object"},
   { "status", T_INT, offsetof(FolderEntry, status), RO,
     "Status of the object"},
   { "name_length", T_INT, offsetof(FolderEntry, nameLength), RO,
     "Status of the object"},
   {NULL}  /* Sentinel */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyTypeObject FolderEntryType = {
   PyObject_HEAD_INIT(NULL)
   0,                          /*ob_size*/
   "pso.FolderEntry",          /*tp_name*/
   sizeof(FolderEntry),        /*tp_basicsize*/
   0,                          /*tp_itemsize*/
   0,                          /*tp_dealloc*/
   0,                          /*tp_print*/
   0,                          /*tp_getattr*/
   0,                          /*tp_setattr*/
   0,                          /*tp_compare*/
   0,                          /*tp_repr*/
   0,                          /*tp_as_number*/
   0,                          /*tp_as_sequence*/
   0,                          /*tp_as_mapping*/
   0,                          /*tp_hash */
   0,                          /*tp_call*/
   FolderEntry_str,                          /*tp_str*/
   0,                          /*tp_getattro*/
   0,                          /*tp_setattro*/
   0,                          /*tp_as_buffer*/
   Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
   "An entry in a folder of photon", /* tp_doc */
   0,		                      /* tp_traverse */
   0,		                      /* tp_clear */
   0,		                      /* tp_richcompare */
   0,		                      /* tp_weaklistoffset */
   0,		                      /* tp_iter */
   0,		                      /* tp_iternext */
   0,                          /* tp_methods */
   FolderEntry_members,        /* tp_members */
   0,                          /* tp_getset */
   0,                          /* tp_base */
   0,                          /* tp_dict */
   0,                          /* tp_descr_get */
   0,                          /* tp_descr_set */
   0,                          /* tp_dictoffset */
   0,                          /* tp_init */
   0,                          /* tp_alloc */
   0,                          /* tp_new */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
