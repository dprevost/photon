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

#ifndef PSO_PY_KEY_DEF_BUILDER_ODBC_H
#define PSO_PY_KEY_DEF_BUILDER_ODBC_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct {
   PyObject_HEAD

   /// The C struct array holding the definition of the fields
   psoKeyFieldDefinition * keyFields;

   /// Number of fields in the definition
   unsigned int numKeyFields;

   /// Iterator
   unsigned int currentKeyField;

   /// true if the definition type is odbc simple, false otherwise.
   PyObject * simpleDef;

   int simpleDefInt;

} pyKeyDefBuilderODBC;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static void
KeyDefBuilderODBC_dealloc( PyObject * self )
{
   pyKeyDefBuilderODBC * builder = (pyKeyDefBuilderODBC *)self;
   
   if ( builder->keyFields == NULL ) free( builder->keyFields );
   builder->keyFields = NULL;

   Py_XDECREF( builder->simpleDef );
   self->ob_type->tp_free( self );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
KeyDefBuilderODBC_new( PyTypeObject * type, PyObject * args, PyObject * kwds )
{
   pyKeyDefBuilderODBC * self;

   self = (pyKeyDefBuilderODBC *)type->tp_alloc( type, 0 );
   if (self != NULL) {
      self->keyFields = NULL;
      self->numKeyFields = 0;
      self->currentKeyField = 0;
      self->simpleDef = NULL;
      self->simpleDefInt = 1;
   }

   return (PyObject *)self;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static int
KeyDefBuilderODBC_init( PyObject * self, PyObject * args, PyObject * kwds )
{
   pyKeyDefBuilderODBC * builder = (pyKeyDefBuilderODBC *)self;
   PyObject * tmp = NULL;
   static char *kwlist[] = {"num_fields", "simple" };
   unsigned int numFields;
   int simple = 1;
   
   if ( ! PyArg_ParseTupleAndKeywords(args, kwds, "I|i", kwlist, 
      &numFields, &simple) ) {
      return -1; 
   }
   
   if ( numFields <= 0 || numFields > PSO_MAX_FIELDS ) {
      SetException( PSO_INVALID_NUM_FIELDS );
      return -1; 
   }

   if ( builder->keyFields != NULL ) free( builder->keyFields );
   builder->keyFields = calloc( numFields * sizeof(psoKeyFieldDefinition), 1 );
   if ( builder->keyFields == NULL ) {
      SetException( PSO_NOT_ENOUGH_HEAP_MEMORY );
      return -1;
   }

   tmp = builder->simpleDef;
   if ( simple ) {
      Py_INCREF(Py_True);
      builder->simpleDef = Py_True;
   }
   else {
      Py_INCREF(Py_False);
      builder->simpleDef = Py_False;
   }
   Py_XDECREF(tmp);

   builder->numKeyFields = numFields;
   builder->simpleDefInt = simple;
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
KeyDefBuilderODBC_AddField( PyObject * self, PyObject * args )
{
   pyKeyDefBuilderODBC * builder = (pyKeyDefBuilderODBC *)self;
   char * name;
   enum psoKeyType type;
   unsigned int length;

   if ( ! PyArg_ParseTuple(args, "siI", &name, &type, &length) ) {
      return NULL; 
   }

   if ( strlen(name) == 0 || strlen(name) > PSO_MAX_FIELD_LENGTH ) {
      SetException( PSO_INVALID_FIELD_NAME );
      return NULL;
   }

   memcpy( builder->keyFields[builder->currentKeyField].name, name, strlen(name) );
   
   switch ( type ) {
   case PSO_KEY_INTEGER:
   case PSO_KEY_BIGINT:
   case PSO_KEY_DATE:
   case PSO_KEY_TIME:
   case PSO_KEY_TIMESTAMP:
      builder->keyFields[builder->currentKeyField].type = type;
      builder->keyFields[builder->currentKeyField].length = 0;
      builder->currentKeyField++;
      break;

   case PSO_KEY_BINARY:
   case PSO_KEY_CHAR:
      if ( length == 0 ) {
         SetException( PSO_INVALID_FIELD_LENGTH );
         return NULL;
      }
      builder->keyFields[builder->currentKeyField].type = type;
      builder->keyFields[builder->currentKeyField].length = length;
      builder->currentKeyField++;
      break;

   case PSO_KEY_VARBINARY:
   case PSO_KEY_VARCHAR:
      if ( builder->simpleDefInt && builder->currentKeyField != builder->numKeyFields-1 ) {
         SetException( PSO_INVALID_FIELD_TYPE );
         return NULL;
      }
      if ( length == 0 ) {
         SetException( PSO_INVALID_FIELD_LENGTH );
         return NULL;
      }
      builder->keyFields[builder->currentKeyField].type = type;
      builder->keyFields[builder->currentKeyField].length = length;
      builder->currentKeyField++;
      break;

   case PSO_KEY_LONGVARBINARY:
   case PSO_KEY_LONGVARCHAR:
      if ( builder->simpleDefInt && builder->currentKeyField != builder->numKeyFields-1 ) {
         SetException( PSO_INVALID_FIELD_TYPE );
         return NULL;
      }
      builder->keyFields[builder->currentKeyField].type = type;
      builder->keyFields[builder->currentKeyField].length = 0;
      builder->currentKeyField++;
      break;

   default:
      SetException( PSO_INVALID_FIELD_TYPE );
      return NULL;
   }
   
   Py_INCREF(Py_None);
   return Py_None;   
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
KeyDefBuilderODBC_GetDefinition( PyObject * self )
{
   pyKeyDefBuilderODBC * builder = (pyKeyDefBuilderODBC *)self;

   if ( builder->currentKeyField != builder->numKeyFields || 
      builder->keyFields == NULL) {
      SetException( PSO_INVALID_NUM_FIELDS );
      return NULL;
   }

   return PyBuffer_FromMemory( builder->keyFields, 
      builder->numKeyFields*sizeof(psoKeyFieldDefinition) ); 
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyMemberDef KeyDefBuilderODBC_members[] = {
   { "key_fields", T_OBJECT_EX, offsetof(pyKeyDefBuilderODBC, keyFields), RO,
     "Byte array of the data definition" },
   { "num_key_fields", T_UINT, offsetof(pyKeyDefBuilderODBC, numKeyFields), RO,
     "Type of definition" },
   { "simple", T_OBJECT_EX, offsetof(pyKeyDefBuilderODBC, simpleDef), RO,
     "A boolean - is the definition a simple type or not" },
   {NULL}  /* Sentinel */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyMethodDef KeyDefBuilderODBC_methods[] = {
   { "add_field", (PyCFunction)KeyDefBuilderODBC_AddField, METH_VARARGS,
     "Add the definition of a field to the data definition"
   },
   { "get_definition", (PyCFunction)KeyDefBuilderODBC_GetDefinition, METH_NOARGS,
     "Retrieve the definition as an array of bytes"
   },
   {NULL}  /* Sentinel */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyTypeObject KeyDefBuilderODBCType = {
   PyObject_HEAD_INIT(NULL)
   0,                             /*ob_size*/
   "pso.KeyDefBuilderODBC",       /*tp_name*/
   sizeof(pyKeyDefBuilderODBC),   /*tp_basicsize*/
   0,                             /*tp_itemsize*/
   KeyDefBuilderODBC_dealloc,     /*tp_dealloc*/
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
   KeyDefBuilderODBC_methods,     /* tp_methods */
   KeyDefBuilderODBC_members,     /* tp_members */
   0,                             /* tp_getset */
   0,                             /* tp_base */
   0,                             /* tp_dict */
   0,                             /* tp_descr_get */
   0,                             /* tp_descr_set */
   0,                             /* tp_dictoffset */
   KeyDefBuilderODBC_init,        /* tp_init */
   0,                             /* tp_alloc */
   KeyDefBuilderODBC_new,         /* tp_new */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSO_PY_KEY_DEF_BUILDER_ODBC_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
