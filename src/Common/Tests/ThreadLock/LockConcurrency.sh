#!/bin/bash

# Copyright (C) 2006-2009 Daniel Prevost <dprevost@photonsoftware.org>
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
# must provide the name of the directory where the sources are located
# as the second argument to the script. Example:
#
# ./LockConcurrency.sh 100 /home/dprevost/photon.git/src
#
# A second argument can be provided to indicate the build directory
# if using the VPATH functionality of make.
#
# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

set -m
if [ "$?" != 0 ] ; then
   exit 77
fi

if [ "$1" = "" ] ; then
   timer=25
fi

if [ "$TMPDIR" = "" ] ; then
   TMPDIR=/tmp
fi
BASE_DIR=$TMPDIR/photon/lock_concurr

trap `rm -rf $BASE_DIR; exit 1` 1 2 3 15

if test -z "$srcdir"; then
  if test -z "$2"; then
    echo "Usage: $0 time srcdir [top_builddir]"
    exit 1
  fi
  srcdir=$2
  if test -z "$3"; then
    top_builddir=$srcdir/..
  else
    top_builddir=$3
  fi
  verbose=1
fi

rm -rf $TMPDIR/pso
mkdir $TMPDIR/pso
if [ "$?" != 0 ] ; then
   exit 1
fi

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

$top_builddir/src/Common/Tests/ThreadLock/LockConcurrency $timer $BASE_DIR "lock" &
if [ "$?" != 0 ] ; then
   rm -rf $BASE_DIR
   exit 1
fi

wait %1
if [ "$?" != 0 ] ; then
   rm -rf $BASE_DIR
   exit 1
fi

echo "Tests terminated successfully"

rm -rf $BASE_DIR

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

