/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@photonsoftware.org>
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
   /* size_t -> on 64 bits OSes, this int will be 64 bits */
   size_t handle;
   size_t sessionHandle;
   PyObject * name;
   
   /* This is completely private. Should not be put in the members struct */

   /* 
    * Set to one when iter() is called so that we know we need call 
    * GetFirst and not GetNext.
    */
   int iteratorStarted;
   
} Folder;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static void
Folder_dealloc( PyObject * self )
{
   Folder * folder = (Folder *) self;

   if ( folder->handle != 0 ) {
      psoFolderClose( (PSO_HANDLE)folder->handle );
   }

   Py_XDECREF(folder->name);

   self->ob_type->tp_free( (PyObject *)self );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Folder_new( PyTypeObject * type, PyObject * args, PyObject * kwds )
{
   Folder * self;
   PyObject * session = NULL;
   const char * folderName = NULL;
   static char *kwlist[] = {"session", "name", NULL};

   if ( ! PyArg_ParseTupleAndKeywords(args, kwds, "O|s", kwlist, 
      &session, &folderName) ) {
      return NULL; 
   }

   self = (Folder *)type->tp_alloc( type, 0 );
   if (self != NULL) {
      self->handle = 0;
      self->sessionHandle = 0;
      self->iteratorStarted = 0;
      self->sessionHandle = ((Session *)session)->handle;
      self->name = NULL;
   }

   return (PyObject *)self;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static int
Folder_init( PyTypeObject * self, PyObject * args, PyObject *kwds )
{
   int errcode;
   PSO_HANDLE h;
   Folder * folder = (Folder *)self;
   PyObject * session = NULL, * name = NULL, * tmp = NULL;
   const char * folderName = NULL;
   static char *kwlist[] = {"session", "name", NULL};

   if ( ! PyArg_ParseTupleAndKeywords(args, kwds, "|Os", kwlist, 
      &session, &folderName) ) {
      return -1; 
   }

   if (folderName) {

      name = PyString_FromString( folderName );
      if ( name == NULL ) return -1;

      errcode = psoFolderOpen( (PSO_HANDLE)folder->sessionHandle,
                               folderName,
                               (psoUint32)strlen(folderName),
                               &h );
      if ( errcode == 0 ) {
         /*
          * Copying the old value of 'name' before decreasing the ref.
          * counter. Likely overkill but safer according to Python
          * documentation.
          */
         tmp = folder->name;
         folder->name = name;
         Py_XDECREF(tmp);
         
         folder->handle = (size_t)h;

         return 0;
      }

      Py_DECREF(name);
      SetException( errcode );
      return -1;
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Folder_iter( PyObject * self )
{
   Folder * folder = (Folder *) self;

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
   Folder * folder = (Folder *) self;
   FolderEntry * e;
   PyObject * objType = NULL, * entryName = NULL;
   
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
   
   entryName = PyString_FromStringAndSize( entry.name,
                                           entry.nameLengthInBytes );
   if ( entryName == NULL ) return NULL;

   e = (FolderEntry *)FolderEntry_new( &FolderEntryType, NULL, NULL );
   if ( e == NULL ) return NULL;
   
   e->status = entry.status;
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
   Folder * folder = (Folder *) self;
   
   errcode = psoFolderClose( (PSO_HANDLE)folder->handle );
   if ( errcode != 0 ) {
      SetException( errcode );
      return NULL;
   }
   
   folder->handle = 0;
   Py_XDECREF(folder->name);
   
   Py_INCREF(Py_None);
   return Py_None;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Folder_CreateObject( PyObject * self, PyObject * args )
{
   return NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Folder_DestroyObject( PyObject * self, PyObject * args )
{
   int errcode;
   const char * objectName;
   Folder * folder = (Folder *) self;
   
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
Folder_GetFirst( PyObject * self, PyObject * args )
{
   int errcode;
   psoFolderEntry entry;
   Folder * folder = (Folder *) self;
   FolderEntry * e;
   PyObject * objType = NULL, * entryName = NULL;

   errcode = psoFolderGetFirst( (PSO_HANDLE)folder->handle, &entry );
   if ( errcode != 0 ) {
      SetException( errcode );
      return NULL;
   }
   
   objType = GetObjectType( entry.type );
   if ( objType == NULL ) return NULL;
   
   entryName = PyString_FromStringAndSize( entry.name,
                                           entry.nameLengthInBytes );
   if ( entryName == NULL ) return NULL;

   e = (FolderEntry *)FolderEntry_new( &FolderEntryType, NULL, NULL );
   if ( e == NULL ) return NULL;
   
   e->status = entry.status;
   e->nameLength = entry.nameLengthInBytes;
   e->name = entryName;
   e->objType = objType;

   return (PyObject *)e;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Folder_GetNext( PyObject * self, PyObject * args )
{
   int errcode;
   psoFolderEntry entry;
   Folder * folder = (Folder *) self;
   FolderEntry * e;
   PyObject * objType = NULL, * entryName = NULL;

   errcode = psoFolderGetNext( (PSO_HANDLE)folder->handle, &entry );
   if ( errcode != 0 ) {
      SetException( errcode );
      return NULL;
   }
   
   objType = GetObjectType( entry.type );
   if ( objType == NULL ) return NULL;
   
   entryName = PyString_FromStringAndSize( entry.name,
                                           entry.nameLengthInBytes );
   if ( entryName == NULL ) return NULL;

   e = (FolderEntry *)FolderEntry_new( &FolderEntryType, NULL, NULL );
   if ( e == NULL ) return NULL;
   
   e->status = entry.status;
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
   Folder * folder = (Folder *) self;
   const char * folderName;
   PSO_HANDLE h;
   PyObject * name = NULL, * tmp = NULL;
   
   if ( !PyArg_ParseTuple(args, "s", &folderName) ) {
      return NULL;
   }

   if ( folder->handle ) {
      SetException( PSO_ALREADY_OPEN );
      return NULL;
   }

   name = PyString_FromString( folderName );
   if ( name == NULL ) return NULL;

   errcode = psoFolderOpen( (PSO_HANDLE)folder->sessionHandle,
                            folderName,
                            (psoUint32)strlen(folderName),
                            &h );
   if ( errcode == 0 ) {
      /*
       * Copying the old value of 'name' before decreasing the ref.
       * counter. Likely overkill but safer according to Python
       * documentation.
       */
      tmp = folder->name;
      folder->name = name;
      Py_XDECREF(tmp);
         
      folder->handle = (size_t)h;

      Py_INCREF(Py_None);
      return Py_None;
   }

   Py_DECREF(name);
   SetException( errcode );
   return NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Folder_Status( PyObject * self )
{
   int errcode;
   ObjStatus * pStatusPy;
   psoObjStatus status;
   Folder * folder = (Folder *) self;
   PyObject * objType = NULL, * objStatus = NULL;
   
   errcode = psoFolderStatus( (PSO_HANDLE)folder->handle, &status );
   if ( errcode != 0 ) {
      SetException( errcode );
      return NULL;
   }
   
   objType = GetObjectType( status.type );
   if ( objType == NULL ) return NULL;

   objStatus = GetObjectStatus( status.status );
   if ( objStatus == NULL ) return NULL;

   pStatusPy = (ObjStatus *)PyObject_New(ObjStatus, &ObjStatusType);
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

static PyMemberDef Folder_members[] = {
   {"handle", T_INT, offsetof(Folder, handle), RO,
    "Folder handle"},
   { "name", T_OBJECT_EX, offsetof(Folder, name), RO,
     "The name of the folder"},
   {NULL}  /* Sentinel */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyMethodDef Folder_methods[] = {
   { "close", (PyCFunction)Folder_Close, METH_NOARGS,
     "Close the current folder"
   },
   { "create_object", (PyCFunction)Folder_CreateObject, METH_VARARGS,
     "Create a new photon object as a child of the current folder"
   },
   { "destroy_object", (PyCFunction)Folder_DestroyObject, METH_VARARGS,
     "Destroy an existing photon object (child of the current folder)"
   },
   { "get_first", (PyCFunction)Folder_GetFirst, METH_VARARGS,
     ""
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
   sizeof(Folder),             /*tp_basicsize*/
   0,                          /*tp_itemsize*/
   (destructor)Folder_dealloc, /*tp_dealloc*/
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
   (initproc)Folder_init,      /* tp_init */
   0,                          /* tp_alloc */
   Folder_new,                 /* tp_new */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
