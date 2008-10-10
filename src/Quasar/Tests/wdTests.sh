#!/bin/sh

# Copyright (C) 2006-2008 Daniel Prevost <dprevost@photonsoftware.org>
# 
# This file is part of Photon (photonsoftware.org).
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

cp $srcdir/../../XML/quasar_config.xsd $BASE_DIR
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
$top_builddir/src/Quasar/quasar 
else
$top_builddir/src/Quasar/quasar >/dev/null 2>&1
fi
if [ "$?" = 0 ] ; then
   exit 1
fi

if [ $verbose = 1 ] ; then
$top_builddir/src/Quasar/quasar -u
else
$top_builddir/src/Quasar/quasar -u >/dev/null 2>&1
fi
if [ "$?" = 0 ] ; then
   exit 1
fi

if [ $verbose = 1 ] ; then
$top_builddir/src/Quasar/quasar --test -c $BASE_DIR/cfg.xml
else
$top_builddir/src/Quasar/quasar --test -c $BASE_DIR/cfg.xml >/dev/null 2>&1
fi
if [ "$?" = 0 ] ; then
   exit 1
fi

touch $BASE_DIR/cfg.xml
if [ $verbose = 1 ] ; then
$top_builddir/src/Quasar/quasar --test -c $BASE_DIR/cfg.xml
else
$top_builddir/src/Quasar/quasar --test -c $BASE_DIR/cfg.xml >/dev/null 2>&1
fi
if [ "$?" = 0 ] ; then
   exit 1
fi


echo "<?xml version=\"1.0\"?>                                   " >> $BASE_DIR/cfg.xml
echo "<quasar_config xmlns=\"http://photonsoftware.org/quasarConfig\" " >> $BASE_DIR/cfg.xml
echo "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"   " >> $BASE_DIR/cfg.xml
echo "xsi:schemaLocation=\"http://photonsoftware.org/quasarConfig $BASE_DIR/quasar_config.xsd\"> " >> $BASE_DIR/cfg.xml
echo "  <mem_location>$BASE_DIR/vds</mem_location>              " >> $BASE_DIR/cfg.xml
echo "  <mem_size size=\"10240\" units=\"kb\" />                " >> $BASE_DIR/cfg.xml
#echo "  <quasar_address>10701</quasar_address>              " >> $BASE_DIR/cfg.xml
echo "  <file_access access=\"group\" />                        " >> $BASE_DIR/cfg.xml
echo "</quasar_config>                                            " >> $BASE_DIR/cfg.xml

if [ $verbose = 1 ] ; then
$top_builddir/src/Quasar/quasar --test -c $BASE_DIR/cfg.xml
else
$top_builddir/src/Quasar/quasar --test -c $BASE_DIR/cfg.xml >/dev/null 2>&1
fi
if [ "$?" = 0 ] ; then
   exit 1
fi

# Success expected
rm -f $BASE_DIR/cfg.xml
echo "<?xml version=\"1.0\"?>                                   " >> $BASE_DIR/cfg.xml
echo "<quasar_config xmlns=\"http://photonsoftware.org/quasarConfig\" " >> $BASE_DIR/cfg.xml
echo "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"   " >> $BASE_DIR/cfg.xml
echo "xsi:schemaLocation=\"http://photonsoftware.org/quasarConfig $BASE_DIR/quasar_config.xsd\"> " >> $BASE_DIR/cfg.xml
echo "  <mem_location>$BASE_DIR/vds</mem_location>              " >> $BASE_DIR/cfg.xml
echo "  <mem_size size=\"10240\" units=\"kb\" />                " >> $BASE_DIR/cfg.xml
echo "  <quasar_address>10701</quasar_address>              " >> $BASE_DIR/cfg.xml
echo "  <file_access access=\"group\" />                        " >> $BASE_DIR/cfg.xml
echo "</quasar_config>                                            " >> $BASE_DIR/cfg.xml

#if [ $verbose = 1 ] ; then
$top_builddir/src/Quasar/quasar --test -c $BASE_DIR/cfg.xml
#else
#$top_builddir/src/Quasar/quasar --test $BASE_DIR/cfg.xml >/dev/null 2>&1
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
