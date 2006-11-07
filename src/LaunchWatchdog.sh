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
# This script is provided both as an example and as a way to easily
# launch the watchdog for debugging purposes.
#
# The script will accept a single argument, the path where the watchdog
# is located. If not provided, the script assumes that the watchdog is
# located in "../Watchdog".
#
# \todo Additional arguments (to control the location of the working
#       files, the size of the VDS, etc.) should be added eventually.
#
# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--


if [ "$TMPDIR" = "" ] ; then
   TMPDIR=/tmp
fi
BASE_DIR=$TMPDIR/vdsf_001

trap `rm -rf $BASE_DIR; exit 1` 1 2 3 15

if test -z "$1"; then
  wddir="../Watchdog"
else
  wddir=$1
fi

verbose=1

rm -rf $BASE_DIR
mkdir $BASE_DIR
if [ "$?" != 0 ] ; then
   exit 1
fi

#mkdir $BASE_DIR/vds
#if [ "$?" != 0 ] ; then
#   exit 1
#fi

echo "# VDSF Config file             "  >  $BASE_DIR/cfg.txt
echo "#                              "  >> $BASE_DIR/cfg.txt
echo "VDSLocation           $BASE_DIR"  >> $BASE_DIR/cfg.txt
echo "MemorySize            10000    "  >> $BASE_DIR/cfg.txt
echo "WatchdogAddress       12345    "  >> $BASE_DIR/cfg.txt
echo "LogTransaction        0        "  >> $BASE_DIR/cfg.txt
echo "FilePermissions       0660     "  >> $BASE_DIR/cfg.txt
echo "DirectoryPermissions  0770     "  >> $BASE_DIR/cfg.txt

if [ $verbose = 1 ] ; then
   verb=
else
   verb=>/dev/null 2>&1
fi

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

$wddir/vdswd  $BASE_DIR/cfg.txt
if [ "$?" = 0 ] ; then
   exit 1
fi


# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

rm -rf $BASE_DIR

exit 0

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
