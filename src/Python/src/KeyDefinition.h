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

#ifndef PSO_PY_KEY_DEFINITION_H
#define PSO_PY_KEY_DEFINITION_H

extern
int psoaKeyDefGetDef( PSO_HANDLE                definitionHandle,
                      char                   ** name,
                      psoUint32               * nameLength,
                      enum psoDefinitionType  * type,
                      unsigned char          ** keyDef,
                      psoUint32               * keyDefLength );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct {
   PyObject_HEAD

   /* size_t -> on 64 bits OSes, this int will be 64 bits */
   size_t definitionHandle;

   PyObject * name;

   PyObject * defType;
   
   /* keyDef is a PyBufferObject */
   PyObject * keyDef;

   unsigned int keyDefLength;
   
   /* This is completely private. Should not be put in the members struct */
   int intType; /* The definition type, as an integer. */

   unsigned int currentLength;

} pyKeyDefinition;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static void
KeyDefinition_dealloc( PyObject * self )
{
   pyKeyDefinition * def = (pyKeyDefinition *)self;
   
   Py_XDECREF( def->name );
   Py_XDECREF( def->defType );
   Py_XDECREF( def->keyDef );
   self->ob_type->tp_free( self );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
KeyDefinition_new( PyTypeObject * type, PyObject * args, PyObject * kwds )
{
   pyKeyDefinition * self;

   self = (pyKeyDefinition *)type->tp_alloc( type, 0 );
   if (self != NULL) {
      self->name = NULL;
      self->definitionHandle = 0;
      self->defType = NULL;
      self->keyDef = NULL;
      self->keyDefLength = 0;
   
      self->intType = 0;
      self->currentLength = 0;
   }

   return (PyObject *)self;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static int
KeyDefinition_init( PyObject * self, PyObject * args, PyObject * kwds )
{
   pyKeyDefinition * def = (pyKeyDefinition *)self;
   PyObject * session = NULL, * name = NULL, *keyDefObj = NULL, * tmp = NULL;
   static char *kwlist[] = {"session", "name", "definition_type", "key_definition", "definition_length", NULL};
   int type, length, errcode;
   PyObject * defType = NULL;
   unsigned char  * keyDef;
   char * definitionName;
   PSO_HANDLE definitionHandle;
   char * dummyName;
   uint32_t dummyLength;
   
   if ( ! PyArg_ParseTupleAndKeywords(args, kwds, "OS|iOi", kwlist, 
      &session, &name, &type, &keyDefObj, &length) ) {
      return -1; 
   }
   
   definitionName = PyString_AsString(name);
   
   if ( keyDef == NULL ) {
      errcode = psoKeyDefOpen( (PSO_HANDLE)((pySession *)session)->handle,
                                definitionName,
                                strlen(definitionName),
                                &definitionHandle );
      if ( errcode != 0 ) return -1;
      
      errcode = psoaKeyDefGetDef( definitionHandle,
                                  &dummyName,
                                  &dummyLength,
                                  (enum psoDefinitionType *)&type,
                                  &keyDef,
                                  (unsigned int *)&length );
      if ( errcode != 0 ) {
         psoKeyDefClose( definitionHandle );
         return -1;
      }
      
      keyDefObj = PyBuffer_FromMemory( keyDef, length ); 
      if ( keyDefObj == NULL ) {
         psoKeyDefClose( definitionHandle );
         return -1;
      }
   }
   else {
      errcode = PyObject_AsCharBuffer(	keyDefObj, (const char **)&keyDef, &length );
      if ( errcode != 0 ) return -1;

      errcode = psoKeyDefCreate( (PSO_HANDLE)((pySession *)session)->handle,
                                 definitionName,
                                 strlen(definitionName),
                                 type,
                                 keyDef,
                                 length,
                                 (PSO_HANDLE*)&def->definitionHandle );
      if ( errcode != 0 ) return -1;
   }
   

   defType = GetDefinitionType( type ); // A new reference
   if ( defType == NULL ) {
      psoKeyDefClose( definitionHandle );
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

   tmp = def->keyDef;
   Py_INCREF(keyDefObj);
   def->keyDef = keyDefObj;
   Py_XDECREF(tmp);

   def->keyDefLength = length;
   def->definitionHandle = (size_t)definitionHandle;
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
KeyDefinition_str( PyObject * self )
{
   pyKeyDefinition * def = (pyKeyDefinition *)self;

   if ( def->name && def->defType ) {
      return PyString_FromFormat( 
         "KeyDefinition{ name: %s, field_type: %s }",
         PyString_AsString(def->name),
         PyString_AsString(def->defType) );
   }

   return PyString_FromString("KeyDefinition is not set");
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
KeyDefinition_Close( pyKeyDefinition * self )
{
   int errcode;

   errcode = psoKeyDefClose( (PSO_HANDLE)self->definitionHandle );
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
KeyDefinition_Create( pyKeyDefinition * self, PyObject * args )
{
   PyObject * session = NULL, * name = NULL, *keyDefObj = NULL, * tmp = NULL;
   int type, length, errcode;
   PyObject * defType = NULL;
   unsigned char  * keyDef;
   char * definitionName;
   PSO_HANDLE definitionHandle;
   
   if ( ! PyArg_ParseTuple(args, "OSiOi", 
      &session, &name, &type, &keyDefObj, &length) ) {
      return NULL; 
   }

   definitionName = PyString_AsString(name);
   
   errcode = PyObject_AsCharBuffer(	keyDefObj, (const char **)&keyDef, &length );
   if ( errcode != 0 ) return NULL;
   
   errcode = psoKeyDefCreate( (PSO_HANDLE)((pySession *)session)->handle,
                               definitionName,
                               strlen(definitionName),
                               type,
                               keyDef,
                               length,
                               &definitionHandle );
   if ( errcode != 0 ) {
      SetException( errcode );
      return NULL;
   }

   defType = GetDefinitionType( type ); // A new reference
   if ( defType == NULL ) {
      psoKeyDefClose( definitionHandle );
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
   
   tmp = self->keyDef;
   Py_INCREF(keyDefObj);
   self->keyDef = keyDefObj;
   Py_XDECREF(tmp);

   self->keyDefLength = length;
   self->definitionHandle = (size_t) definitionHandle;

   Py_INCREF(Py_None);
   return Py_None;   
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

//std::string 
static PyObject *
KeyDefinition_GetNext(pyKeyDefinition * self )
{
   int length, errcode;
   unsigned char  * keyDef;
   char msg[1000];
   int i, count = 0;
   
   errcode = PyObject_AsCharBuffer(	self->keyDef, (const char **)&keyDef, &length );
   if ( errcode != 0 ) return NULL;
   
   if ( self->currentLength >= length ) {
      self->currentLength = 0;

      Py_INCREF(Py_None);
      return Py_None;   
   }

   if ( self->intType == PSO_DEF_USER_DEFINED ) {
      
      for ( i = self->currentLength; i < length; ++i ) {
         if ( keyDef[i] == 0 ) {
            self->currentLength = i + 1;
            break;
         }
         msg[count] = keyDef[i];
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
      uint32_t currentField = self->currentLength / sizeof(psoKeyFieldDefinition);
      psoKeyFieldDefinition * field = (psoKeyFieldDefinition *)keyDef;
   
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

      case PSO_KEY_INTEGER:
         strcat( msg, "Integer" );
         break;
      case PSO_KEY_BIGINT:
         strcat( msg, "BigInt" );
         break;
      case PSO_KEY_DATE:
         strcat( msg, "Date" );
         break;
      case PSO_KEY_TIME:
         strcat( msg, "Time" );
         break;
      case PSO_KEY_TIMESTAMP:
         strcat( msg, "TimeStamp" );
         break;

      case PSO_KEY_BINARY:
         strcat( msg, "Binary, Length: " );
         sprintf( tmp, "%d", field[currentField].length );
         strcat( msg, tmp );
         break;
      case PSO_KEY_CHAR:
         strcat( msg, "Char, Length: " );
         sprintf( tmp, "%d", field[currentField].length );
         strcat( msg, tmp );
         break;

      case PSO_KEY_VARBINARY:
         strcat( msg, "VarBinary, Length: " );
         sprintf( tmp, "%d", field[currentField].length );
         strcat( msg, tmp );
         break;

      case PSO_KEY_VARCHAR:
         strcat( msg, "VarChar, Length: " );
         sprintf( tmp, "%d", field[currentField].length );
         strcat( msg, tmp );
         break;

      case PSO_KEY_LONGVARBINARY:
         strcat( msg, "LongVarBinary" );
         break;
      case PSO_KEY_LONGVARCHAR:
         strcat( msg, "LongVarChar" );
         break;

      default:
         strcat( msg, "Invalid/unknown key type" );
      }

      self->currentLength += sizeof(psoKeyFieldDefinition);
   }

   return PyString_FromString( msg );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
KeyDefinition_Open( pyKeyDefinition * self, PyObject * args )
{
   PyObject * session = NULL, * name = NULL, *keyDefObj = NULL, * tmp = NULL;
   int type, length, errcode;
   PyObject * defType = NULL;
   unsigned char  * keyDef;
   char * definitionName;
   PSO_HANDLE definitionHandle;
   char * dummyName;
   uint32_t dummyLength;
   
   if ( !PyArg_ParseTuple(args, "OS", &session, &name) ) {
      return NULL;
   }

   definitionName = PyString_AsString(name);
   
   errcode = psoKeyDefOpen( (PSO_HANDLE)((pySession *)session)->handle,
                             definitionName,
                             strlen(definitionName),
                             &definitionHandle );
   if ( errcode != 0 ) {
      SetException( errcode );
      return NULL;
   }
      
   errcode = psoaKeyDefGetDef( definitionHandle,
                               &dummyName,
                               &dummyLength,
                               (enum psoDefinitionType *)&type,
                               &keyDef,
                               (unsigned int *)&length );
   if ( errcode != 0 ) {
      psoKeyDefClose( definitionHandle );
      SetException( errcode );
      return NULL;
   }

   keyDefObj = PyBuffer_FromMemory( keyDef, length ); 
   if ( keyDefObj == NULL ) {
      psoKeyDefClose( definitionHandle );
      return NULL;
   }

   defType = GetDefinitionType( type ); // A new reference
   if ( defType == NULL ) {
      psoKeyDefClose( definitionHandle );
      Py_XDECREF(keyDefObj);
      return NULL;
   }
   tmp = self->defType;
   self->defType = defType;
   Py_XDECREF(tmp);
   self->intType = type;
   
   tmp = self->keyDef;
   Py_INCREF(keyDefObj);
   self->keyDef = keyDefObj;
   Py_XDECREF(tmp);

   tmp = self->name;
   Py_INCREF(name);
   self->name = name;
   Py_XDECREF(tmp);

   self->keyDefLength = length;
   self->definitionHandle = (size_t) definitionHandle;
   
   Py_INCREF(Py_None);
   return Py_None;   
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyMemberDef KeyDefinition_members[] = {
   { "name", T_OBJECT_EX, offsetof(pyKeyDefinition, name), RO,
     "Name of the key definition" },
   { "definition_type", T_OBJECT_EX, offsetof(pyKeyDefinition, defType), RO,
     "Type of definition" },
   { "key_definition", T_OBJECT_EX, offsetof(pyKeyDefinition, keyDef), RO,
     "Buffer containing the definition" },
   { "length", T_UINT, offsetof(pyKeyDefinition, keyDefLength), RO,
     "Length of the buffer definition" },
   {NULL}  /* Sentinel */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyMethodDef KeyDefinition_methods[] = {
   { "close", (PyCFunction)KeyDefinition_Close, METH_NOARGS,
     "Close the current key definition object"
   },
   { "create", (PyCFunction)KeyDefinition_Create, METH_VARARGS,
     "Create a new key definition in shared memory"
   },
   { "get_next", (PyCFunction)KeyDefinition_GetNext, METH_NOARGS,
     "Display the definition of each field"
   },
   { "open", (PyCFunction)KeyDefinition_Open, METH_VARARGS,
     "Open an existing key definition in shared memory"
   },
   {NULL}  /* Sentinel */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyTypeObject KeyDefinitionType = {
   PyObject_HEAD_INIT(NULL)
   0,                          /*ob_size*/
   "pso.KeyDefinition",        /*tp_name*/
   sizeof(pyKeyDefinition),    /*tp_basicsize*/
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
   KeyDefinition_str,          /*tp_str*/
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
   KeyDefinition_methods,      /* tp_methods */
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

#endif /* PSO_PY_KEY_DEFINITION_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
