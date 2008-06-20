/*
 * This file was generated by the program errorParser
 * using the input file vdsf.xml.
 * Date: Thu Jun 19 23:35:10 2008.
 *
 * The version of this interface is 0.3.
 *
 * Copyright (C) 2006-2008 Daniel Prevost
 *
 * This file is part of the vdsf library (Virtual Data Space Framework).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free
 * Software Foundation and appearing in the file COPYING included in
 * the packaging of this library.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifndef VDSERRORHANDLER_H
#define VDSERRORHANDLER_H

#ifdef __cplusplus
extern "C" {
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Use this function to access the error messages (defined in the xml
 * input file).
 * 
 * Parameters:
 *   - errnum    The error number
 *
 * Return values:
 *   - the error message if errnum is valid (exists)
 *   - NULL otherwise
 */
const char * vdse_ErrorMessage( int errnum );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
}
#endif

#endif /* VDSERRORHANDLER_H */

