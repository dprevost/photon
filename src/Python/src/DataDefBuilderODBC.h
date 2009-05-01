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
   PyObject * fields;

   /// Number of fields in the definition
   unsigned int numFields;

   /// Iterator
   unsigned int currentField;

   /// true if the definition type is odbc simple, false otherwise.
   PyObject * simpleDef;

} pyDataDefBuilederODBC;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static void
DataDefBuilederODBC_dealloc( PyObject * self )
{
   pyDataDefBuilederODBC * builder = (pyDataDefBuilederODBC *)self;
   
   Py_XDECREF( builder->fields );
   Py_XDECREF( builder->simpleDef );
   self->ob_type->tp_free( self );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
DataDefBuilederODBC_new( PyTypeObject * type, PyObject * args, PyObject * kwds )
{
   pyDataDefBuilederODBC * self;

   self = (pyDataDefBuilederODBC *)type->tp_alloc( type, 0 );
   if (self != NULL) {
      self->fields = NULL;
      self->numFields = 0;
      self->currentField = 0;
      self->simpleDef = NULL;
   }

   return (PyObject *)self;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSO_PY_DATA_DEF_BUILDER_ODBC_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
