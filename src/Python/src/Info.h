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

#ifndef PSO_PY_INFO_H
#define PSO_PY_INFO_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This data structure is used to retrieve the status of the Photon shared memory.
 */

typedef struct {
   PyObject_HEAD

   /** Total size of the virtual data space. */
   size_t totalSizeInBytes;
   
   /** Total size of the allocated blocks. */
   size_t allocatedSizeInBytes;
   
   /** Number of API objects in the pso (internal objects are not counted). */
   size_t numObjects;
   
   /** Total number of groups of blocks. */
   size_t numGroups;
   
   /** Number of calls to allocate groups of blocks. */
   size_t numMallocs;
   
   /** Number of calls to free groups of blocks. */
   size_t numFrees;
   
   /** Largest contiguous group of free blocks. */
   size_t largestFreeInBytes;
   
   /** Shared-memory version. */
   int memoryVersion;

   /** Endianess (0 for little endian, 1 for big endian). */
   int bigEndian;
   
   /** Compiler name. */
   PyObject * compiler;

   /** Compiler version (if available). */
   PyObject * compilerVersion;
   
   /** Platform. */
   PyObject * platform;
   
   /** Shared lib version. */
   PyObject * dllVersion;
   
   /** The server version (of the pso creator). */
   PyObject * quasarVersion;
   
   /** Timestamp of creation of the shared memory. */
   PyObject * creationTime;

} pyInfo;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static void
Info_dealloc( PyObject * self )
{
   pyInfo * info = (pyInfo *) self;

   Py_XDECREF(info->compiler);
   Py_XDECREF(info->compilerVersion);
   Py_XDECREF(info->platform);
   Py_XDECREF(info->dllVersion);
   Py_XDECREF(info->quasarVersion);
   Py_XDECREF(info->creationTime);

   self->ob_type->tp_free( (PyObject *)self );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyObject *
Info_str( PyObject * self )
{
   pyInfo * obj = (pyInfo *)self;

   if ( obj->compiler ) {
      return PyString_FromFormat( 
         "Info{ total_size: %zu, "
         "allocated_size: %zu, "
         "num_objects: %zu, "
         "num_groups: %zu, "
         "num_mallocs: %zu, "
         "num_frees: %zu, "
         "largest_free: %zu, "
         "memory_version: %d, "
         "big_endian: %d, "
         "compiler_name: %s, "
         "compiler_version: %s, "
         "platform: %s, "
         "dll_version: %s, "
         "quasar_version: %s, "
         "creation_time: %s }",
         obj->totalSizeInBytes,
         obj->allocatedSizeInBytes,
         obj->numObjects,
         obj->numGroups,
         obj->numMallocs,
         obj->numFrees,
         obj->largestFreeInBytes,
         obj->memoryVersion,
         obj->bigEndian,
         PyString_AsString(obj->compiler),
         PyString_AsString(obj->compilerVersion),
         PyString_AsString(obj->platform),
         PyString_AsString(obj->dllVersion),
         PyString_AsString(obj->quasarVersion),
         PyString_AsString(obj->creationTime) );
   }
   
   return PyString_FromString("Info is not set");
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyMemberDef Info_members[] = {
#if SIZEOF_VOID_P == 4     
   { "total_size", T_UINT, offsetof(pyInfo, totalSizeInBytes), RO,
     "Total size of the shared memory"},
   { "allocated_size", T_UINT, offsetof(pyInfo, allocatedSizeInBytes), RO,
     "Total size of the allocated blocks"},
   { "num_objects", T_UINT, offsetof(pyInfo, numObjects), RO,
     "Number of API objects in shared memory"},
   { "num_groups", T_UINT, offsetof(pyInfo, numGroups), RO,
     "Total number of groups of blocks"},
   { "num_mallocs", T_UINT, offsetof(pyInfo, numMallocs), RO,
     "Number of calls to allocate groups of blocks"},
   { "num_frees", T_UINT, offsetof(pyInfo, numFrees), RO,
     "Number of calls to free groups of blocks"},
   { "largest_free", T_UINT, offsetof(pyInfo, largestFreeInBytes), RO,
     "Largest contiguous group of free blocks"},
#else
   /* 
    * On 64 bits machine, long long is a solution. V 2.6 of Python 
    * supports the T_PYSSIZET type - which would be a better solution.
    */
   { "total_size", T_ULONGLONG, offsetof(pyInfo, totalSizeInBytes), RO,
     "Total size of the shared memory"},
   { "allocated_size", T_ULONGLONG, offsetof(pyInfo, allocatedSizeInBytes), RO,
     "Total size of the allocated blocks"},
   { "num_objects", T_ULONGLONG, offsetof(pyInfo, numObjects), RO,
     "Number of API objects in shared memory"},
   { "num_groups", T_ULONGLONG, offsetof(pyInfo, numGroups), RO,
     "Total number of groups of blocks"},
   { "num_mallocs", T_ULONGLONG, offsetof(pyInfo, numMallocs), RO,
     "Number of calls to allocate groups of blocks"},
   { "num_frees", T_ULONGLONG, offsetof(pyInfo, numFrees), RO,
     "Number of calls to free groups of blocks"},
   { "largest_free", T_ULONGLONG, offsetof(pyInfo, largestFreeInBytes), RO,
     "Largest contiguous group of free blocks"},
#endif
   { "memory_version", T_INT, offsetof(pyInfo, memoryVersion), RO,
     "Shared-memory version"},
   { "big_endian", T_INT, offsetof(pyInfo, bigEndian), RO,
     "Endianess (0 for little endian, 1 for big endian)."},
   { "compiler_name", T_OBJECT_EX, offsetof(pyInfo, compiler), RO,
     "Compiler name"},
   { "compiler_version", T_OBJECT_EX, offsetof(pyInfo, compilerVersion), RO,
     "Compiler version (if available)"},
   { "platform", T_OBJECT_EX, offsetof(pyInfo, platform), RO,
     "Platform"},
   { "dll_version", T_OBJECT_EX, offsetof(pyInfo, dllVersion), RO,
     "Shared lib version"},
   { "quasar_version", T_OBJECT_EX, offsetof(pyInfo, quasarVersion), RO,
     "The server version (of the pso creator)"},
   { "creation_time", T_OBJECT_EX, offsetof(pyInfo, creationTime), RO,
     "Timestamp of creation of the shared memory"},
   {NULL}  /* Sentinel */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static PyTypeObject InfoType = {
   PyObject_HEAD_INIT(NULL)
   0,                          /*ob_size*/
   "pso.Info",                 /*tp_name*/
   sizeof(pyInfo),             /*tp_basicsize*/
   0,                          /*tp_itemsize*/
   Info_dealloc,               /*tp_dealloc*/
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
   Info_str,                   /*tp_str*/
   0,                          /*tp_getattro*/
   0,                          /*tp_setattro*/
   0,                          /*tp_as_buffer*/
   Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
   "An entry in a folder of photon", /* tp_doc */
   0,		                      /* tp_traverse */
   0,		                      /* tp_clear */
   0,		                      /* tp_richcompare */
   0,		                      /* tp_weaklistoffset */
   0,		                      /* tp_iter */
   0,		                      /* tp_iternext */
   0,                          /* tp_methods */
   Info_members,               /* tp_members */
   0,                          /* tp_getset */
   0,                          /* tp_base */
   0,                          /* tp_dict */
   0,                          /* tp_descr_get */
   0,                          /* tp_descr_set */
   0,                          /* tp_dictoffset */
   0,                          /* tp_init */
   0,                          /* tp_alloc */
   0,                          /* tp_new */
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSO_PY_INFO_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

