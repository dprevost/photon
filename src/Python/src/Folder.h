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
   
   /* This is completely private. Should not be put in the members struct */

   /* 
    * Set to one when iter() is called so that we know we need call 
    * GetFirst and not GetNext.
    */
   int iteratorStarted;
   
} Folder;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static void
Folder_dealloc( Folder * self )
{
   if ( self->handle != 0 ) {
      psoFolderClose( (PSO_HANDLE)self->handle );
   }
   
   self->ob_type->tp_free( (PyObject *)self );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Folder_new( PyTypeObject * type, PyObject * args, PyObject * kwds )
{
   Folder * self;

   self = (Folder *)type->tp_alloc( type, 0 );
   if (self != NULL) {
      self->handle = 0;
      self->sessionHandle = 0;
      self->iteratorStarted = 0;
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
   PyObject * session = NULL;
   const char * folderName = NULL;
   static char *kwlist[] = {"session", "name", NULL};

   if ( ! PyArg_ParseTupleAndKeywords(args, kwds, "|Os", kwlist, 
      &session, &folderName) ) {
      return -1; 
   }
      
   if (session == NULL) {
      PyErr_SetString( PyExc_SyntaxError, 
         "Syntax: Folder( Session [, folder_name ] )" );
      return -1;
   }

   folder->sessionHandle = ((Session *)session)->handle;

   if (folderName) {
      errcode = psoFolderOpen( (PSO_HANDLE)folder->sessionHandle,
                               folderName,
                               (psoUint32)strlen(folderName),
                               &h );
      if ( errcode == 0 ) {
         folder->handle = (size_t)h;
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
   Folder * fold = (Folder *) self;
   
   fold->iteratorStarted = 1;

   return self;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Folder_next( PyObject * self )
{
   int errcode;
   psoFolderEntry entry;
   Folder * fold = (Folder *) self;
   FolderEntry * e;
   PyObject * objType = NULL, * entryName = NULL;
   
   if ( fold->iteratorStarted == 1 ) {
      fold->iteratorStarted = 0;
      errcode = psoFolderGetFirst( (PSO_HANDLE)fold->handle, &entry );
      if ( errcode == PSO_IS_EMPTY ) {
         PyErr_SetString( PyExc_StopIteration, "" );
         return NULL;
      }
   }
   else {
      errcode = psoFolderGetNext( (PSO_HANDLE)fold->handle, &entry );
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

   e = PyObject_New(FolderEntry, &FolderEntryType);
   if ( e == NULL ) return NULL;
   
   e->status = entry.status;
   e->nameLength = entry.nameLengthInBytes;
   e->name = entryName;
   e->objType = objType;
   
   return (PyObject *)e;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Folder_Close( Session * self )
{
   int errcode;
   
   errcode = psoFolderClose( (PSO_HANDLE)self->handle );
   if ( errcode != 0 ) {
      SetException( errcode );
      return NULL;
   }
   
   Py_INCREF(Py_None);
   return Py_None;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Folder_CreateObject( Session * self, PyObject * args )
{
   return NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Folder_DestroyObject( Session * self, PyObject * args )
{
   int errcode;
   const char * objectName;
   
   if ( !PyArg_ParseTuple(args, "s", &objectName) ) {
      return NULL;
   }
   
   errcode = psoDestroyObject( (PSO_HANDLE)self->handle,
                               objectName,
                               (psoUint32)strlen(objectName) );
   
   return Py_BuildValue("i", errcode);
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Folder_GetFirst( Session * self, PyObject * args )
{
   return NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Folder_GetNext( Session * self, PyObject * args )
{
   return NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Folder_Open( Session * self, PyObject * args )
{
   return NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Folder_Status( Session * self, PyObject * args )
{
   int errcode;
   ObjStatus * pStatusPy;
   psoObjStatus status;
   
   if ( !PyArg_ParseTuple(args, "O", &pStatusPy) ) {
      return NULL;
   }

   errcode = psoFolderStatus( (PSO_HANDLE)self->handle,
                              &status );
   if ( errcode != 0 ) {
      SetException( errcode );
      return NULL;
   }
   
   pStatusPy->objType       = status.type;
   pStatusPy->status        = status.status;
   pStatusPy->numBlocks     = status.numBlocks;
   pStatusPy->numBlockGroup = status.numBlockGroup;
   pStatusPy->numDataItem   = status.numDataItem;
   pStatusPy->freeBytes     = status.freeBytes;
   pStatusPy->maxDataLength = status.maxDataLength;
   pStatusPy->maxKeyLength  = status.maxKeyLength;

   Py_INCREF(Py_None);
   return Py_None;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyMemberDef Folder_members[] = {
   {"handle", T_INT, offsetof(Folder, handle), RO,
    "Folder handle"},
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
   { "status", (PyCFunction)Folder_Status, METH_VARARGS,
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
