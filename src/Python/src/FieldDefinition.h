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

#ifndef PSO_PY_FIELD_DEFINITION_H
#define PSO_PY_FIELD_DEFINITION_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct {
   PyObject_HEAD

   PyObject * name;
   PyObject * fieldType;
   int        length;
   int        minLength;
   int        maxLength;
   int        precision;
   int        scale;
   
   /* This is completely private. Should not be put in the members struct */
   int intType; /* The key type, as an integer. */
} FieldDefinition;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static void
FieldDefinition_dealloc( PyObject * self )
{
   FieldDefinition * def = (FieldDefinition *)self;
   
   Py_XDECREF( def->name );
   Py_XDECREF( def->fieldType );
   self->ob_type->tp_free( self );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
FieldDefinition_new( PyTypeObject * type, PyObject * args, PyObject * kwds )
{
   FieldDefinition * self;

   self = (FieldDefinition *)type->tp_alloc( type, 0 );
   if (self != NULL) {
      self->name = NULL;
      self->fieldType = NULL;
      self->length = 0;
      self->minLength = 0;
      self->maxLength = 0;
      self->precision = 0;
      self->scale = 0;
      self->intType = 0;
   }

   return (PyObject *)self;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static int
FieldDefinition_init( PyObject * self, PyObject * args, PyObject * kwds )
{
   FieldDefinition * def = (FieldDefinition *)self;
   PyObject * fieldType = NULL, * name = NULL, * tmp = NULL;
   static char *kwlist[] = {"name", "field_type", "length", "min_length", "max_length", "precision", "scale", NULL};
   int type, length, minLength, maxLength, precision, scale;
   
   if ( ! PyArg_ParseTupleAndKeywords(args, kwds, "Siiiiii", kwlist, 
      &name, &type, &length, &minLength, &maxLength, &precision, &scale) ) {
      return -1; 
   }
   
   fieldType = GetFieldType( type );
   if ( fieldType == NULL ) return -1;

   tmp = def->name;
   Py_INCREF(name);
   def->name = name;
   Py_XDECREF(tmp);

   def->fieldType = fieldType;
   def->intType = type;
   def->length = length;
   def->minLength = minLength;
   def->maxLength = maxLength;
   def->precision = precision;
   def->scale = scale;
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
FieldDefinition_str( PyObject * self )
{
   FieldDefinition * def = (FieldDefinition *)self;

   if ( def->name && def->fieldType ) {
      return PyString_FromFormat( 
         "FieldDefinition{ "
         "name: %s, "
         "field_type: %s, "
         "length: %d, "
         "min_length: %d, "
         "max_length: %d, "
         "precision: %d, "
         "scale: %d }",
         PyString_AsString(def->name),
         PyString_AsString(def->fieldType),
         def->length,
         def->minLength,
         def->maxLength,
         def->precision,
         def->scale );
   }

   return PyString_FromString("FieldDefinition is not set");
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyMemberDef FieldDefinition_members[] = {
   { "name", T_OBJECT_EX, offsetof(FieldDefinition, name), RO,
     "Name of the field" },
   { "field_type", T_OBJECT_EX, offsetof(FieldDefinition, fieldType), RO,
     "Type of field" },
   { "length", T_INT, offsetof(FieldDefinition, length), RO,
     "Length of the key (if type is fixed length)" },
   { "min_length", T_INT, offsetof(FieldDefinition, minLength), RO,
     "Minimum length of the key (if type is variable length)" },
   { "max_length", T_INT, offsetof(FieldDefinition, maxLength), RO,
     "Maximum length of the key (if type is variable length)" },
   { "precision", T_INT, offsetof(FieldDefinition, precision), RO,
     "Precision (used by decimal type)" },
   { "scale", T_INT, offsetof(FieldDefinition, scale), RO,
     "Scale (used by decimal type)" },
   {NULL}  /* Sentinel */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyTypeObject FieldDefinitionType = {
   PyObject_HEAD_INIT(NULL)
   0,                          /*ob_size*/
   "pso.FieldDefinition",      /*tp_name*/
   sizeof(FieldDefinition),    /*tp_basicsize*/
   0,                          /*tp_itemsize*/
   FieldDefinition_dealloc,    /*tp_dealloc*/
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
   FieldDefinition_str,        /*tp_str*/
   0,                          /*tp_getattro*/
   0,                          /*tp_setattro*/
   0,                          /*tp_as_buffer*/
   Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
   "Key Definition of a photon object (if any)", /* tp_doc */
   0,		                      /* tp_traverse */
   0,		                      /* tp_clear */
   0,		                      /* tp_richcompare */
   0,		                      /* tp_weaklistoffset */
   0,		                      /* tp_iter */
   0,		                      /* tp_iternext */
   0,                          /* tp_methods */
   FieldDefinition_members,    /* tp_members */
   0,                          /* tp_getset */
   0,                          /* tp_base */
   0,                          /* tp_dict */
   0,                          /* tp_descr_get */
   0,                          /* tp_descr_set */
   0,                          /* tp_dictoffset */
   FieldDefinition_init,       /* tp_init */
   0,                          /* tp_alloc */
   FieldDefinition_new,        /* tp_new */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject * FieldDefToList( int number, psoFieldDefinition * fields )
{
   PyObject * list = NULL;
   FieldDefinition * item = NULL;
   int i, errcode;
   
   list = PyList_New(number);
   if (list == NULL) return NULL;
   
   for ( i = 0; i < number; ++i ) {
      item = (FieldDefinition *)
         FieldDefinition_new( &FieldDefinitionType, NULL, NULL );
      if (item == NULL) goto cleanup;
      
      item->fieldType = GetFieldType( fields[i].type );
      if ( item->fieldType == NULL ) goto cleanup;

      item->name = GetString( fields[i].name, PSO_MAX_FIELD_LENGTH );
      if ( item->name == NULL ) goto cleanup;

      item->intType   = fields[i].type;
      item->length    = fields[i].length;
      item->minLength = fields[i].minLength;
      item->maxLength = fields[i].maxLength;
      item->precision = fields[i].precision;
      item->scale     = fields[i].scale;
   
      errcode = PyList_SetItem( list, i, (PyObject *)item );
      item = NULL;
      if ( errcode != 0 ) goto cleanup;
   }
   
   return list;

cleanup:
   if ( item ) {
      Py_XDECREF(item->fieldType);
      Py_XDECREF(item->name);
      Py_XDECREF(item);
   }
   
   for ( i = 0; i < number; ++i ) {
      item = (FieldDefinition *)PyList_GetItem( list, i );
      if ( item ) {
         Py_XDECREF(item->fieldType);
         Py_XDECREF(item->name);
         Py_XDECREF(item);
      }
   }   
   Py_XDECREF(list);
   
   return NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSO_PY_FIELD_DEFINITION_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

