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

#ifndef PSO_PY_OBJ_DEFINITION_H
#define PSO_PY_OBJ_DEFINITION_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct {
   PyObject_HEAD

   PyObject * objType;

   /**
    * Flags defining the properties of the object.
    *
    * Currently defined flags:
    *     - PSO_MULTIPLE_DATA_DEFINITIONS
    *
    * Flags should be ORed.
    */
   unsigned int flags;
   
   /**
    * Optimization feature - not implemented yet.
    *
    * The expected minimum number of data records. This is used internally
    * to avoid shrinking the internal "holder" of the data beyond what is
    * needed to hold this minimum number of data records.
    */
   unsigned long minNumOfDataRecords;
   
   /**
    * Optimization feature - not implemented yet
    *
    * The expected minimum number of blocks. This is used internally
    * to avoid shrinking the shared-memory object beyond a certain predefined
    * minimum size. 
    *
    * Potential issue: the amount of overhead used by Photon will vary;
    * some potential factors includes the type of object, the number of 
    * data records, the length of the data records (and keys, if used).
    *
    * You might want to retrieve the status of the object and evaluate
    * the minimum number of blocks needed from it..
    */
   unsigned long minNumBlocks;
   
   /* This is completely private. Should not be put in the members struct */
   int intType; /* The type, as an integer. */
   
} ObjDefinition;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static void
ObjDefinition_dealloc( PyObject * self )
{
   ObjDefinition * def = (ObjDefinition *)self;
   
   Py_XDECREF( def->objType );
   self->ob_type->tp_free( self );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
ObjDefinition_new( PyTypeObject * type, PyObject * args, PyObject * kwds )
{
   ObjDefinition * self;

   self = (ObjDefinition *)type->tp_alloc( type, 0 );
   if (self != NULL) {
      self->objType = NULL;
      self->flags = 0;
      self->minNumOfDataRecords = 0;
      self->minNumBlocks = 0;
      self->intType = 0;
   }

   return (PyObject *)self;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static int
ObjDefinition_init( PyObject * self, PyObject * args, PyObject *kwds )
{
   ObjDefinition * def = (ObjDefinition *)self;
   PyObject * objType = NULL, * tmp = NULL;
   static char *kwlist[] = { "obj_type", "flags", "min_num_records", 
                             "min_num_blocks", NULL};
   int type, flags = 0, numRecords = 0, numBlocks = 0;
   
   if ( ! PyArg_ParseTupleAndKeywords(args, kwds, "i|ikk", kwlist, 
      &type, &flags, &numRecords, &numBlocks) ) {
      return -1; 
   }

   objType = GetObjectType( type ); // A new reference
   if ( objType == NULL ) return -1;

   // This function can be called twice -> def->objType might already exist!
   tmp = def->objType;
   Py_XDECREF(tmp);
   def->objType = objType;
   
   def->intType = type;
   def->flags = flags;
   def->minNumOfDataRecords = numRecords;
   def->minNumBlocks = numBlocks;
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
ObjDefinition_str( PyObject * self )
{
   ObjDefinition * obj = (ObjDefinition *)self;
   PyObject * outStr;
   
   if ( obj->objType ) {
      outStr = PyString_FromFormat( 
         "ObjDefinition{ obj_type: %s, flags: %u, minNumRecords: %ul, minNumBlocks: %ul }",
         PyString_AsString(obj->objType),
         obj->flags,
         obj->minNumOfDataRecords,
         obj->minNumBlocks );
      return outStr;
   }
   
   return PyString_FromString("ObjDefinition is not set");
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyMemberDef ObjDefinition_members[] = {
   { "obj_type", T_OBJECT_EX, offsetof(ObjDefinition, objType), RO,
     "Status of the object"},
   { "flags", T_UINT, offsetof(ObjDefinition, flags), RO,
     "Creation flags for the object"},
   { "min_num_records", T_ULONG, offsetof(ObjDefinition, minNumOfDataRecords), RO,
     "The expected minimum number of data records"},
   { "min_num_blocks", T_ULONG, offsetof(ObjDefinition, minNumBlocks), RO,
     "The expected minimum number of memory blocks"},
   {NULL}  /* Sentinel */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyTypeObject ObjDefinitionType = {
   PyObject_HEAD_INIT(NULL)
   0,                          /*ob_size*/
   "pso.ObjDefinition",        /*tp_name*/
   sizeof(ObjDefinition),      /*tp_basicsize*/
   0,                          /*tp_itemsize*/
   ObjDefinition_dealloc,      /*tp_dealloc*/
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
   ObjDefinition_str,          /*tp_str*/
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
   ObjDefinition_members,      /* tp_members */
   0,                          /* tp_getset */
   0,                          /* tp_base */
   0,                          /* tp_dict */
   0,                          /* tp_descr_get */
   0,                          /* tp_descr_set */
   0,                          /* tp_dictoffset */
   ObjDefinition_init,         /* tp_init */
   0,                          /* tp_alloc */
   ObjDefinition_new,          /* tp_new */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if 0

is this used? 
static ObjDefinition *
ObjDefinitionToObject( psoObjectDefinition * def, PyObject * key ) 
{
   ObjDefinition * base = NULL;
   PyObject * objType;
   
   base = (ObjDefinition *)ObjDefinition_new(&ObjDefinitionType, NULL, NULL);
   if ( base == NULL ) return NULL;
   
   objType = GetObjectType( def->type );
   if ( objType == NULL ) return NULL;

   base->objType   = objType;
   base->intType   = def->type;
   base->numFields = def->numFields;
   base->keyDef    = key;
   
   return base;
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSO_PY_OBJ_DEFINITION_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

