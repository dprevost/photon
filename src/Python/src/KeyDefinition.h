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

#ifndef KEY_DEFINITION_H
#define KEY_DEFINITION_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct {
   PyObject_HEAD

   PyObject * keyType;
   int        length;
   int        minLength;
   int        maxLength;

   /* This is completely private. Should not be put in the members struct */
   int intType; /* The key type, as an integer. */
} KeyDefinition;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static void
KeyDefinition_dealloc( PyObject * self )
{
   KeyDefinition * def = (KeyDefinition *)self;
   
   Py_XDECREF( def->keyType );
   self->ob_type->tp_free( self );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
KeyDefinition_new( PyTypeObject * type, PyObject * args, PyObject * kwds )
{
   KeyDefinition * self;

   self = (KeyDefinition *)type->tp_alloc( type, 0 );
   if (self != NULL) {
      self->keyType = NULL;
      self->length = 0;
      self->minLength = 0;
      self->maxLength = 0;
      self->intType = 0;
   }

   return (PyObject *)self;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static int
KeyDefinition_init( PyObject * self, PyObject * args, PyObject *kwds )
{
   KeyDefinition * def = (KeyDefinition *)self;
   PyObject * keyType = NULL;
   static char *kwlist[] = {"key_type", "length", "min_length", "max_length", NULL};
   int type, length, minLength, maxLength;
   
   if ( ! PyArg_ParseTupleAndKeywords(args, kwds, "iiii", kwlist, 
      &type, &length, &minLength, &maxLength) ) {
      return -1; 
   }

   keyType = GetKeyType( type );
   if ( keyType == NULL ) return -1;

   def->keyType = keyType;
   def->intType = type;
   def->length = length;
   def->minLength = minLength;
   def->maxLength = maxLength;
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyMemberDef KeyDefinition_members[] = {
   { "key_type", T_OBJECT_EX, offsetof(KeyDefinition, keyType), RO,
     "Type of key"},
   { "length", T_INT, offsetof(KeyDefinition, length), RO,
     "Length of the key (if type is fixed length)"},
   { "min_length", T_INT, offsetof(KeyDefinition, minLength), RO,
     "Minimum length of the key (if type is variable length)"},
   { "max_length", T_INT, offsetof(KeyDefinition, maxLength), RO,
     "Maximum length of the key (if type is variable length)"},
   {NULL}  /* Sentinel */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyTypeObject KeyDefinitionType = {
   PyObject_HEAD_INIT(NULL)
   0,                          /*ob_size*/
   "pso.KeyDefinition",        /*tp_name*/
   sizeof(KeyDefinition),      /*tp_basicsize*/
   0,                          /*tp_itemsize*/
   KeyDefinition_dealloc,      /*tp_dealloc*/
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
   0,                          /*tp_str*/
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
   KeyDefinition_members,      /* tp_members */
   0,                          /* tp_getset */
   0,                          /* tp_base */
   0,                          /* tp_dict */
   0,                          /* tp_descr_get */
   0,                          /* tp_descr_set */
   0,                          /* tp_dictoffset */
   KeyDefinition_init,         /* tp_init */
   0,                          /* tp_alloc */
   KeyDefinition_new,          /* tp_new */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static KeyDefinition *
keyDefToObject( psoKeyDefinition * def ) 
{
   KeyDefinition * key = NULL;
   PyObject * keyType;
   
   key = (KeyDefinition *)KeyDefinition_new(&KeyDefinitionType, NULL, NULL);
   if ( key == NULL ) return NULL;
      
   keyType = GetKeyType( def->type );
   if ( keyType == NULL ) return NULL;

   key->keyType   = keyType;
   key->intType   = def->type;
   key->length    = def->length;
   key->minLength = def->minLength;
   key->maxLength = def->maxLength;
   
   return key;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* KEY_DEFINITION_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

