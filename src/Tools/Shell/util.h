/* -*- c++ -*- */
/* :mode=c++:  - For jedit, previous line for emacs */
/*
 * Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

/*
 * Utility functions to be used by the shell program. 
 */
 
// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#ifndef PSO_SHELL_UTIL_H
#define PSO_SHELL_UTIL_H

#include "Common/Common.h"
#include <photon/photon>
#if defined(WIN32)
#  pragma warning(disable: 4786)
#endif
#include <string>
#include <iostream>
#include <vector>
//#include "Tools/Shell/constants.h"

using namespace std;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

unsigned char * shellInToBuff( string               & inData, 
                               pso::FieldDefinition & definition,
                               uint32_t             & length );

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

unsigned char * shellInToKey( string             & inData, 
                              pso::KeyDefinition & definition,
                              uint32_t           & length );

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#endif // PSO_SHELL_UTIL_H

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

