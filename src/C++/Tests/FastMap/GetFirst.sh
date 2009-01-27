#!/bin/sh

# Copyright (C) 2008-2009 Daniel Prevost <dprevost@photonsoftware.org>
# 
# This file is part of Photon (photonsoftware.org).
#
# This file may be distributed and/or modified under the terms of the
# GNU General Public License version 2 or version 3 as published by the 
# Free Software Foundation and appearing in the file COPYING.GPL2 and 
# COPYING.GPL3 included in the packaging of this software.
#
# Licensees holding a valid Photon Commercial license can use this file 
# in accordance with the terms of their license.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
#
# To run these tests standalone (outside the scope of make check), you
# must provide some basic arguments:
#
#  - the path of the top directory of the source
#  - the path of the top directory of the build
#  - the tcp port for the server
#
# For example: 
# 
# ./InitPass.sh /home/dprevost/photon/ /home/dprevost/photon 10701
#
# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

test_name=GetFirst
test_dir=src/C++/Tests/FastMap
errcode=0

# If the env. variable top_srcdir is not defined... we must have three
# arguments if we want to be able to run the test.
if test -z "$top_srcdir"; then
   if [ "$#" != 3 ] ; then
      echo "usage: $0 src_dir build_dir tcp_port"
      exit 1
   fi

   top_srcdir=$1
   top_builddir=$2
   tcp_port=$3
   verbose=1
   PYTHON=python
fi

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

if [ $verbose = 1 ] ; then
   $top_srcdir/src/Tests/TestWithQuasar.sh $top_srcdir $top_builddir $test_dir $test_name $tcp_port $errcode
else
   $top_srcdir/src/Tests/TestWithQuasar.sh $top_srcdir $top_builddir $test_dir $test_name $tcp_port $errcode >/dev/null 2>&1
fi
if [ "$?" != 0 ] ; then
   if [ $verbose = 1 ] ; then
      echo "FAIL: $test_name "
   fi
   exit 1
fi

if [ $verbose = 1 ] ; then
   echo "PASS: $test_name "
fi

exit 0

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

