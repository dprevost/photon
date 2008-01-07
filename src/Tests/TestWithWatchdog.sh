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
# Arguments to this script:
#
#  - the path of the top directory of the source
#  - the path of the top directory of the build
#  - the relative path of the test to be run (versus the build dir)
#  - the name of the test to be run
#  - the tcp/ip port for the watchdog
#  - expected error code
#
# For example: 
# 
# /home/dprevost/vdsf/ /home/dprevost/vdsf src/API/Tests/Api InitPass 10701 0
#
# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

if [ "$#" != 6 ] ; then
   echo "usage: $0 src_dir build_dir test_dir test_name watchdog_port expected_error_code"
   exit 1
fi

src_dir=$1
top_build_dir=$2
test_dir=$3
test_name=$4
port=$5
errcode=$6

if [ "$TMPDIR" = "" ] ; then
   TMPDIR=/tmp
fi
BASE_DIR=$TMPDIR/vdsf_$test_name

trap `rm -rf $BASE_DIR; exit 1` 1 2 3 15

verbose=1
PYTHON=python

rm -rf $BASE_DIR
mkdir $BASE_DIR
if [ "$?" != 0 ] ; then
   exit 1
fi

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

if [ $verbose = 1 ] ; then
   $PYTHON $src_dir/src/Tests/TestWithWatchdog.py $src_dir $top_build_dir $test_dir $test_name $port $errcode $BASE_DIR
else
   $PYTHON $src_dir/src/Tests/TestWithWatchdog.py $src_dir $top_build_dir $test_dir $test_name $port $errcode $BASE_DIR >/dev/null 2>&1
fi
if [ "$?" != 0 ] ; then
#   if [ $verbose = 1 ] ; then
#      echo "FAIL: $test_name (Python test)"
#   fi
   rm -rf $BASE_DIR
   exit 1
fi

#if [ $verbose = 1 ] ; then
#   echo "PASS: $test_name (Python test)"
#fi

rm -rf $BASE_DIR
exit 0

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

