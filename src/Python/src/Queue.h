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

#ifndef PSO_PY_QUEUE_H
#define PSO_PY_QUEUE_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaQueueFirst( void           * pQueue,
                    unsigned char ** data,
                    uint32_t       * length );

int psoaQueueNext( void           * pQueue,
                   unsigned char ** data,
                   uint32_t       * length );

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
   
} pyQueue;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static void
Queue_dealloc( PyObject * self )
{
   pyQueue * queue = (pyQueue *) self;

   if ( queue->handle != 0 ) {
      psoQueueClose( (PSO_HANDLE)queue->handle );
   }

   Py_XDECREF(queue->name);
   Py_XDECREF(queue->session);

   self->ob_type->tp_free( (PyObject *)self );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Queue_new( PyTypeObject * type, PyObject * args, PyObject * kwds )
{
   pyQueue * self;

   self = (pyQueue *)type->tp_alloc( type, 0 );
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
Queue_init( PyObject * self, PyObject * args, PyObject *kwds )
{
   int errcode;
   PSO_HANDLE handle;
   pyQueue * queue = (pyQueue *)self;
   PyObject * session = NULL, * name = NULL, * tmp = NULL;
   const char * queueName = NULL;
   static char *kwlist[] = {"session", "name", NULL};

   if ( ! PyArg_ParseTupleAndKeywords(args, kwds, "|Os", kwlist, 
      &session, &queueName) ) {
      return -1; 
   }

   if (session && queueName) {

      errcode = psoQueueOpen( (PSO_HANDLE)((pySession *)session)->handle,
                              queueName,
                              (psoUint32)strlen(queueName),
                              &handle );
      if ( errcode == 0 ) {
      
         name = PyString_FromString( queueName );
         if ( name == NULL ) {
            psoQueueClose( handle );
            return -1;
         }
         /*
          * Copying the old value of 'name' before decreasing the ref.
          * counter. Likely overkill but safer according to Python
          * documentation.
          */
         tmp = queue->name;
         queue->name = name;
         Py_XDECREF(tmp);
         
         tmp = queue->session;
         queue->session = session;
         Py_XDECREF(tmp);

         queue->handle = (size_t)handle;
         
         return 0;
      }

      SetException( errcode );
      return -1;
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Queue_iter( PyObject * self )
{
   pyQueue * queue = (pyQueue *) self;

   Py_INCREF(self); 
   queue->iteratorStarted = 1;

   return self;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Queue_next( PyObject * self )
{
   int errcode;
   pyQueue * queue = (pyQueue *) self;
   PyObject * pyData = NULL;
   unsigned char * data;
   unsigned int dataLength;

   if ( queue->iteratorStarted == 1 ) {
      queue->iteratorStarted = 0;
      errcode = psoaQueueFirst( (PSO_HANDLE)queue->handle, &data, &dataLength );
      if ( errcode == PSO_IS_EMPTY ) {
         PyErr_SetString( PyExc_StopIteration, "" );
         return NULL;
      }
   }
   else {
      errcode = psoaQueueNext( (PSO_HANDLE)queue->handle, &data, &dataLength );
      if ( errcode == PSO_REACHED_THE_END ) {
         PyErr_SetString( PyExc_StopIteration, "" );
         return NULL;
      }
   }
   if ( errcode != 0 ) {
      SetException( errcode );
      return NULL;
   }
  
   pyData = PyByteArray_FromStringAndSize( (char *)data, dataLength );
   if ( pyData == NULL ) return NULL;

   return (PyObject *)pyData;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Queue_Close( PyObject * self )
{
   int errcode;
   pyQueue * queue = (pyQueue *) self;
   PyObject * tmp;
   
   errcode = psoQueueClose( (PSO_HANDLE)queue->handle );
   if ( errcode != 0 ) {
      SetException( errcode );
      return NULL;
   }
   
   queue->handle = 0;
   tmp = queue->name;
   Py_XDECREF(tmp);
   queue->name = NULL;
   
   Py_INCREF(Py_None);
   return Py_None;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Queue_GetDataDefinition( PyObject * self )
{
   int errcode;
   pyQueue * queue = (pyQueue *) self;
   pyDataDefinition * pyDef = NULL;
   PSO_HANDLE dataDefHandle;
   int type, length;
   unsigned int nameLength;
   PyObject * defType = NULL;
   unsigned char  * dataDef;
   char * definitionName;
   PyObject * name = NULL, *dataDefObj = NULL, * tmp = NULL;

   errcode = psoQueueDefinition( (PSO_HANDLE)queue->handle,
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
Queue_GetFirst( PyObject * self )
{
   int errcode;
   pyQueue * queue = (pyQueue *) self;
   PyObject * pyData = NULL;
   unsigned char * data;
   unsigned int dataLength;
   
   errcode = psoaQueueFirst( (PSO_HANDLE)queue->handle, &data, &dataLength );
   if ( errcode != 0 ) {
      SetException( errcode );
      return NULL;
   }
   
   pyData = PyByteArray_FromStringAndSize( (char *)data, dataLength );
   if ( pyData == NULL ) return NULL;

   return (PyObject *)pyData;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Queue_GetNext( PyObject * self )
{
   int errcode;
   pyQueue * queue = (pyQueue *) self;
   PyObject * pyData = NULL;
   unsigned char * data;
   unsigned int dataLength;

   errcode = psoaQueueNext( (PSO_HANDLE)queue->handle, &data, &dataLength );
   if ( errcode != 0 ) {
      SetException( errcode );
      return NULL;
   }
   
   pyData = PyByteArray_FromStringAndSize( (char *)data, dataLength );
   if ( pyData == NULL ) return NULL;

   return (PyObject *)pyData;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Queue_Open( PyObject * self, PyObject * args )
{
   int errcode;
   pyQueue * queue = (pyQueue *) self;
   const char * queueName;
   PSO_HANDLE handle;
   PyObject * name = NULL, *session = NULL, * tmp = NULL;
   
   if ( !PyArg_ParseTuple(args, "Os", &session, &queueName) ) {
      return NULL;
   }

   if ( queue->handle ) {
      SetException( PSO_ALREADY_OPEN );
      return NULL;
   }

   errcode = psoQueueOpen( (PSO_HANDLE)((pySession *)session)->handle,
                            queueName,
                            (psoUint32)strlen(queueName),
                            &handle );
   if ( errcode == 0 ) {

      name = PyString_FromString( queueName );
      if ( name == NULL ) {
         psoQueueClose( handle );
         return NULL;
      }
      tmp = queue->name;
      queue->name = name;
      Py_XDECREF(tmp);

      tmp = queue->session;
      queue->session = session;
      Py_XDECREF(tmp);

      queue->handle = (size_t)handle;

      Py_INCREF(Py_None);
      return Py_None;
   }
   SetException( errcode );

   return NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Queue_Status( PyObject * self )
{
   int errcode;
   pyObjStatus * pStatusPy;
   psoObjStatus status;
   pyQueue * queue = (pyQueue *) self;
   PyObject * objType = NULL, * objStatus = NULL;
   
   errcode = psoQueueStatus( (PSO_HANDLE)queue->handle, &status );
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
Queue_str( PyObject * self )
{
   pyQueue * queue = (pyQueue *) self;
   
   if ( queue->name != NULL ) {
      return PyString_FromFormat( 
         "Queue{ name: %s }",
         PyString_AsString(queue->name) );
   }
   
   return PyString_FromString("Queue is not open");
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyMemberDef Queue_members[] = {
   { "name", T_OBJECT_EX, offsetof(pyQueue, name), RO,
     "The name of the queue"},
   {NULL}  /* Sentinel */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyMethodDef Queue_methods[] = {
   { "close", (PyCFunction)Queue_Close, METH_NOARGS,
     "Close the current queue"
   },
   { "get_data_definition", (PyCFunction)Queue_GetDataDefinition, METH_NOARGS,
     "Get the data definition of a Photon object"
   },

   { "get_first", (PyCFunction)Queue_GetFirst, METH_NOARGS,
     ""
   },
   { "get_next", (PyCFunction)Queue_GetNext, METH_NOARGS,
     ""
   },
   { "open", (PyCFunction)Queue_Open, METH_VARARGS,
     "Open (get access to) a Photon queue"
   },
   { "status", (PyCFunction)Queue_Status, METH_NOARGS,
     "Return the error code of the last error"
   },
   {NULL}  /* Sentinel */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyTypeObject QueueType = {
   PyObject_HEAD_INIT(NULL)
   0,                          /*ob_size*/
   "pso.Folder",               /*tp_name*/
   sizeof(pyQueue),            /*tp_basicsize*/
   0,                          /*tp_itemsize*/
   Queue_dealloc,              /*tp_dealloc*/
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
   Queue_str,                  /*tp_str*/
   0,                          /*tp_getattro*/
   0,                          /*tp_setattro*/
   0,                          /*tp_as_buffer*/
   Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_ITER, /*tp_flags*/
   "Folder objects",           /* tp_doc */
   0,		                      /* tp_traverse */
   0,		                      /* tp_clear */
   0,		                      /* tp_richcompare */
   0,		                      /* tp_weaklistoffset */
   Queue_iter,                 /* tp_iter */
   Queue_next,                 /* tp_iternext */
   Queue_methods,              /* tp_methods */
   Queue_members,              /* tp_members */
   0,                          /* tp_getset */
   0,                          /* tp_base */
   0,                          /* tp_dict */
   0,                          /* tp_descr_get */
   0,                          /* tp_descr_set */
   0,                          /* tp_dictoffset */
   Queue_init,                 /* tp_init */
   0,                          /* tp_alloc */
   Queue_new,                  /* tp_new */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSO_PY_QUEUE_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

