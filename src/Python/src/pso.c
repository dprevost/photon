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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Python.h"
#include "structmember.h"

#include <photon/photon.h>

#include "Common.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
pso_exit( PyObject * self, PyObject * args  )
{
   psoExit();
   
   Py_INCREF( Py_None );
   return Py_None;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
pso_init( PyObject * self, PyObject * args )
{
   int errcode;
   const char * quasarAddress;
   const char * processName = NULL;
   
   if ( !PyArg_ParseTuple(args, "s|s", &quasarAddress, &processName ) ) {
      return NULL;
   }
   
   errcode = psoInit( quasarAddress, processName );
   if ( errcode != 0 ) {
      SetException( errcode );
      return NULL;
   }

   Py_INCREF(Py_None);
   return Py_None;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyMethodDef pso_methods[] = {
   { "exit", pso_exit, METH_NOARGS,
     "Terminate access to Photon shared memory."},
   { "init",  pso_init, METH_VARARGS,
     "Initialize the Photon module (connect to server or shared-memory file."},
   {NULL, NULL, 0, NULL}
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC
initpso(void) 
{
   PyObject * m = NULL, * tup = NULL, * errs = NULL, * errNames = NULL;
   int rc;
   
   if (PyType_Ready(&DataDefBuilderODBCType) < 0) return;
   if (PyType_Ready(&DataDefBuilderUserType) < 0) return;
   if (PyType_Ready(&DataDefinitionType) < 0) return;
   if (PyType_Ready(&FolderType) < 0) return;
   if (PyType_Ready(&FolderEntryType) < 0) return;
   if (PyType_Ready(&InfoType) < 0) return;
   if (PyType_Ready(&KeyDefBuilderODBCType) < 0) return;
   if (PyType_Ready(&KeyDefBuilderUserType) < 0) return;
   if (PyType_Ready(&KeyDefinitionType) < 0) return;
   if (PyType_Ready(&ObjDefinitionType) < 0) return;
   if (PyType_Ready(&ObjStatusType) < 0) return;
   if (PyType_Ready(&QueueType) < 0) return; 
   if (PyType_Ready(&SessionType) < 0) return; 

   m = Py_InitModule3( "pso", 
                       pso_methods,
                       "Example module that creates an extension type.");
   if (m == NULL) return;

   /* Our exception */
   PhotonError = PyErr_NewException("pso.error", NULL, NULL);
   Py_INCREF(PhotonError);
   PyModule_AddObject(m, "error", PhotonError);

   tup = AddErrors();
   if ( tup == NULL ) return;
   
   if ( ! PyArg_ParseTuple(tup, "OO", &errs, &errNames) ) return;
   rc = PyModule_AddObject( m, "errs", errs );
   if ( rc != 0 ) return;
   rc = PyModule_AddObject( m, "err_names", errNames );
   if ( rc != 0 ) return;
   
   /* C structs (and enums?) */
   Py_INCREF( &DataDefBuilderODBCType );
   PyModule_AddObject( m, "DataDefBuilderODBC", (PyObject *)&DataDefBuilderODBCType );
   Py_INCREF( &DataDefBuilderUserType );
   PyModule_AddObject( m, "DataDefBuilderUser", (PyObject *)&DataDefBuilderUserType );

   Py_INCREF( &DataDefinitionType );
   PyModule_AddObject( m, "DataDefinition", (PyObject *)&DataDefinitionType );
   Py_INCREF( &FolderEntryType );
   PyModule_AddObject( m, "FolderEntry", (PyObject *)&FolderEntryType );
   Py_INCREF( &InfoType );
   PyModule_AddObject( m, "Info", (PyObject *)&InfoType );

   Py_INCREF( &KeyDefBuilderODBCType );
   PyModule_AddObject( m, "KeyDefBuilderODBC", (PyObject *)&KeyDefBuilderODBCType );
   Py_INCREF( &KeyDefBuilderUserType );
   PyModule_AddObject( m, "KeyDefBuilderUser", (PyObject *)&KeyDefBuilderUserType );

   Py_INCREF( &KeyDefinitionType );
   PyModule_AddObject( m, "KeyDefinition", (PyObject *)&KeyDefinitionType );
   Py_INCREF( &ObjDefinitionType );
   PyModule_AddObject( m, "ObjDefinition", (PyObject *)&ObjDefinitionType );
   Py_INCREF( &ObjStatusType );
   PyModule_AddObject( m, "ObjStatus", (PyObject *)&ObjStatusType );

   /* Photon objects */
   Py_INCREF( &FolderType );
   PyModule_AddObject( m, "Folder", (PyObject *)&FolderType );
   Py_INCREF( &QueueType );
   PyModule_AddObject( m, "Queue", (PyObject *)&QueueType );
   Py_INCREF( &SessionType );
   PyModule_AddObject( m, "Session", (PyObject *)&SessionType );
   
   /* Object types */
   PyModule_AddIntConstant( m, "FOLDER",   PSO_FOLDER );
   PyModule_AddIntConstant( m, "HASH_MAP", PSO_HASH_MAP );
   PyModule_AddIntConstant( m, "LIFO",     PSO_LIFO );
   PyModule_AddIntConstant( m, "FAST_MAP", PSO_FAST_MAP );
   PyModule_AddIntConstant( m, "QUEUE",    PSO_QUEUE );

   /* Definition types */
   PyModule_AddIntConstant( m, "USER_DEFINED", PSO_DEF_USER_DEFINED );
   PyModule_AddIntConstant( m, "ODBC_SIMPLE",  PSO_DEF_PHOTON_ODBC_SIMPLE );
   
   /* Iterators - needed ? */
   PyModule_AddIntConstant( m, "FIRST", PSO_FIRST );
   PyModule_AddIntConstant( m, "NEXT",  PSO_NEXT );

   /* Field types */
//   PyModule_AddIntConstant( m, "INTEGER",    PSO_INTEGER );
//   PyModule_AddIntConstant( m, "BINARY",     PSO_BINARY );
//   PyModule_AddIntConstant( m, "STRING",     PSO_CHAR );
//   PyModule_AddIntConstant( m, "DECIMAL",    PSO_DECIMAL );
//   PyModule_AddIntConstant( m, "BOOLEAN",    PSO_TINYINT );
//   PyModule_AddIntConstant( m, "VAR_BINARY", PSO_VARBINARY );
//   PyModule_AddIntConstant( m, "VAR_STRING", PSO_VARCHAR );

   /* Key types */
//   PyModule_AddIntConstant( m, "KEY_INTEGER",    PSO_KEY_INTEGER );
//   PyModule_AddIntConstant( m, "KEY_BINARY",     PSO_KEY_BINARY );
//   PyModule_AddIntConstant( m, "KEY_STRING",     PSO_KEY_STRING );
//   PyModule_AddIntConstant( m, "KEY_VAR_BINARY", PSO_KEY_VAR_BINARY );
//   PyModule_AddIntConstant( m, "KEY_VAR_STRING", PSO_KEY_VAR_STRING );

}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

