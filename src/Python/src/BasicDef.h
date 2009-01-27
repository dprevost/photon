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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct {
   PyObject_HEAD

   PyObject * objType;
   int        numFields;
   PyObject * keyDef;

   /* This is completely private. Should not be put in the members struct */
   int intType; /* The type, as an integer. */
   
} BaseDef;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static void
BaseDef_dealloc( PyObject * self )
{
   BaseDef * def = (BaseDef *)self;
   
   Py_XDECREF( def->objType );
   Py_XDECREF( def->keyDef );
   self->ob_type->tp_free( self );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
BaseDef_new( PyTypeObject * type, PyObject * args, PyObject * kwds )
{
   BaseDef * self;

   self = (BaseDef *)type->tp_alloc( type, 0 );
   if (self != NULL) {
      self->objType = NULL;
      self->numFields = 0;
      self->keyDef = NULL;
      self->intType = 0;
   }

   return (PyObject *)self;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static int
BaseDef_init( PyObject * self, PyObject * args, PyObject *kwds )
{
   BaseDef * def = (BaseDef *)self;
   PyObject * key = NULL, * objType = NULL, * tmp = NULL;
   static char *kwlist[] = {"obj_type", "num_fields", "key_def", NULL};
   int type, number;
   
   if ( ! PyArg_ParseTupleAndKeywords(args, kwds, "ii|O", kwlist, 
      &type, &number, &key) ) {
      return -1; 
   }

   objType = GetObjectType( type );
   if ( objType == NULL ) return -1;

   def->objType = objType;
   def->intType = type;
   def->numFields = number;
   
   if ( key ) {
      tmp = def->keyDef;
      Py_INCREF(key);
      def->keyDef = key;
      Py_XDECREF(tmp);
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyMemberDef BaseDef_members[] = {
   { "obj_type", T_OBJECT_EX, offsetof(BaseDef, objType), RO,
     "Status of the object"},
   { "num_fields", T_INT, offsetof(BaseDef, numFields), RO,
     "Status of the object"},
   { "key_definition", T_OBJECT_EX, offsetof(BaseDef, keyDef), RO,
     "The type of the object"},
   {NULL}  /* Sentinel */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyTypeObject BaseDefType = {
   PyObject_HEAD_INIT(NULL)
   0,                          /*ob_size*/
   "pso.BaseDef",              /*tp_name*/
   sizeof(BaseDef),            /*tp_basicsize*/
   0,                          /*tp_itemsize*/
   BaseDef_dealloc,            /*tp_dealloc*/
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
   "Base definition of a Photon object", /* tp_doc */
   0,		                      /* tp_traverse */
   0,		                      /* tp_clear */
   0,		                      /* tp_richcompare */
   0,		                      /* tp_weaklistoffset */
   0,		                      /* tp_iter */
   0,		                      /* tp_iternext */
   0,                          /* tp_methods */
   BaseDef_members,            /* tp_members */
   0,                          /* tp_getset */
   0,                          /* tp_base */
   0,                          /* tp_dict */
   0,                          /* tp_descr_get */
   0,                          /* tp_descr_set */
   0,                          /* tp_dictoffset */
   BaseDef_init,               /* tp_init */
   0,                          /* tp_alloc */
   BaseDef_new,                /* tp_new */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static BaseDef *
BaseDefToObject( psoBasicObjectDef * def, PyObject * key ) 
{
   BaseDef * base = NULL;
   PyObject * objType;
   
   base = (BaseDef *)BaseDef_new(&BaseDefType, NULL, NULL);
   if ( base == NULL ) return NULL;
   
   objType = GetObjectType( def->type );
   if ( objType == NULL ) return NULL;

   base->objType   = objType;
   base->intType   = def->type;
   base->numFields = def->numFields;
   base->keyDef    = key;
   
   return base;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
