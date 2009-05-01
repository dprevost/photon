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
 
#ifndef PSO_PY_OBJ_STATUS_H
#define PSO_PY_OBJ_STATUS_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct {
   PyObject_HEAD

   /** The object type. */
   PyObject * objType;

   /* Status of the object. */
   PyObject * status;

   /** The number of blocks allocated to this object. */
   size_t numBlocks;

   /** The number of groups of blocks allocated to this object. */
   size_t numBlockGroup;

   /** The number of data items in this object. */
   size_t numDataItem;
   
   /** The amount of free space available in the blocks allocated to this object. */
   size_t freeBytes;

   /** Maximum data length (in bytes). */
   psoUint32 maxDataLength;
   
   /** Maximum key length (in bytes) if keys are supported - zero otherwise */
   psoUint32 maxKeyLength;

} pyObjStatus;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static void
ObjStatus_dealloc( PyObject * self )
{
   pyObjStatus * status = (pyObjStatus *) self;

   Py_XDECREF(status->objType);
   Py_XDECREF(status->status);

   self->ob_type->tp_free( (PyObject *)self );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
ObjStatus_str( PyObject * self )
{
   pyObjStatus * obj = (pyObjStatus *)self;

   if ( obj->objType && obj->status ) {
      return PyString_FromFormat( 
         "ObjStatus{ obj_type: %s, status: %s, num_blocks: %zu, "
         "num_groups: %zu, num_data_items: %zu, free_bytes: %zu, "
         "max_data_length: %u, max_key_length: %u }",
         PyString_AsString(obj->objType),
         PyString_AsString(obj->status),
         obj->numBlocks,
         obj->numBlockGroup,
         obj->numDataItem,
         obj->freeBytes,
         obj->maxDataLength,
         obj->maxKeyLength );
   }
   
   return PyString_FromString("ObjStatus is not set");
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyMemberDef ObjStatus_members[] = {
   { "obj_type", T_OBJECT_EX, offsetof(pyObjStatus, objType), RO,
     "The type of the object"},
   { "status", T_OBJECT_EX, offsetof(pyObjStatus, status), RO,
     "Status of the object"},
#if SIZEOF_VOID_P == 4
   { "num_blocks", T_UINT, offsetof(pyObjStatus, numBlocks), RO,
     "The number of blocks allocated to this object"},
   { "num_groups", T_UINT, offsetof(pyObjStatus, numBlockGroup), RO,
     "The number of groups of blocks allocated to this object"},
   { "num_data_items", T_UINT, offsetof(pyObjStatus, numDataItem), RO,
     "The number of data items in this object"},
   { "free_bytes", T_UINT, offsetof(pyObjStatus, freeBytes), RO,
     "The amount of free space available in the blocks allocated to this object"},
#else
   /* 
    * On 64 bits machine, long long is a solution. V 2.6 of Python 
    * supports the T_PYSSIZET type - which would be a better solution.
    */
   { "num_blocks", T_ULONGLONG, offsetof(pyObjStatus, numBlocks), RO,
     "The number of blocks allocated to this object"},
   { "num_groups", T_ULONGLONG, offsetof(pyObjStatus, numBlockGroup), RO,
     "The number of groups of blocks allocated to this object"},
   { "num_data_items", T_ULONGLONG, offsetof(pyObjStatus, numDataItem), RO,
     "The number of data items in this object"},
   { "free_bytes", T_ULONGLONG, offsetof(pyObjStatus, freeBytes), RO,
     "The amount of free space available in the blocks allocated to this object"},
#endif
   { "max_data_length", T_UINT, offsetof(pyObjStatus, maxDataLength), RO,
     "Maximum data length (in bytes)"},
   { "max_key_length", T_UINT, offsetof(pyObjStatus, maxKeyLength), RO,
     "Maximum key length (in bytes) if keys are supported - zero otherwise"},
   {NULL}  /* Sentinel */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyTypeObject ObjStatusType = {
   PyObject_HEAD_INIT(NULL)
   0,                          /*ob_size*/
   "pso.ObjStatus",            /*tp_name*/
   sizeof(pyObjStatus),        /*tp_basicsize*/
   0,                          /*tp_itemsize*/
   ObjStatus_dealloc,          /*tp_dealloc*/
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
   ObjStatus_str,              /*tp_str*/
   0,                          /*tp_getattro*/
   0,                          /*tp_setattro*/
   0,                          /*tp_as_buffer*/
   Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
   "Status of Photon objects", /* tp_doc */
   0,		                      /* tp_traverse */
   0,		                      /* tp_clear */
   0,		                      /* tp_richcompare */
   0,		                      /* tp_weaklistoffset */
   0,		                      /* tp_iter */
   0,		                      /* tp_iternext */
   0,                          /* tp_methods */
   ObjStatus_members,          /* tp_members */
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

#endif /* PSO_PY_OBJ_STATUS_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

