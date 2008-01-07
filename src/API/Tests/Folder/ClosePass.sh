#!/bin/sh

# Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
# 
# This file is part of vdsf (Virtual Data Space Framework).
#
# This file may be distributed and/or modified under the terms of the
# GNU General Public License version 2 as published by the Free Software
# Foundation and appearing in the file COPYING included in the
# packaging of this library.
#
# This file is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
#
# To run these tests standalone (outside the scope of make check), you
# must provide some basic arguments:
#
#  - the path of the top directory of the source
#  - the path of the top directory of the build
#  - the tcp port for the watchdog
#
# For example: 
# 
# ./InitPass.sh /home/dprevost/vdsf/ /home/dprevost/vdsf 10701
#
# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

test_name=ClosePass
test_dir=src/API/Tests/Folder
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
   $top_srcdir/src/Tests/TestWithWatchdog.sh $top_srcdir $top_builddir $test_dir $test_name $tcp_port $errcode
else
   $top_srcdir/src/Tests/TestWithWatchdog.sh $top_srcdir $top_builddir $test_dir $test_name $tcp_port $errcode >/dev/null 2>&1
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

