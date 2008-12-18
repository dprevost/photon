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

PyObject * AddErrors(void)
{
   PyObject * errors = NULL, * value = NULL;
   int errcode;
   
   errors = PyDict_New();
   if ( errors == NULL ) {
      fprintf( stderr, "Errors not added to module 1 !\n" );
      return NULL;
   }
   
   value = PyInt_FromLong( 8 );
   if ( value == NULL ) 
      PyDict_Clear( errors );
      Py_DECREF(errors);
      return NULL;
   }
   errcode = PyDict_SetItemString( errors, "INVALID_LENGTH", value);
   Py_DECREF(value);
   if (errcode != 0) {
      PyDict_Clear( errors );
      Py_DECREF(errors);
      return NULL;
   }

   return errors;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
