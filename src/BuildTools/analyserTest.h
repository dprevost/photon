/* -*- c++ -*- */
/*
 * Copyright (C) 2006 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of the vdsf (Virtual Data Space Framework) Library.
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation and appearing in the file COPYING included in the
 * packaging of this library.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifndef ANALYSER_TEST_H
#define ANALYSER_TEST_H

#include "Common.h"
#include <exception>
#include <string>
#include <vector>
#include "analyser.h"

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

class analyserTest
{
 public:

   analyserTest( analyser& a );
   
   ~analyserTest();

   int Run();

 private:

   analyser m_a;
   
   int TestCreatePattern();
   int TestStripComments();   
   int TestGetToken();
   int TestGetEnum();
   int TestProcessInput();
   int TestIdentifyElements();
   
};

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#endif // ANALYSER_TEST_H
