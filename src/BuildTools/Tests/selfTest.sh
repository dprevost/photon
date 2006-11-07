#!/bin/bash

# Copyright (C) 2006 Daniel Prevost <dprevost@users.sourceforge.net>
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
# must provide the name of the directory where the sources are located
# as the first argument to the script. Example:
#
# ./selfTest.sh /home/dprevost/vdsf0.1/src
#
# A second argument can be provided to indicate the build directory
# if using the VPATH functionality of make.
#
# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

set -m
if [ "$?" != 0 ] ; then
   exit 77
fi

if test -z "$srcdir"; then
  if test -z "$1"; then
    echo "Usage: $0 srcdir [top_builddir]"
    exit 1
  fi
  srcdir=$1
  if test -z "$2"; then
    top_builddir=$srcdir/..
  else
    top_builddir=$2
  fi
  verbose=1
fi

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

$top_builddir/src/BuildTools/errorParser --test
if [ "$?" != 0 ] ; then
   exit 1
fi

echo "Tests terminated successfully"

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
