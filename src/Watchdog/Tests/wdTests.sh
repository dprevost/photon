#!/bin/sh

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
# ./transactions.sh /home/dprevost/vdsf0.1/src
#
# A second argument can be provided to indicate the build directory
# if using the VPATH functionality of make.
#
# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
#
# The tests in this script are pretty basic (checking arguments, config
# parameters, etc.).
#
# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

if [ "$TMPDIR" = "" ] ; then
   TMPDIR=/tmp
fi
BASE_DIR=$TMPDIR/wd_tests

trap `rm -rf $BASE_DIR; exit 1` 1 2 3 15

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

rm -rf $BASE_DIR
mkdir $BASE_DIR
if [ "$?" != 0 ] ; then
   exit 1
fi
mkdir $BASE_DIR/vds
if [ "$?" != 0 ] ; then
   exit 1
fi

if [ $verbose = 1 ] ; then
   verb=
else
   verb=>/dev/null 2>&1
fi

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

if [ $verbose = 1 ] ; then
$top_builddir/src/Watchdog/vdswd 
else
$top_builddir/src/Watchdog/vdswd >/dev/null 2>&1
fi
if [ "$?" = 0 ] ; then
   exit 1
fi

if [ $verbose = 1 ] ; then
$top_builddir/src/Watchdog/vdswd -u
else
$top_builddir/src/Watchdog/vdswd -u >/dev/null 2>&1
fi
if [ "$?" = 0 ] ; then
   exit 1
fi

if [ $verbose = 1 ] ; then
$top_builddir/src/Watchdog/vdswd $BASE_DIR/cfg.txt
else
$top_builddir/src/Watchdog/vdswd $BASE_DIR/cfg.txt >/dev/null 2>&1
fi
if [ "$?" = 0 ] ; then
   exit 1
fi

touch $BASE_DIR/cfg.txt
if [ $verbose = 1 ] ; then
$top_builddir/src/Watchdog/vdswd $BASE_DIR/cfg.txt
else
$top_builddir/src/Watchdog/vdswd $BASE_DIR/cfg.txt >/dev/null 2>&1
fi
if [ "$?" = 0 ] ; then
   exit 1
fi

echo "# Test for config"                   >  $BASE_DIR/cfg.txt
echo "#"                                   >> $BASE_DIR/cfg.txt
echo "MemoryFileName   $BASE_DIR/mem-file" >> $BASE_DIR/cfg.txt
echo "MemorySize       10                " >> $BASE_DIR/cfg.txt
echo "LogDirectoryName $BASE_DIR/log_dir " >> $BASE_DIR/cfg.txt
if [ $verbose = 1 ] ; then
$top_builddir/src/Watchdog/vdswd $BASE_DIR/cfg.txt
else
$top_builddir/src/Watchdog/vdswd $BASE_DIR/cfg.txt >/dev/null 2>&1
fi
if [ "$?" = 0 ] ; then
   exit 1
fi

# Success expected
rm -f $BASE_DIR/cfg.txt
echo "# Test for config"                   >  $BASE_DIR/cfg.txt
echo "#"                                   >> $BASE_DIR/cfg.txt
echo "VDSLocation          $BASE_DIR/vds " >> $BASE_DIR/cfg.txt
echo "# in kbytes"                         >> $BASE_DIR/cfg.txt
echo "MemorySize           10000         " >> $BASE_DIR/cfg.txt
echo "WatchdogAddress      12345         " >> $BASE_DIR/cfg.txt
echo "LogTransaction       0             " >> $BASE_DIR/cfg.txt
echo "FilePermissions      0660          " >> $BASE_DIR/cfg.txt
echo "DirectoryPermissions 0770          " >> $BASE_DIR/cfg.txt
#if [ $verbose = 1 ] ; then
$top_builddir/src/Watchdog/vdswd --test -c $BASE_DIR/cfg.txt
#else
#$top_builddir/src/Watchdog/vdswd --test $BASE_DIR/cfg.txt >/dev/null 2>&1
#fi
if [ "$?" != 0 ] ; then
   exit 1
fi

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

if [ $verbose = 1 ] ; then
  echo "Success wdTests.sh"
fi

rm -rf $BASE_DIR

exit 0

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
