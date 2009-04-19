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

#ifndef PSO_PY_DATA_DEFINITION_H
#define PSO_PY_DATA_DEFINITION_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct {
   PyObject_HEAD

   /* size_t -> on 64 bits OSes, this int will be 64 bits */
   size_t definitionHandle;
   size_t sessionHandle;
   PyObject * name;

   PyObject * defType;
   
   /* dataDef is a PyBufferObject */
   PyObject * dataDef;
   int        dataDefLength;
   
   /* This is completely private. Should not be put in the members struct */
   int intType; /* The definition type, as an integer. */

   int currentLength;

} DataDefinition;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static void
DataDefinition_dealloc( PyObject * self )
{
   DataDefinition * def = (DataDefinition *)self;
   
   Py_XDECREF( def->name );
   Py_XDECREF( def->defType );
   Py_XDECREF( def->dataDef );
   self->ob_type->tp_free( self );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
DataDefinition_new( PyTypeObject * type, PyObject * args, PyObject * kwds )
{
   DataDefinition * self;

   self = (DataDefinition *)type->tp_alloc( type, 0 );
   if (self != NULL) {
      self->name = NULL;
      self->definitionHandle = 0;
      self->sessionHandle = 0;
      self->defType = NULL;
      self->dataDef = NULL;
      self->dataDefLength = 0;
   
      self->intType = 0;
      self->currentLength = 0;
   }

   return (PyObject *)self;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static int
DataDefinition_init( PyObject * self, PyObject * args, PyObject * kwds )
{
   DataDefinition * def = (DataDefinition *)self;
   PyObject * session = NULL, * name = NULL, *dataDef = NULL, * tmp = NULL;
   static char *kwlist[] = {"session", "name", "definition_type", "data_definition", "definition_length", NULL};
   int type, length;
   PyObject * defType = NULL;
   
   if ( ! PyArg_ParseTupleAndKeywords(args, kwds, "OS|iOi", kwlist, 
      &session, &name, &type, &dataDef, &length) ) {
      return -1; 
   }
   
   def->sessionHandle = ((Session *)session)->handle;

   //    DataDefinition( Session                & session,
   //                const std::string        name,
     //              enum psoDefinitionType   type,
       //            const unsigned char    * dataDef,
         //          psoUint32                dataDefLength );

   defType = GetDefinitionType( type );
   if ( defType == NULL ) return -1;
   tmp = def->defType;
   def->defType = defType;
   Py_XDECREF(tmp);
   
   tmp = def->name;
   Py_INCREF(name);
   def->name = name;
   Py_XDECREF(tmp);

   tmp = def->dataDef;
   Py_INCREF(dataDef);
   def->dataDef = dataDef;
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
DataDefinition_str( PyObject * self )
{
   DataDefinition * def = (DataDefinition *)self;

   if ( def->name && def->fieldType ) {
      return PyString_FromFormat( 
         "DataDefinition{ "
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

   return PyString_FromString("DataDefinition is not set");
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyMemberDef DataDefinition_members[] = {
   { "name", T_OBJECT_EX, offsetof(DataDefinition, name), RO,
     "Name of the field" },
   { "field_type", T_OBJECT_EX, offsetof(DataDefinition, fieldType), RO,
     "Type of field" },
   { "length", T_INT, offsetof(DataDefinition, length), RO,
     "Length of the key (if type is fixed length)" },
   { "min_length", T_INT, offsetof(DataDefinition, minLength), RO,
     "Minimum length of the key (if type is variable length)" },
   { "max_length", T_INT, offsetof(DataDefinition, maxLength), RO,
     "Maximum length of the key (if type is variable length)" },
   { "precision", T_INT, offsetof(DataDefinition, precision), RO,
     "Precision (used by decimal type)" },
   { "scale", T_INT, offsetof(DataDefinition, scale), RO,
     "Scale (used by decimal type)" },
   {NULL}  /* Sentinel */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyTypeObject DataDefinitionType = {
   PyObject_HEAD_INIT(NULL)
   0,                          /*ob_size*/
   "pso.DataDefinition",       /*tp_name*/
   sizeof(DataDefinition),     /*tp_basicsize*/
   0,                          /*tp_itemsize*/
   DataDefinition_dealloc,     /*tp_dealloc*/
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
   DataDefinition_str,         /*tp_str*/
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
   DataDefinition_members,     /* tp_members */
   0,                          /* tp_getset */
   0,                          /* tp_base */
   0,                          /* tp_dict */
   0,                          /* tp_descr_get */
   0,                          /* tp_descr_set */
   0,                          /* tp_dictoffset */
   DataDefinition_init,        /* tp_init */
   0,                          /* tp_alloc */
   DataDefinition_new,         /* tp_new */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject * DataDefToList( int number, psoFieldDefinition * fields )
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

#endif /* PSO_PY_DATA_DEFINITION_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
