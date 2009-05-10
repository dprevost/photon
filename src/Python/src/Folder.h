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

#ifndef PSO_PY_FOLDER_H
#define PSO_PY_FOLDER_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct {
   PyObject_HEAD

   PyObject * name;
   
   /* This is completely private. Should not be put in the members struct */

   /* size_t -> on 64 bits OSes, this int will be 64 bits */
   size_t handle;

   PyObject * session;
   
   /* 
    * Set to one when iter() is called so that we know we need call 
    * GetFirst and not GetNext.
    */
   int iteratorStarted;
   
} pyFolder;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static void
Folder_dealloc( PyObject * self )
{
   pyFolder * folder = (pyFolder *) self;

   if ( folder->handle != 0 ) {
      psoFolderClose( (PSO_HANDLE)folder->handle );
   }

   Py_XDECREF(folder->name);
   Py_XDECREF(folder->session);

   self->ob_type->tp_free( (PyObject *)self );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Folder_new( PyTypeObject * type, PyObject * args, PyObject * kwds )
{
   pyFolder * self;

   self = (pyFolder *)type->tp_alloc( type, 0 );
   if (self != NULL) {
      self->handle = 0;
      self->iteratorStarted = 0;
      self->name = NULL;
      self->session = NULL;
   }

   return (PyObject *)self;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static int
Folder_init( PyObject * self, PyObject * args, PyObject *kwds )
{
   int errcode;
   PSO_HANDLE handle;
   pyFolder * folder = (pyFolder *)self;
   PyObject * session = NULL, * name = NULL, * tmp = NULL;
   const char * folderName = NULL;
   static char *kwlist[] = {"session", "name", NULL};

   if ( ! PyArg_ParseTupleAndKeywords(args, kwds, "|Os", kwlist, 
      &session, &folderName) ) {
      return -1; 
   }

   if (session && folderName) {

      errcode = psoFolderOpen( (PSO_HANDLE)((pySession *)session)->handle,
                               folderName,
                               (psoUint32)strlen(folderName),
                               &handle );
      if ( errcode == 0 ) {
      
         name = PyString_FromString( folderName );
         if ( name == NULL ) {
            psoFolderClose( handle );
            return -1;
         }
         /*
          * Copying the old value of 'name' before decreasing the ref.
          * counter. Likely overkill but safer according to Python
          * documentation.
          */
         tmp = folder->name;
         folder->name = name;
         Py_XDECREF(tmp);
         
         tmp = folder->session;
         folder->session = session;
         Py_XDECREF(tmp);

         folder->handle = (size_t)handle;
         
         return 0;
      }

      SetException( errcode );
      return -1;
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Folder_iter( PyObject * self )
{
   pyFolder * folder = (pyFolder *) self;

   Py_INCREF(self); 
   folder->iteratorStarted = 1;

   return self;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Folder_next( PyObject * self )
{
   int errcode;
   psoFolderEntry entry;
   pyFolder * folder = (pyFolder *) self;
   pyFolderEntry * e;
   PyObject * objType = NULL, * entryName = NULL, * status = NULL;
   
   if ( folder->iteratorStarted == 1 ) {
      folder->iteratorStarted = 0;
      errcode = psoFolderGetFirst( (PSO_HANDLE)folder->handle, &entry );
      if ( errcode == PSO_IS_EMPTY ) {
         PyErr_SetString( PyExc_StopIteration, "" );
         return NULL;
      }
   }
   else {
      errcode = psoFolderGetNext( (PSO_HANDLE)folder->handle, &entry );
      if ( errcode == PSO_REACHED_THE_END ) {
         PyErr_SetString( PyExc_StopIteration, "" );
         return NULL;
      }
   }
   if ( errcode != 0 ) {
      SetException( errcode );
      return NULL;
   }
  
   objType = GetObjectType( entry.type );
   if ( objType == NULL ) return NULL;
   status = GetObjectStatus( entry.status );
   if ( status == NULL ) return NULL;
   
   entryName = PyString_FromStringAndSize( entry.name,
                                           entry.nameLengthInBytes );
   if ( entryName == NULL ) return NULL;

   e = (pyFolderEntry *)FolderEntry_new( &FolderEntryType, NULL, NULL );
   if ( e == NULL ) return NULL;
   
   e->status = status;
   e->nameLength = entry.nameLengthInBytes;
   e->name = entryName;
   e->objType = objType;

   return (PyObject *)e;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Folder_Close( PyObject * self )
{
   int errcode;
   pyFolder * folder = (pyFolder *) self;
   PyObject * tmp;
   
   errcode = psoFolderClose( (PSO_HANDLE)folder->handle );
   if ( errcode != 0 ) {
      SetException( errcode );
      return NULL;
   }
   
   folder->handle = 0;
   tmp = folder->name;
   Py_XDECREF(tmp);
   folder->name = NULL;
   
   Py_INCREF(Py_None);
   return Py_None;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Folder_CreateFolder( PyObject * self, PyObject * args )
{
   int errcode;
   pyFolder * folder = (pyFolder *) self;
   const char * objectName;
   
   if ( !PyArg_ParseTuple(args, "s", &objectName) ) {
      return NULL;
   }
   
   errcode = psoFolderCreateFolder( (PSO_HANDLE)folder->handle,
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
Folder_CreateObject( PyObject * self, PyObject * args )
{
   int errcode;
   pyFolder * folder = (pyFolder *) self;
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

      errcode = psoDataDefOpen( (PSO_HANDLE) ((pySession*)(folder->session))->handle,
                                definitionName,
                                strlen(definitionName),
                                &dataDefHandle );
      if ( errcode != 0 ) {
         SetException( errcode );
         return NULL;
      }

      errcode = psoFolderCreateQueue( (PSO_HANDLE)folder->handle,
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
      errcode = psoFolderCreateQueue( (PSO_HANDLE)folder->handle,
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
Folder_CreateKeyObject( PyObject * self, PyObject * args )
{
   int errcode;
   pyFolder * folder = (pyFolder *) self;
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

      errcode = psoKeyDefOpen( (PSO_HANDLE) ((pySession*)(folder->session))->handle,
                               keyDefName,
                               strlen(keyDefName),
                               &keyDefHandle );
      if ( errcode != 0 ) {
         SetException( errcode );
         return NULL;
      }
      errcode = psoDataDefOpen( (PSO_HANDLE) ((pySession*)(folder->session))->handle,
                                dataDefName,
                                strlen(dataDefName),
                                &dataDefHandle );
      if ( errcode != 0 ) {
         psoKeyDefClose( keyDefHandle );
         SetException( errcode );
         return NULL;
      }

      errcode = psoFolderCreateMap( (PSO_HANDLE)folder->handle,
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

      errcode = psoFolderCreateMap( (PSO_HANDLE)folder->handle,
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
Folder_DestroyObject( PyObject * self, PyObject * args )
{
   int errcode;
   const char * objectName;
   pyFolder * folder = (pyFolder *) self;
   
   if ( !PyArg_ParseTuple(args, "s", &objectName) ) {
      return NULL;
   }
   
   errcode = psoDestroyObject( (PSO_HANDLE)folder->handle,
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
Folder_GetDataDefinition( PyObject * self, PyObject * args )
{
   int errcode;
   const char * objectName;
   pyFolder * folder = (pyFolder *) self;
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

   errcode = psoFolderGetDataDefinition( (PSO_HANDLE)folder->handle,
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
Folder_GetDefinition( PyObject * self, PyObject * args )
{
   int errcode;
   const char * objectName;
   pyFolder * folder = (pyFolder *) self;
   psoObjectDefinition definition;
   pyObjDefinition * pyDef = NULL;

   if ( !PyArg_ParseTuple(args, "s", &objectName) ) {
      return NULL;
   }
   
   memset( &definition, 0, sizeof(psoObjectDefinition) );
   errcode = psoFolderGetDefinition( (PSO_HANDLE)folder->handle,
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
Folder_GetFirst( PyObject * self, PyObject * args )
{
   int errcode;
   psoFolderEntry entry;
   pyFolder * folder = (pyFolder *) self;
   pyFolderEntry * e;
   PyObject * objType = NULL, * entryName = NULL, * status = NULL;

   errcode = psoFolderGetFirst( (PSO_HANDLE)folder->handle, &entry );
   if ( errcode != 0 ) {
      SetException( errcode );
      return NULL;
   }
   
   objType = GetObjectType( entry.type );
   if ( objType == NULL ) return NULL;
   status = GetObjectStatus( entry.status );
   if ( status == NULL ) return NULL;
   
   entryName = PyString_FromStringAndSize( entry.name,
                                           entry.nameLengthInBytes );
   if ( entryName == NULL ) return NULL;

   e = (pyFolderEntry *)FolderEntry_new( &FolderEntryType, NULL, NULL );
   if ( e == NULL ) return NULL;
   
   e->status = status;
   e->nameLength = entry.nameLengthInBytes;
   e->name = entryName;
   e->objType = objType;

   return (PyObject *)e;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Folder_GetKeyDefinition( PyObject * self, PyObject * args )
{
   int errcode;
   const char * objectName;
   pyFolder * folder = (pyFolder *) self;
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

   errcode = psoFolderGetKeyDefinition( (PSO_HANDLE)folder->handle,
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
Folder_GetNext( PyObject * self, PyObject * args )
{
   int errcode;
   psoFolderEntry entry;
   pyFolder * folder = (pyFolder *) self;
   pyFolderEntry * e;
   PyObject * objType = NULL, * entryName = NULL, * status = NULL;

   errcode = psoFolderGetNext( (PSO_HANDLE)folder->handle, &entry );
   if ( errcode != 0 ) {
      SetException( errcode );
      return NULL;
   }
   
   objType = GetObjectType( entry.type );
   if ( objType == NULL ) return NULL;
   status = GetObjectStatus( entry.status );
   if ( status == NULL ) return NULL;
   
   entryName = PyString_FromStringAndSize( entry.name,
                                           entry.nameLengthInBytes );
   if ( entryName == NULL ) return NULL;

   e = (pyFolderEntry *)FolderEntry_new( &FolderEntryType, NULL, NULL );
   if ( e == NULL ) return NULL;
   
   e->status = status;
   e->nameLength = entry.nameLengthInBytes;
   e->name = entryName;
   e->objType = objType;

   return (PyObject *)e;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Folder_Open( PyObject * self, PyObject * args )
{
   int errcode;
   pyFolder * folder = (pyFolder *) self;
   const char * folderName;
   PSO_HANDLE handle;
   PyObject * name = NULL, *session = NULL, * tmp = NULL;
   
   if ( !PyArg_ParseTuple(args, "Os", &session, &folderName) ) {
      return NULL;
   }

   if ( folder->handle ) {
      SetException( PSO_ALREADY_OPEN );
      return NULL;
   }

   errcode = psoFolderOpen( (PSO_HANDLE)((pySession *)session)->handle,
                            folderName,
                            (psoUint32)strlen(folderName),
                            &handle );
   if ( errcode == 0 ) {

      name = PyString_FromString( folderName );
      if ( name == NULL ) {
         psoFolderClose( handle );
         return NULL;
      }
      tmp = folder->name;
      folder->name = name;
      Py_XDECREF(tmp);

      tmp = folder->session;
      folder->session = session;
      Py_XDECREF(tmp);

      folder->handle = (size_t)handle;

      Py_INCREF(Py_None);
      return Py_None;
   }
   SetException( errcode );

   return NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Folder_Status( PyObject * self )
{
   int errcode;
   pyObjStatus * pStatusPy;
   psoObjStatus status;
   pyFolder * folder = (pyFolder *) self;
   PyObject * objType = NULL, * objStatus = NULL;
   
   errcode = psoFolderStatus( (PSO_HANDLE)folder->handle, &status );
   if ( errcode != 0 ) {
      SetException( errcode );
      return NULL;
   }

fprintf( stderr, "TYPE = %d\n", status.type );
   objType = GetObjectType( status.type );
   if ( objType == NULL ) return NULL;

   objStatus = GetObjectStatus( status.status );
   if ( objStatus == NULL ) return NULL;

   pStatusPy = (pyObjStatus *)PyObject_New(pyObjStatus, &ObjStatusType);
   if ( pStatusPy == NULL ) return NULL;
   
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
Folder_str( PyObject * self )
{
   pyFolder * folder = (pyFolder *) self;
   
   if ( folder->name != NULL ) {
      return PyString_FromFormat( 
         "Folder{ name: %s }",
         PyString_AsString(folder->name) );
   }
   
   return PyString_FromString("Folder is not open");
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyMemberDef Folder_members[] = {
   { "name", T_OBJECT_EX, offsetof(pyFolder, name), RO,
     "The name of the folder"},
   {NULL}  /* Sentinel */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyMethodDef Folder_methods[] = {
   { "close", (PyCFunction)Folder_Close, METH_NOARGS,
     "Close the current folder"
   },
   { "create_folder", (PyCFunction)Folder_CreateFolder, METH_VARARGS,
     "Create a new photon folder as a child of the current folder"
   },
   { "create_object", (PyCFunction)Folder_CreateObject, METH_VARARGS,
     "Create a new photon object as a child of the current folder"
   },
   { "create_key_object", (PyCFunction)Folder_CreateKeyObject, METH_VARARGS,
     "Create a new photon keyed object as a child of the current folder"
   },
   { "destroy_object", (PyCFunction)Folder_DestroyObject, METH_VARARGS,
     "Destroy an existing photon object (child of the current folder)"
   },
   { "get_data_definition", (PyCFunction)Folder_GetDataDefinition, METH_VARARGS,
     "Get the data definition of a Photon object"
   },
   { "get_definition", (PyCFunction)Folder_GetDefinition, METH_VARARGS,
     "Get the definition of a Photon object"
   },
   { "get_first", (PyCFunction)Folder_GetFirst, METH_VARARGS,
     ""
   },
   { "get_key_definition", (PyCFunction)Folder_GetKeyDefinition, METH_VARARGS,
     "Get the key definition of a Photon object"
   },
   { "get_next", (PyCFunction)Folder_GetNext, METH_VARARGS,
     ""
   },
   { "open", (PyCFunction)Folder_Open, METH_VARARGS,
     "Open (get access to) a Photon folder"
   },
   { "status", (PyCFunction)Folder_Status, METH_NOARGS,
     "Return the error code of the last error"
   },
   {NULL}  /* Sentinel */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyTypeObject FolderType = {
   PyObject_HEAD_INIT(NULL)
   0,                          /*ob_size*/
   "pso.Folder",               /*tp_name*/
   sizeof(pyFolder),           /*tp_basicsize*/
   0,                          /*tp_itemsize*/
   Folder_dealloc,             /*tp_dealloc*/
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
   Folder_str,                 /*tp_str*/
   0,                          /*tp_getattro*/
   0,                          /*tp_setattro*/
   0,                          /*tp_as_buffer*/
   Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_ITER, /*tp_flags*/
   "Folder objects",           /* tp_doc */
   0,		                      /* tp_traverse */
   0,		                      /* tp_clear */
   0,		                      /* tp_richcompare */
   0,		                      /* tp_weaklistoffset */
   Folder_iter,                /* tp_iter */
   Folder_next,                /* tp_iternext */
   Folder_methods,             /* tp_methods */
   Folder_members,             /* tp_members */
   0,                          /* tp_getset */
   0,                          /* tp_base */
   0,                          /* tp_dict */
   0,                          /* tp_descr_get */
   0,                          /* tp_descr_set */
   0,                          /* tp_dictoffset */
   Folder_init,                /* tp_init */
   0,                          /* tp_alloc */
   Folder_new,                 /* tp_new */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSO_PY_FOLDER_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

