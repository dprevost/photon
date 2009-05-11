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

#ifndef PSO_PY_SESSION_H
#define PSO_PY_SESSION_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Python.h"
#include "structmember.h"

#include <photon/photon.h>

/*
 * The declaration of pySession is in ForwardDeclare.h
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static void
Session_dealloc( PyObject * self )
{
   pySession * session = (pySession *)self;

   if ( session->handle != 0 ) {
      psoExitSession( (PSO_HANDLE)session->handle );
   }
   session->handle = 0;
   
   self->ob_type->tp_free( self );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Session_new( PyTypeObject * type, PyObject * args, PyObject * kwds )
{
   pySession * self;

   self = (pySession *)type->tp_alloc( type, 0 );
   if (self != NULL) {
      self->handle = 0;
   }

   return (PyObject *)self;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static int
Session_init( PyObject * self, PyObject * args, PyObject * kwds )
{
   int errcode;
   PSO_HANDLE handle;
   pySession * session = (pySession *)self;
    
   errcode = psoInitSession( &handle );
   if ( errcode == 0 ) {
      session->handle = (size_t)handle;
      return 0;
   }
   
   PyErr_SetString( PhotonError, "Photon Error!" );
   return -1;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Session_Commit( PyObject * self )
{
   int errcode;
   pySession * session = (pySession *) self;
   
   errcode = psoCommit( (PSO_HANDLE)session->handle );
   if ( errcode != 0 ) {
      SetException( errcode );
      return NULL;
   }

   Py_INCREF(Py_None);
   return Py_None;   
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Session_CreateFolder( PyObject * self, PyObject * args )
{
   int errcode;
   pySession * session = (pySession *) self;
   const char * objectName;
   
   if ( !PyArg_ParseTuple(args, "s", &objectName) ) {
      return NULL;
   }
   
   errcode = psoCreateFolder( (PSO_HANDLE)session->handle,
                              objectName,
                              (psoUint32)strlen(objectName) );
   if ( errcode != 0 ) {
      SetException( errcode );
      return NULL;
   }

   Py_INCREF(Py_None);
   return Py_None;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Session_CreateObject( PyObject * self, PyObject * args )
{
   int errcode;
   pySession * session = (pySession *) self;
   const char * objectName;
   psoObjectDefinition definition;
   pyObjDefinition * pyObjDefinition;
   PyObject * pyObj;
   char * definitionName;
   PSO_HANDLE dataDefHandle;

   if ( !PyArg_ParseTuple( args, "sOO", &objectName, 
                                        &pyObjDefinition,
                                        &pyObj ) ) {
      return NULL;
   }

   definition.type = pyObjDefinition->intType;
   definition.flags = pyObjDefinition->flags;
   definition.minNumOfDataRecords = pyObjDefinition->minNumOfDataRecords;
   definition.minNumBlocks = pyObjDefinition->minNumBlocks;

   if ( PyString_Check( pyObj ) ) {
      definitionName = PyString_AsString(pyObj);

      errcode = psoDataDefOpen( (PSO_HANDLE) session->handle,
                                definitionName,
                                strlen(definitionName),
                                &dataDefHandle );
      if ( errcode != 0 ) {
         SetException( errcode );
         return NULL;
      }

      errcode = psoCreateQueue( (PSO_HANDLE)session->handle,
                                objectName,
                                strlen(objectName),
                                &definition,
                                dataDefHandle );
      psoDataDefClose( dataDefHandle );
      if ( errcode != 0 ) {
         SetException( errcode );
         return NULL;
      }
   }
   else {
      dataDefHandle = (PSO_HANDLE) ((pyDataDefinition *)pyObj)->definitionHandle;
      errcode = psoCreateQueue( (PSO_HANDLE)session->handle,
                                objectName,
                                strlen(objectName),
                                &definition,
                                dataDefHandle );
      if ( errcode != 0 ) {
         SetException( errcode );
         return NULL;
      }
   }

   Py_INCREF(Py_None);
   return Py_None;   
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Session_CreateKeyObject( PyObject * self, PyObject * args )
{
   int errcode;
   pySession * session = (pySession *) self;
   const char * objectName;
   psoObjectDefinition definition;
   pyObjDefinition * pyObjDefinition;
   PyObject * pyKeyObj, * pyDataObj;
   char * dataDefName, * keyDefName;
   PSO_HANDLE dataDefHandle, keyDefHandle;

   if ( !PyArg_ParseTuple( args, "sOOO", &objectName, 
                                         &pyObjDefinition,
                                         &pyKeyObj,
                                         &pyDataObj ) ) {
      return NULL;
   }

   ObjectToObjDefinition( &definition, pyObjDefinition );

   if ( PyString_Check( pyKeyObj ) ) {
      if ( ! PyString_Check( pyDataObj ) ) {
         return NULL;
      }
      dataDefName = PyString_AsString(pyDataObj);
      keyDefName  = PyString_AsString(pyKeyObj);

      errcode = psoKeyDefOpen( (PSO_HANDLE) session->handle,
                               keyDefName,
                               strlen(keyDefName),
                               &keyDefHandle );
      if ( errcode != 0 ) {
         SetException( errcode );
         return NULL;
      }
      errcode = psoDataDefOpen( (PSO_HANDLE) session->handle,
                                dataDefName,
                                strlen(dataDefName),
                                &dataDefHandle );
      if ( errcode != 0 ) {
         psoKeyDefClose( keyDefHandle );
         SetException( errcode );
         return NULL;
      }

      errcode = psoCreateMap( (PSO_HANDLE)session->handle,
                              objectName,
                              strlen(objectName),
                              &definition,
                              dataDefHandle,
                              keyDefHandle );
      psoDataDefClose( dataDefHandle );
      psoKeyDefClose( keyDefHandle );
      if ( errcode != 0 ) {
         SetException( errcode );
         return NULL;
      }
   }
   else {
      if ( PyString_Check( pyDataObj ) ) {
         return NULL;
      }
      dataDefHandle = (PSO_HANDLE) ((pyDataDefinition *)pyDataObj)->definitionHandle;
      keyDefHandle  = (PSO_HANDLE) ((pyKeyDefinition *)pyKeyObj)->definitionHandle;

      errcode = psoCreateMap( (PSO_HANDLE)session->handle,
                              objectName,
                              strlen(objectName),
                              &definition,
                              dataDefHandle,
                              keyDefHandle );
      if ( errcode != 0 ) {
         SetException( errcode );
         return NULL;
      }
   }

   Py_INCREF(Py_None);
   return Py_None;   
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Session_DestroyObject( PyObject * self, PyObject * args )
{
   int errcode;
   const char * objectName;
   pySession * session = (pySession *)self;
   
   if ( !PyArg_ParseTuple(args, "s", &objectName) ) {
      return NULL;
   }
   
   errcode = psoDestroyObject( (PSO_HANDLE)session->handle,
                               objectName,
                               (psoUint32)strlen(objectName) );
   if ( errcode != 0 ) {
      SetException( errcode );
      return NULL;
   }

   Py_INCREF(Py_None);
   return Py_None;   
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Session_ErrorMsg( PyObject * self )
{
   int errcode;
   char message[1024];
   PyObject * str = NULL;
   pySession * session = (pySession *)self;
   
   errcode = psoErrorMsg( (PSO_HANDLE)session->handle,
                          message,
                          1024 );
   if ( errcode != 0 ) {
      SetException( errcode );
      return NULL;
   }

# if PY_MAJOR_VERSION == 2 && PY_MINOR_VERSION < 6
   str = PyString_FromString( message );
#else
   str = PyBytes_FromString( message );
#endif
   return str;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Session_ExitSession( PyObject * self )
{
   pySession * session = (pySession *)self;

   if ( session->handle != 0 ) {
      psoExitSession( (PSO_HANDLE)session->handle );
   }
   session->handle = 0;

   Py_INCREF(Py_None);
   return Py_None;   
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Session_GetDataDefinition( PyObject * self, PyObject * args )
{
   int errcode;
   const char * objectName;
   pySession * session = (pySession *)self;
   pyDataDefinition * pyDef = NULL;
   PSO_HANDLE dataDefHandle;
   int type, length;
   unsigned int nameLength;
   PyObject * defType = NULL;
   unsigned char  * dataDef;
   char * definitionName;
   PyObject * name = NULL, *dataDefObj = NULL, * tmp = NULL;

   if ( !PyArg_ParseTuple(args, "s", &objectName) ) {
      return NULL;
   }

   errcode = psoGetDataDefinition( (PSO_HANDLE)session->handle,
                                   objectName,
                                   strlen(objectName),
                                   &dataDefHandle );
   if ( errcode != 0 ) {
      SetException( errcode );
      return NULL;
   }

   pyDef = (pyDataDefinition *) DataDefinition_new( &DataDefinitionType, 
                                                    NULL, NULL );
   if (pyDef == NULL) return NULL;
   
   errcode = psoaDataDefGetDef( dataDefHandle,
                                &definitionName,
                                &nameLength,
                                (enum psoDefinitionType *)&type,
                                &dataDef,
                                (unsigned int *)&length );
   if ( errcode != 0 ) {
      Py_XDECREF(pyDef);
      SetException( errcode );
      return NULL;
   }

   name = PyString_FromStringAndSize(definitionName, nameLength);
   if ( name == NULL ) {
      Py_XDECREF(pyDef);
      return NULL;
   }

   dataDefObj = PyBuffer_FromMemory( dataDef, length ); 
   if ( dataDefObj == NULL ) {
      Py_XDECREF(pyDef);
      Py_XDECREF(name);
      return NULL;
   }

   defType = GetDefinitionType( type ); // A new reference
   if ( defType == NULL ) {
      Py_XDECREF(pyDef);
      Py_XDECREF(name);
      Py_XDECREF(dataDefObj);
      return NULL;
   }
   tmp = pyDef->defType;
   pyDef->defType = defType;
   Py_XDECREF(tmp);
   pyDef->intType = type;
   
   tmp = pyDef->dataDef;
   Py_INCREF(dataDefObj);
   pyDef->dataDef = dataDefObj;
   Py_XDECREF(tmp);

   tmp = pyDef->name;
   Py_INCREF(name);
   pyDef->name = name;
   Py_XDECREF(tmp);

   pyDef->dataDefLength = length;
   pyDef->definitionHandle = (size_t) dataDefHandle;

   return (PyObject *)pyDef;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Session_GetDefinition( PyObject * self, PyObject * args )
{
   int errcode;
   const char * objectName;
   pySession * session = (pySession *)self;
   psoObjectDefinition definition;
   pyObjDefinition * pyDef = NULL;

   if ( !PyArg_ParseTuple(args, "s", &objectName) ) {
      return NULL;
   }
   
   memset( &definition, 0, sizeof(psoObjectDefinition) );
   errcode = psoGetDefinition( (PSO_HANDLE)session->handle,
                               objectName,
                               strlen(objectName),
                               &definition );
   if ( errcode != 0 ) {
      SetException( errcode );
      return NULL;
   }

   pyDef = ObjDefinitionToObject( &definition );
   if ( pyDef == NULL ) return NULL;
   
   return (PyObject *)pyDef;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Session_GetInfo( PyObject * self, PyObject * args )
{
   int errcode;
   pySession * session = (pySession *)self;
   pyInfo * pInfo;
   psoInfo info;
   PyObject * compiler = NULL;
   PyObject * compilerVersion = NULL;
   PyObject * platform = NULL;
   PyObject * dllVersion = NULL;
   PyObject * quasarVersion = NULL;
   PyObject * creationTime = NULL;
   
   errcode = psoGetInfo( (PSO_HANDLE)session->handle, &info );
   if ( errcode != 0 ) {
      SetException( errcode );
      return NULL;
   }
   
   compiler        = GetString( info.compiler, 20 );
   if ( compiler == NULL ) goto cleanup;
   compilerVersion = GetString( info.compilerVersion, 10 );
   if ( compilerVersion == NULL ) goto cleanup;
   platform        = GetString( info.platform, 20 );
   if ( platform == NULL ) goto cleanup;
   dllVersion      = GetString( info.dllVersion, 10 );
   if ( dllVersion == NULL ) goto cleanup;
   quasarVersion   = GetString( info.quasarVersion, 10 );
   if ( quasarVersion == NULL ) goto cleanup;
   creationTime    = GetString( info.creationTime, 30 );
   if ( creationTime == NULL ) goto cleanup;

   pInfo = (pyInfo *)PyObject_New(pyInfo, &InfoType);
   if ( pInfo == NULL ) return NULL;

   pInfo->totalSizeInBytes = info.totalSizeInBytes;
   pInfo->allocatedSizeInBytes = info.allocatedSizeInBytes;
   pInfo->numObjects = info.numObjects;
   pInfo->numGroups = info.numGroups;
   pInfo->numMallocs = info.numMallocs;
   pInfo->numFrees = info.numFrees;
   pInfo->largestFreeInBytes = info.largestFreeInBytes;
   pInfo->memoryVersion = info.memoryVersion;
   pInfo->bigEndian = info.bigEndian;
   pInfo->compiler = compiler;
   pInfo->compilerVersion = compilerVersion;
   pInfo->platform = platform;
   pInfo->dllVersion = dllVersion;
   pInfo->quasarVersion = quasarVersion;
   pInfo->creationTime = creationTime;
   
   return (PyObject *)pInfo;
   
cleanup:
   Py_XDECREF( compiler );
   Py_XDECREF( compilerVersion );
   Py_XDECREF( platform );
   Py_XDECREF( dllVersion );
   Py_XDECREF( quasarVersion );
   Py_XDECREF( creationTime );

   return NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Session_GetKeyDefinition( PyObject * self, PyObject * args )
{
   int errcode;
   const char * objectName;
   pySession * session = (pySession *)self;
   pyKeyDefinition * pyDef = NULL;
   PSO_HANDLE keyDefHandle;
   int type, length;
   unsigned int nameLength;
   PyObject * defType = NULL;
   unsigned char  * keyDef;
   char * definitionName;
   PyObject * name = NULL, *keyDefObj = NULL, * tmp = NULL;

   if ( !PyArg_ParseTuple(args, "s", &objectName) ) {
      return NULL;
   }

   errcode = psoGetKeyDefinition( (PSO_HANDLE)session->handle,
                                  objectName,
                                  strlen(objectName),
                                  &keyDefHandle );
   if ( errcode != 0 ) {
      SetException( errcode );
      return NULL;
   }

   pyDef = (pyKeyDefinition *) KeyDefinition_new( &KeyDefinitionType, 
                                                  NULL, NULL );
   if (pyDef == NULL) return NULL;
   
   errcode = psoaKeyDefGetDef( keyDefHandle,
                               &definitionName,
                               &nameLength,
                               (enum psoDefinitionType *)&type,
                               &keyDef,
                               (unsigned int *)&length );
   if ( errcode != 0 ) {
      Py_XDECREF(pyDef);
      SetException( errcode );
      return NULL;
   }

   name = PyString_FromStringAndSize(definitionName, nameLength);
   if ( name == NULL ) {
      Py_XDECREF(pyDef);
      return NULL;
   }

   keyDefObj = PyBuffer_FromMemory( keyDef, length ); 
   if ( keyDefObj == NULL ) {
      Py_XDECREF(pyDef);
      Py_XDECREF(name);
      return NULL;
   }

   defType = GetDefinitionType( type ); // A new reference
   if ( defType == NULL ) {
      Py_XDECREF(pyDef);
      Py_XDECREF(name);
      Py_XDECREF(keyDefObj);
      return NULL;
   }
   tmp = pyDef->defType;
   pyDef->defType = defType;
   Py_XDECREF(tmp);
   pyDef->intType = type;
   
   tmp = pyDef->keyDef;
   Py_INCREF(keyDefObj);
   pyDef->keyDef = keyDefObj;
   Py_XDECREF(tmp);

   tmp = pyDef->name;
   Py_INCREF(name);
   pyDef->name = name;
   Py_XDECREF(tmp);

   pyDef->keyDefLength = length;
   pyDef->definitionHandle = (size_t) keyDefHandle;

   return (PyObject *)pyDef;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Session_GetStatus( PyObject * self, PyObject * args )
{
   int errcode;
   const char * objectName;
   pySession * session = (pySession *)self;
   pyObjStatus * pStatusPy;
   psoObjStatus status;
   PyObject * objType = NULL, * objStatus = NULL;
   
   if ( !PyArg_ParseTuple(args, "s", &objectName) ) {
      return NULL;
   }

   errcode = psoGetStatus( (PSO_HANDLE)session->handle,
                           objectName,
                           (psoUint32)strlen(objectName),
                           &status );
   if ( errcode != 0 ) {
      SetException( errcode );
      return NULL;
   }
   
   objType = GetObjectType( status.type );
   if ( objType == NULL ) return NULL;

   objStatus = GetObjectStatus( status.status );
   if ( objStatus == NULL ) {
      Py_XDECREF(objType);
      return NULL;
   }
   
   pStatusPy = (pyObjStatus *)PyObject_New(pyObjStatus, &ObjStatusType);
   if ( pStatusPy == NULL ) {
      Py_XDECREF(objType);
      Py_XDECREF(objStatus);
      return NULL;
   }
   
   pStatusPy->objType       = objType;
   pStatusPy->status        = objStatus;
   pStatusPy->numBlocks     = status.numBlocks;
   pStatusPy->numBlockGroup = status.numBlockGroup;
   pStatusPy->numDataItem   = status.numDataItem;
   pStatusPy->freeBytes     = status.freeBytes;
   pStatusPy->maxDataLength = status.maxDataLength;
   pStatusPy->maxKeyLength  = status.maxKeyLength;

   return (PyObject *)pStatusPy;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Session_LastError( PyObject * self )
{
   int errcode;
   pySession * session = (pySession *)self;
   
   errcode = psoLastError( (PSO_HANDLE)session->handle );
   
   return Py_BuildValue("i", errcode);
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Session_Rollback( PyObject * self )
{
   int errcode;
   pySession * session = (pySession *)self;
   
   errcode = psoRollback( (PSO_HANDLE)session->handle );
   if ( errcode != 0 ) {
      SetException( errcode );
      return NULL;
   }

   Py_INCREF(Py_None);
   return Py_None;   
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyMemberDef Session_members[] = {
   {"handle", T_INT, offsetof(pySession, handle), RO,
    "Session handle"},
   {NULL}  /* Sentinel */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyMethodDef Session_methods[] = {
   { "commit", (PyCFunction)Session_Commit, METH_NOARGS,
     "Commit the current session"
   },
   { "create_folder", (PyCFunction)Session_CreateFolder, METH_VARARGS,
     "Create a new photon folder in shared memory"
   },
   { "create_object", (PyCFunction)Session_CreateObject, METH_VARARGS,
     "Create a new photon object in shared memory"
   },
   { "create_key_object", (PyCFunction)Session_CreateKeyObject, METH_VARARGS,
     "Create a new keyed photon object in shared memory"
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
   { "get_data_definition", (PyCFunction)Session_GetDataDefinition, METH_VARARGS,
     "Get the data definition of a Photon object"
   },
   { "get_definition", (PyCFunction)Session_GetDefinition, METH_VARARGS,
     "Get the definition of a Photon object"
   },
   { "get_info", (PyCFunction)Session_GetInfo, METH_NOARGS,
     "Return information on the shared-memory system"
   },
   { "get_key_definition", (PyCFunction)Session_GetKeyDefinition, METH_VARARGS,
     "Get the key definition of a Photon object"
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

static PyTypeObject SessionType = {
   PyObject_HEAD_INIT(NULL)
   0,                           /*ob_size*/
   "pso.Session",               /*tp_name*/
   sizeof(pySession),           /*tp_basicsize*/
   0,                           /*tp_itemsize*/
   Session_dealloc,             /*tp_dealloc*/
   0,                           /*tp_print*/
   0,                           /*tp_getattr*/
   0,                           /*tp_setattr*/
   0,                           /*tp_compare*/
   0,                           /*tp_repr*/
   0,                           /*tp_as_number*/
   0,                           /*tp_as_sequence*/
   0,                           /*tp_as_mapping*/
   0,                           /*tp_hash */
   0,                           /*tp_call*/
   0,                           /*tp_str*/
   0,                           /*tp_getattro*/
   0,                           /*tp_setattro*/
   0,                           /*tp_as_buffer*/
   Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
   "Session objects",           /* tp_doc */
   0,		                       /* tp_traverse */
   0,		                       /* tp_clear */
   0,		                       /* tp_richcompare */
   0,		                       /* tp_weaklistoffset */
   0,		                       /* tp_iter */
   0,		                       /* tp_iternext */
   Session_methods,             /* tp_methods */
   Session_members,             /* tp_members */
   0,                           /* tp_getset */
   0,                           /* tp_base */
   0,                           /* tp_dict */
   0,                           /* tp_descr_get */
   0,                           /* tp_descr_set */
   0,                           /* tp_dictoffset */
   Session_init,                /* tp_init */
   0,                           /* tp_alloc */
   Session_new,                 /* tp_new */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSO_PY_SESSION_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

