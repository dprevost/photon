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

extern
int psoaDataDefGetDef( PSO_HANDLE                definitionHandle,
                       enum psoDefinitionType  * type,
                       unsigned char          ** dataDef,
                       psoUint32               * dataDefLength );

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
   PyObject * session = NULL, * name = NULL, *dataDefObj = NULL, * tmp = NULL;
   static char *kwlist[] = {"session", "name", "definition_type", "data_definition", "definition_length", NULL};
   int type, length, errcode;
   PyObject * defType = NULL;
   unsigned char  * dataDef;
   char * definitionName;
   
   if ( ! PyArg_ParseTupleAndKeywords(args, kwds, "OS|iOi", kwlist, 
      &session, &name, &type, &dataDefObj, &length) ) {
      return -1; 
   }
   
   def->sessionHandle = ((Session *)session)->handle;

   tmp = def->name;
   Py_INCREF(name);
   def->name = name;
   Py_XDECREF(tmp);

   definitionName = PyString_AsString(def->name);
   
   if ( dataDef == NULL ) {
      errcode = psoDataDefOpen( (PSO_HANDLE)def->sessionHandle,
                                definitionName,
                                strlen(definitionName),
                                (PSO_HANDLE*)&def->definitionHandle );
      if ( errcode != 0 ) return -1;
      
      errcode = psoaDataDefGetDef( (PSO_HANDLE)def->sessionHandle,
                                   (enum psoDefinitionType *)&type,
                                   &dataDef,
                                   (unsigned int *)&length );
      if ( errcode != 0 ) return -1;
      
      dataDefObj = PyBuffer_FromMemory( dataDef, length ); 
   }
   else {
      errcode = psoDataDefCreate( (PSO_HANDLE)def->sessionHandle,
                                  definitionName,
                                  strlen(definitionName),
                                  type,
                                  dataDef,
                                  length,
                                  (PSO_HANDLE*)&def->definitionHandle );
   }
   
   if ( errcode != 0 ) return -1;

   defType = GetDefinitionType( type ); // A new reference
   if ( defType == NULL ) return -1;
   tmp = def->defType;
   def->defType = defType;
   Py_XDECREF(tmp);
   
   tmp = def->dataDef;
   Py_INCREF(dataDefObj);
   def->dataDef = dataDefObj;
   Py_XDECREF(tmp);

   def->dataDefLength = length;
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
DataDefinition_str( PyObject * self )
{
   DataDefinition * def = (DataDefinition *)self;

   if ( def->name && def->defType ) {
      return PyString_FromFormat( 
         "DataDefinition{ name: %s, field_type: %s }",
         PyString_AsString(def->name),
         PyString_AsString(def->defType) );
   }

   return PyString_FromString("DataDefinition is not set");
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
DataDefinition_Close( DataDefinition * self, PyObject * args )
{
   int errcode;

   errcode = psoDataDefClose( (PSO_HANDLE)self->definitionHandle );
   if ( errcode != 0 ) {
      SetException( errcode );
      return NULL;
   }

   self->sessionHandle = self->definitionHandle = 0;

   Py_INCREF(Py_None);
   return Py_None;   
}
 
static PyObject *
Create( Session                & session,
                const std::string        name,
                enum psoDefinitionType   type,
                const unsigned char    * dataDef,
                psoUint32                dataDefLength );


//std::string GetNext();


void GetDefinition( unsigned char * buffer,
                       psoUint32       bufferLength );

psoUint32 GetLength();


enum psoDefinitionType GetType();
   

void Open( Session & session, const std::string name );


static PyMemberDef DataDefinition_members[] = {
   { "name", T_OBJECT_EX, offsetof(DataDefinition, name), RO,
     "Name of the field" },
   { "definition_type", T_OBJECT_EX, offsetof(DataDefinition, defType), RO,
     "Type of definition" },
   { "definition_handle", T_INT, offsetof(DataDefinition, definitionHandle), RO,
     "Handle to the definition" },
   { "session_handle", T_INT, offsetof(DataDefinition, sessionHandle), RO,
     "Handle to the session we belong to" },
   { "data_definition", T_OBJECT_EX, offsetof(DataDefinition, dataDef), RO,
     "Buffer containing the definition" },
   { "length", T_INT, offsetof(DataDefinition, dataDefLength), RO,
     "Length of the buffer definition" },
   {NULL}  /* Sentinel */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyMethodDef Session_methods[] = {
   { "commit", (PyCFunction)Session_Commit, METH_NOARGS,
     "Commit the current session"
   },
   { "create_object", (PyCFunction)Session_CreateObject, METH_VARARGS,
     "Create a new photon object in shared memory"
   },
   { "destroy_object", (PyCFunction)Session_DestroyObject, METH_VARARGS,
     "Destroy an existing photon object in shared memory"
   },
   { "error_msg", (PyCFunction)Session_ErrorMsg, METH_NOARGS,
     "Return the messages associated with the last error"
   },
   { "exit", (PyCFunction)Session_ExitSession, METH_NOARGS,
     "Terminate the current session"
   },
   { "get_definition", (PyCFunction)Session_GetDefinition, METH_VARARGS,
     "Get the definition of a Photon object"
   },
   { "get_info", (PyCFunction)Session_GetInfo, METH_NOARGS,
     "Return information on the shared-memory system"
   },
   { "get_status", (PyCFunction)Session_GetStatus, METH_VARARGS,
     "Get the status of a Photon object"
   },
   { "last_error", (PyCFunction)Session_LastError, METH_NOARGS,
     "Return the error code of the last error"
   },
   { "rollback", (PyCFunction)Session_Rollback, METH_NOARGS,
     "Rollback the current session"
   },
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

#endif /* PSO_PY_DATA_DEFINITION_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
