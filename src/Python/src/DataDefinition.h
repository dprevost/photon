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
                       char                   ** name,
                       psoUint32               * nameLength,
                       enum psoDefinitionType  * type,
                       unsigned char          ** dataDef,
                       psoUint32               * dataDefLength );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct {
   PyObject_HEAD

   /* size_t -> on 64 bits OSes, this int will be 64 bits */
   size_t definitionHandle;
   
   PyObject * name;

   PyObject * defType;
   
   /* dataDef is a PyBufferObject */
   PyObject * dataDef;
   
   unsigned int dataDefLength;
   
   /* This is completely private. Should not be put in the members struct */
   int intType; /* The definition type, as an integer. */

   unsigned int currentLength;

} pyDataDefinition;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static void
DataDefinition_dealloc( PyObject * self )
{
   pyDataDefinition * def = (pyDataDefinition *)self;
   
   Py_XDECREF( def->name );
   Py_XDECREF( def->defType );
   Py_XDECREF( def->dataDef );
   self->ob_type->tp_free( self );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
DataDefinition_new( PyTypeObject * type, PyObject * args, PyObject * kwds )
{
   pyDataDefinition * self;

   self = (pyDataDefinition *)type->tp_alloc( type, 0 );
   if (self != NULL) {
      self->name = NULL;
      self->definitionHandle = 0;
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
   pyDataDefinition * def = (pyDataDefinition *)self;
   PyObject * session = NULL, * name = NULL, *dataDefObj = NULL, * tmp = NULL;
   static char *kwlist[] = {"session", "name", "definition_type", "data_definition", "definition_length", NULL};
   int type, length, errcode;
   PyObject * defType = NULL;
   unsigned char  * dataDef;
   char * definitionName;
   PSO_HANDLE definitionHandle;
   char * dummyName;
   uint32_t dummyLength;
   
   if ( ! PyArg_ParseTupleAndKeywords(args, kwds, "OS|iOi", kwlist, 
      &session, &name, &type, &dataDefObj, &length) ) {
      return -1; 
   }
   
   definitionName = PyString_AsString(name);
   
   if ( dataDef == NULL ) {
      errcode = psoDataDefOpen( (PSO_HANDLE)((pySession *)session)->handle,
                                definitionName,
                                strlen(definitionName),
                                &definitionHandle );
      if ( errcode != 0 ) return -1;
      
      errcode = psoaDataDefGetDef( definitionHandle,
                                   &dummyName,
                                   &dummyLength,
                                   (enum psoDefinitionType *)&type,
                                   &dataDef,
                                   (unsigned int *)&length );
      if ( errcode != 0 ) {
         psoDataDefClose( definitionHandle );
         return -1;
      }
      
      dataDefObj = PyBuffer_FromMemory( dataDef, length ); 
      if ( dataDefObj == NULL ) {
         psoDataDefClose( definitionHandle );
         return -1;
      }
   }
   else {
      // This interface is deprecated in python version 3.0
      errcode = PyObject_AsCharBuffer(	dataDefObj, (const char **)&dataDef, &length );
      if ( errcode != 0 ) return -1;

      errcode = psoDataDefCreate( (PSO_HANDLE)((pySession *)session)->handle,
                                  definitionName,
                                  strlen(definitionName),
                                  type,
                                  dataDef,
                                  length,
                                  (PSO_HANDLE*)&def->definitionHandle );
      if ( errcode != 0 ) return -1;
   }
   
   defType = GetDefinitionType( type ); // A new reference
   if ( defType == NULL ) {
      psoDataDefClose( definitionHandle );
      return -1;
   }
   tmp = def->defType;
   def->defType = defType;
   Py_XDECREF(tmp);
   def->intType = type;
   
   tmp = def->name;
   Py_INCREF(name);
   def->name = name;
   Py_XDECREF(tmp);

   tmp = def->dataDef;
   Py_INCREF(dataDefObj);
   def->dataDef = dataDefObj;
   Py_XDECREF(tmp);

   def->dataDefLength = length;
   def->definitionHandle = (size_t)definitionHandle;
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
DataDefinition_str( PyObject * self )
{
   pyDataDefinition * def = (pyDataDefinition *)self;

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
DataDefinition_Close( pyDataDefinition * self )
{
   int errcode;

   errcode = psoDataDefClose( (PSO_HANDLE)self->definitionHandle );
   if ( errcode != 0 ) {
      SetException( errcode );
      return NULL;
   }

   self->definitionHandle = 0;

   Py_INCREF(Py_None);
   return Py_None;   
}
 
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
DataDefinition_Create( pyDataDefinition * self, PyObject * args )
{
   PyObject * session = NULL, * name = NULL, *dataDefObj = NULL, * tmp = NULL;
   int type, length, errcode;
   PyObject * defType = NULL;
   unsigned char  * dataDef;
   char * definitionName;
   PSO_HANDLE definitionHandle;

   if ( ! PyArg_ParseTuple(args, "OSiOi", 
      &session, &name, &type, &dataDefObj, &length) ) {
      return NULL; 
   }

   definitionName = PyString_AsString(name);
   
   // This interface is deprecated in python version 3.0
   errcode = PyObject_AsCharBuffer(	dataDefObj, (const char **)&dataDef, &length );
   if ( errcode != 0 ) return NULL;
   
   errcode = psoDataDefCreate( (PSO_HANDLE)((pySession *)session)->handle,
                               definitionName,
                               strlen(definitionName),
                               type,
                               dataDef,
                               length,
                               &definitionHandle );
   if ( errcode != 0 ) {
      SetException( errcode );
      return NULL;
   }

   defType = GetDefinitionType( type ); // A new reference
   if ( defType == NULL ) {
      psoDataDefClose( definitionHandle );
      SetException( 666 );
      return NULL;
   }
   tmp = self->defType;
   self->defType = defType;
   Py_XDECREF(tmp);
   self->intType = type;

   tmp = self->name;
   Py_INCREF(name);
   self->name = name;
   Py_XDECREF(tmp);
   
   tmp = self->dataDef;
   Py_INCREF(dataDefObj);
   self->dataDef = dataDefObj;
   Py_XDECREF(tmp);

   self->dataDefLength = length;
   self->definitionHandle = (size_t) definitionHandle;

   Py_INCREF(Py_None);
   return Py_None;   
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

//std::string 
static PyObject *
DataDefinition_GetNext( pyDataDefinition * self )
{
   int length, errcode;
   unsigned char  * dataDef;
   char msg[1000];
   int i, count = 0;
   
   // This interface is deprecated in python version 3.0
   errcode = PyObject_AsCharBuffer(	self->dataDef, (const char **)&dataDef, &length );
   if ( errcode != 0 ) return NULL;
   
   if ( self->currentLength >= length ) {
      self->currentLength = 0;

      Py_INCREF(Py_None);
      return Py_None;   
   }

   if ( self->intType == PSO_DEF_USER_DEFINED ) {
      
      for ( i = self->currentLength; i < length; ++i ) {
         if ( dataDef[i] == 0 ) {
            self->currentLength = i + 1;
            break;
         }
         msg[count] = dataDef[i];
         self->currentLength++;
         count++;
         if ( count == 999 ) {
            // Reallocation would be better here
            msg[count] = 0;
            break;
         }
      }
   }
   else if ( self->intType == PSO_DEF_PHOTON_ODBC_SIMPLE ) {
      /*
       * We don't need to count - msg cannot overflow with this type.
       */
      char name [PSO_MAX_FIELD_LENGTH+1];
      char tmp[20];
      uint32_t currentField = self->currentLength / sizeof(psoFieldDefinition);
      psoFieldDefinition * field = (psoFieldDefinition *)dataDef;
   
      strcpy( msg, "Name: " );
      
      if ( field[currentField].name[PSO_MAX_FIELD_LENGTH-1] == '\0' ) {
         strcat( msg, field[currentField].name );
      }
      else {
         memcpy( name, field[currentField].name, PSO_MAX_FIELD_LENGTH );
         name[PSO_MAX_FIELD_LENGTH] = '\0';
         strcat( msg, name );
      }
      
      strcat( msg, ", Type: " );
      switch ( field[currentField].type ) {

      case PSO_TINYINT:
         strcat( msg, "TinyInt" );
         break;
      case PSO_SMALLINT:
         strcat( msg, "SmallInt" );
         break;
      case PSO_INTEGER:
         strcat( msg, "Integer" );
         break;
      case PSO_BIGINT:
         strcat( msg, "BigInt" );
         break;
      case PSO_REAL:
         strcat( msg, "Real" );
         break;
      case PSO_DOUBLE:
         strcat( msg, "Double" );
         break;
      case PSO_DATE:
         strcat( msg, "Date" );
         break;
      case PSO_TIME:
         strcat( msg, "Time" );
         break;
      case PSO_TIMESTAMP:
         strcat( msg, "TimeStamp" );
         break;

      case PSO_BINARY:
         strcat( msg, "Binary, Length: " );
         sprintf( tmp, "%d", field[currentField].vals.length );
         strcat( msg, tmp );
         break;
      case PSO_CHAR:
         strcat( msg, "Char, Length: " );
         sprintf( tmp, "%d", field[currentField].vals.length );
         strcat( msg, tmp );
         break;

      case PSO_VARBINARY:
         strcat( msg, "VarBinary, Length: " );
         sprintf( tmp, "%d", field[currentField].vals.length );
         strcat( msg, tmp );
         break;

      case PSO_VARCHAR:
         strcat( msg, "VarChar, Length: " );
         sprintf( tmp, "%d", field[currentField].vals.length );
         strcat( msg, tmp );
         break;

      case PSO_LONGVARBINARY:
         strcat( msg, "LongVarBinary" );
         break;
      case PSO_LONGVARCHAR:
         strcat( msg, "LongVarChar" );
         break;

      case PSO_NUMERIC:
         strcat( msg, "Numeric, Precision = " );
         sprintf( tmp, "%d", field[currentField].vals.decimal.precision );
         strcat( msg, tmp );
         strcat( msg, ", Scale = " );
         sprintf( tmp, "%d", field[currentField].vals.decimal.scale );
         strcat( msg, tmp );
         break;

      default:
         strcat( msg, "Invalid/unknown data type" );
      }

      self->currentLength += sizeof(psoFieldDefinition);
   }

   return PyString_FromString( msg );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
DataDefinition_Open( pyDataDefinition * self, PyObject * args )
{
   PyObject * session = NULL, * name = NULL, *dataDefObj = NULL, * tmp = NULL;
   int type, length, errcode;
   PyObject * defType = NULL;
   unsigned char  * dataDef;
   char * definitionName;
   PSO_HANDLE definitionHandle;
   char * dummyName;
   uint32_t dummyLength;
   
   if ( !PyArg_ParseTuple(args, "OS", &session, &name) ) {
      return NULL;
   }

   definitionName = PyString_AsString(name);
   
   errcode = psoDataDefOpen( (PSO_HANDLE)((pySession *)session)->handle,
                             definitionName,
                             strlen(definitionName),
                             &definitionHandle );
   if ( errcode != 0 ) {
      SetException( errcode );
      return NULL;
   }
      
   errcode = psoaDataDefGetDef( definitionHandle,
                                &dummyName,
                                &dummyLength,
                                (enum psoDefinitionType *)&type,
                                &dataDef,
                                (unsigned int *)&length );
   if ( errcode != 0 ) {
      psoDataDefClose( definitionHandle );
      SetException( errcode );
      return NULL;
   }

   dataDefObj = PyBuffer_FromMemory( dataDef, length ); 
   if ( dataDefObj == NULL ) {
      psoDataDefClose( definitionHandle );
      return NULL;
   }

   defType = GetDefinitionType( type ); // A new reference
   if ( defType == NULL ) {
      psoDataDefClose( definitionHandle );
      Py_XDECREF(dataDefObj);
      return NULL;
   }
   tmp = self->defType;
   self->defType = defType;
   Py_XDECREF(tmp);
   self->intType = type;
   
   tmp = self->dataDef;
   Py_INCREF(dataDefObj);
   self->dataDef = dataDefObj;
   Py_XDECREF(tmp);

   tmp = self->name;
   Py_INCREF(name);
   self->name = name;
   Py_XDECREF(tmp);

   self->dataDefLength = length;
   self->definitionHandle = (size_t) definitionHandle;
   
   Py_INCREF(Py_None);
   return Py_None;   
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyMemberDef DataDefinition_members[] = {
   { "name", T_OBJECT_EX, offsetof(pyDataDefinition, name), RO,
     "Name of the data definition" },
   { "definition_type", T_OBJECT_EX, offsetof(pyDataDefinition, defType), RO,
     "Type of definition" },
   { "data_definition", T_OBJECT_EX, offsetof(pyDataDefinition, dataDef), RO,
     "Buffer containing the definition" },
   { "length", T_UINT, offsetof(pyDataDefinition, dataDefLength), RO,
     "Length of the buffer definition" },
   {NULL}  /* Sentinel */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyMethodDef DataDefinition_methods[] = {
   { "close", (PyCFunction)DataDefinition_Close, METH_NOARGS,
     "Close the current data definition object"
   },
   { "create", (PyCFunction)DataDefinition_Create, METH_VARARGS,
     "Create a new data definition in shared memory"
   },
   { "get_next", (PyCFunction)DataDefinition_GetNext, METH_NOARGS,
     "Display the definition of each field"
   },
   { "open", (PyCFunction)DataDefinition_Open, METH_VARARGS,
     "Open an existing data definition in shared memory"
   },
   {NULL}  /* Sentinel */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyTypeObject DataDefinitionType = {
   PyObject_HEAD_INIT(NULL)
   0,                          /*ob_size*/
   "pso.DataDefinition",       /*tp_name*/
   sizeof(pyDataDefinition),   /*tp_basicsize*/
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
   DataDefinition_methods,     /* tp_methods */
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
