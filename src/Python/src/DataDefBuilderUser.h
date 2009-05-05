/*
 * Copyright (C) 2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#ifndef PSO_PY_DATA_DEF_BUILDER_USER_H
#define PSO_PY_DATA_DEF_BUILDER_USER_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct {
   PyObject_HEAD

   /// The C struct array holding the definition of the fields
   unsigned char * fields;

   /// Number of fields in the definition
   unsigned int numFields;

   /// Iterator
   unsigned int currentField;

   /// true if the definition type is odbc simple, false otherwise.
   int currentLength;

} pyDataDefBuilderUser;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static void
DataDefBuilderUser_dealloc( PyObject * self )
{
   pyDataDefBuilderUser * builder = (pyDataDefBuilderUser *)self;
   
   if ( builder->fields == NULL ) free( builder->fields );
   builder->fields = NULL;

   self->ob_type->tp_free( self );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
DataDefBuilderUser_new( PyTypeObject * type, PyObject * args, PyObject * kwds )
{
   pyDataDefBuilderUser * self;

   self = (pyDataDefBuilderUser *)type->tp_alloc( type, 0 );
   if (self != NULL) {
      self->fields = NULL;
      self->numFields = 0;
      self->currentField = 0;
      self->currentLength = 0;
   }

   return (PyObject *)self;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static int
DataDefBuilderUser_init( PyObject * self, PyObject * args, PyObject * kwds )
{
   pyDataDefBuilderUser * builder = (pyDataDefBuilderUser *)self;
   static char *kwlist[] = {"num_fields"};
   unsigned int numFields;
   
   if ( ! PyArg_ParseTupleAndKeywords(args, kwds, "I", kwlist, &numFields) ) {
      return -1; 
   }
   
   builder->numFields = numFields;
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
DataDefBuilderUser_AddField( PyObject * self, PyObject * args )
{
   pyDataDefBuilderUser * builder = (pyDataDefBuilderUser *)self;
   unsigned char * tmp = NULL;
   size_t length;
   char * fieldDescriptor;

   if ( ! PyArg_ParseTuple(args, "s", &fieldDescriptor) ) {
      return NULL; 
   }

   if ( builder->currentField >= builder->numFields ) {
      SetException( PSO_INVALID_FIELD_NAME );
      return NULL;
   }

   length = strlen(fieldDescriptor);
   if ( strlen(fieldDescriptor) == 0 ) {
      SetException( PSO_INVALID_LENGTH );
      return NULL;
   }

   if ( builder->currentLength == 0 ) {
      tmp = calloc( strlen(fieldDescriptor), 1 );
      if ( tmp == NULL ) {
         SetException( PSO_NOT_ENOUGH_HEAP_MEMORY );
         return NULL;
      }
   }
   else {
      length = builder->currentLength + strlen(fieldDescriptor) + 1;
      tmp = calloc( length, 1 );
      if ( tmp == NULL ) {
         SetException( PSO_NOT_ENOUGH_HEAP_MEMORY );
         return NULL;
      }
      memcpy( tmp, builder->fields, builder->currentLength );
      tmp[builder->currentLength] = '\0';
      builder->currentLength++;
      free( builder->fields );
   }
   memcpy( &tmp[builder->currentLength], fieldDescriptor, strlen(fieldDescriptor) );

   builder->currentField++;
   builder->fields = tmp;
   builder->currentLength = length;
   
   Py_INCREF(Py_None);
   return Py_None;   
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
DataDefBuilderUser_GetDefinition( PyObject * self )
{
   pyDataDefBuilderUser * builder = (pyDataDefBuilderUser *)self;

   if ( builder->currentField != builder->numFields || builder->fields == NULL) {
      SetException( PSO_INVALID_NUM_FIELDS );
      return NULL;
   }

   return PyBuffer_FromMemory( builder->fields, builder->currentLength ); 
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyMemberDef DataDefBuilderUser_members[] = {
   { "fields", T_OBJECT_EX, offsetof(pyDataDefBuilderUser, fields), RO,
     "Byte array of the data definition" },
   { "num_fields", T_UINT, offsetof(pyDataDefBuilderUser, numFields), RO,
     "Type of definition" },
   {NULL}  /* Sentinel */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyMethodDef DataDefBuilderUser_methods[] = {
   { "add_field", (PyCFunction)DataDefBuilderUser_AddField, METH_VARARGS,
     "Add the definition of a field to the data definition"
   },
   { "get_definition", (PyCFunction)DataDefBuilderUser_GetDefinition, METH_NOARGS,
     "Retrieve the definition as an array of bytes"
   },
   {NULL}  /* Sentinel */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyTypeObject DataDefBuilderUserType = {
   PyObject_HEAD_INIT(NULL)
   0,                             /*ob_size*/
   "pso.DataDefBuilderUser",      /*tp_name*/
   sizeof(pyDataDefBuilderUser),  /*tp_basicsize*/
   0,                             /*tp_itemsize*/
   DataDefBuilderUser_dealloc,    /*tp_dealloc*/
   0,                             /*tp_print*/
   0,                             /*tp_getattr*/
   0,                             /*tp_setattr*/
   0,                             /*tp_compare*/
   0,                             /*tp_repr*/
   0,                             /*tp_as_number*/
   0,                             /*tp_as_sequence*/
   0,                             /*tp_as_mapping*/
   0,                             /*tp_hash */
   0,                             /*tp_call*/
   0,                             /*tp_str*/
   0,                             /*tp_getattro*/
   0,                             /*tp_setattro*/
   0,                             /*tp_as_buffer*/
   Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
   "Key Definition of a photon object (if any)", /* tp_doc */
   0,		                         /* tp_traverse */
   0,		                         /* tp_clear */
   0,		                         /* tp_richcompare */
   0,		                         /* tp_weaklistoffset */
   0,		                         /* tp_iter */
   0,		                         /* tp_iternext */
   DataDefBuilderUser_methods,    /* tp_methods */
   DataDefBuilderUser_members,    /* tp_members */
   0,                             /* tp_getset */
   0,                             /* tp_base */
   0,                             /* tp_dict */
   0,                             /* tp_descr_get */
   0,                             /* tp_descr_set */
   0,                             /* tp_dictoffset */
   DataDefBuilderUser_init,       /* tp_init */
   0,                             /* tp_alloc */
   DataDefBuilderUser_new,        /* tp_new */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSO_PY_DATA_DEF_BUILDER_USER_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
