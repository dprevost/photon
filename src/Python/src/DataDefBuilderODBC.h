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

#ifndef PSO_PY_DATA_DEF_BUILDER_ODBC_H
#define PSO_PY_DATA_DEF_BUILDER_ODBC_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct {
   PyObject_HEAD

   /// The C struct array holding the definition of the fields
   psoFieldDefinition * fields;

   /// Number of fields in the definition
   unsigned int numFields;

   /// Iterator
   unsigned int currentField;

   /// true if the definition type is odbc simple, false otherwise.
   PyObject * simpleDef;

   int simpleDefInt;
   
} pyDataDefBuilderODBC;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static void
DataDefBuilderODBC_dealloc( PyObject * self )
{
   pyDataDefBuilderODBC * builder = (pyDataDefBuilderODBC *)self;
   
   if ( builder->fields == NULL ) free( builder->fields );
   builder->fields = NULL;
   
   Py_XDECREF( builder->simpleDef );
   self->ob_type->tp_free( self );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
DataDefBuilderODBC_new( PyTypeObject * type, PyObject * args, PyObject * kwds )
{
   pyDataDefBuilderODBC * self;

   self = (pyDataDefBuilderODBC *)type->tp_alloc( type, 0 );
   if (self != NULL) {
      self->fields = NULL;
      self->numFields = 0;
      self->currentField = 0;
      self->simpleDef = NULL;
      self->simpleDefInt = 1;
   }

   return (PyObject *)self;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static int
DataDefBuilderODBC_init( PyObject * self, PyObject * args, PyObject * kwds )
{
   pyDataDefBuilderODBC * builder = (pyDataDefBuilderODBC *)self;
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

   if ( builder->fields != NULL ) free( builder->fields );
   builder->fields = calloc( numFields * sizeof(psoFieldDefinition), 1 );
   if ( builder->fields == NULL ) {
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
   
   builder->numFields = numFields;
   builder->simpleDefInt = simple;
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
DataDefBuilderODBC_AddField( PyObject * self, PyObject * args )
{
   pyDataDefBuilderODBC * builder = (pyDataDefBuilderODBC *)self;
   char * name;
   enum psoFieldType type;
   unsigned int length, precision, scale;

   if ( ! PyArg_ParseTuple(args, "siIII", 
      &name, &type, &length, &precision, &scale) ) {
      return NULL; 
   }

   if ( strlen(name) == 0 || strlen(name) > PSO_MAX_FIELD_LENGTH ) {
      SetException( PSO_INVALID_FIELD_NAME );
      return NULL;
   }

   memcpy( builder->fields[builder->currentField].name, name, strlen(name) );
   
   switch ( type ) {
   case PSO_TINYINT:
   case PSO_SMALLINT:
   case PSO_INTEGER:
   case PSO_BIGINT:
   case PSO_REAL:
   case PSO_DOUBLE:
   case PSO_DATE:
   case PSO_TIME:
   case PSO_TIMESTAMP:
      builder->fields[builder->currentField].type = type;
      builder->fields[builder->currentField].vals.length = 0;
      builder->currentField++;
      break;

   case PSO_BINARY:
   case PSO_CHAR:
      if ( length == 0 ) {
         SetException( PSO_INVALID_FIELD_LENGTH );
         return NULL;
      }
      builder->fields[builder->currentField].type = type;
      builder->fields[builder->currentField].vals.length = length;
      builder->currentField++;
      break;

   case PSO_VARBINARY:
   case PSO_VARCHAR:
      if ( builder->simpleDefInt && builder->currentField != builder->numFields-1 ) {
         SetException( PSO_INVALID_FIELD_TYPE );
         return NULL;
      }
      if ( length == 0 ) {
         SetException( PSO_INVALID_FIELD_LENGTH );
         return NULL;
      }
      builder->fields[builder->currentField].type = type;
      builder->fields[builder->currentField].vals.length = length;
      builder->currentField++;
      break;

   case PSO_LONGVARBINARY:
   case PSO_LONGVARCHAR:
      if ( builder->simpleDefInt && builder->currentField != builder->numFields-1 ) {
         SetException( PSO_INVALID_FIELD_TYPE );
         return NULL;
      }
      builder->fields[builder->currentField].type = type;
      builder->fields[builder->currentField].vals.length = 0;
      builder->currentField++;
      break;

   case PSO_NUMERIC:
      if ( precision == 0 || precision > PSO_FIELD_MAX_PRECISION ) {
         SetException( PSO_INVALID_PRECISION );
         return NULL;
      }
      if ( scale > precision ) {
         SetException( PSO_INVALID_SCALE );
         return NULL;
      }
      builder->fields[builder->currentField].type = type;
      builder->fields[builder->currentField].vals.decimal.precision = precision;
      builder->fields[builder->currentField].vals.decimal.scale = scale;
      builder->currentField++;
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
DataDefBuilderODBC_GetDefinition( PyObject * self )
{
   pyDataDefBuilderODBC * builder = (pyDataDefBuilderODBC *)self;

   if ( builder->currentField != builder->numFields || builder->fields == NULL) {
      SetException( PSO_INVALID_NUM_FIELDS );
      return NULL;
   }

   return PyBuffer_FromMemory( builder->fields, 
                               builder->numFields*sizeof(psoFieldDefinition) ); 
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyMemberDef DataDefBuilderODBC_members[] = {
   { "fields", T_OBJECT_EX, offsetof(pyDataDefBuilderODBC, fields), RO,
     "Byte array of the data definition" },
   { "num_fields", T_UINT, offsetof(pyDataDefBuilderODBC, numFields), RO,
     "Type of definition" },
   { "simple", T_OBJECT_EX, offsetof(pyDataDefBuilderODBC, simpleDef), RO,
     "A boolean - is the definition a simple type or not" },
   {NULL}  /* Sentinel */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyMethodDef DataDefBuilderODBC_methods[] = {
   { "add_field", (PyCFunction)DataDefBuilderODBC_AddField, METH_VARARGS,
     "Add the definition of a field to the data definition"
   },
   { "get_definition", (PyCFunction)DataDefBuilderODBC_GetDefinition, METH_NOARGS,
     "Retrieve the definition as an array of bytes"
   },
   {NULL}  /* Sentinel */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyTypeObject DataDefBuilderODBCType = {
   PyObject_HEAD_INIT(NULL)
   0,                             /*ob_size*/
   "pso.DataDefBuilderODBC",      /*tp_name*/
   sizeof(pyDataDefBuilderODBC),  /*tp_basicsize*/
   0,                             /*tp_itemsize*/
   DataDefBuilderODBC_dealloc,    /*tp_dealloc*/
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
   DataDefBuilderODBC_methods,    /* tp_methods */
   DataDefBuilderODBC_members,    /* tp_members */
   0,                             /* tp_getset */
   0,                             /* tp_base */
   0,                             /* tp_dict */
   0,                             /* tp_descr_get */
   0,                             /* tp_descr_set */
   0,                             /* tp_dictoffset */
   DataDefBuilderODBC_init,       /* tp_init */
   0,                             /* tp_alloc */
   DataDefBuilderODBC_new,        /* tp_new */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSO_PY_DATA_DEF_BUILDER_ODBC_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
