#!/bin/sh

# Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
# 
# This file is part of photon (photonsoftware.org).
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

echo "This launch a new watchdog. Warning: The vds is removed upon termination."
echo "You can specify the path to the watchdog executable as the first argument"

if [ "$TMPDIR" = "" ] ; then
   TMPDIR=/tmp
fi
BASE_DIR=$TMPDIR/vdsf_001

trap 'rm -rf $BASE_DIR; exit 1' 1 2 3 15

if test -z "$1"; then
  wddir="./Watchdog"
else
  wddir=$1
fi

verbose=1

rm -rf $BASE_DIR
mkdir $BASE_DIR
if [ "$?" != 0 ] ; then
   exit 1
fi

cp $wddir/../XML/wd_config.xsd $BASE_DIR
if [ "$?" != 0 ] ; then
   exit 1
fi

echo "<?xml version=\"1.0\"?>                                   " >> $BASE_DIR/cfg.xml
echo "<vdsf_config xmlns=\"http://vdsf.sourceforge.net/Config\" " >> $BASE_DIR/cfg.xml
echo "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"   " >> $BASE_DIR/cfg.xml
echo "xsi:schemaLocation=\"http://vdsf.sourceforge.net/Config $BASE_DIR/wd_config.xsd\"> " >> $BASE_DIR/cfg.xml
echo "  <vds_location>$BASE_DIR</vds_location>                  " >> $BASE_DIR/cfg.xml
echo "  <mem_size size=\"10240\" units=\"kb\" />                " >> $BASE_DIR/cfg.xml
echo "  <watchdog_address>10701</watchdog_address>              " >> $BASE_DIR/cfg.xml
echo "  <file_access access=\"group\" />                        " >> $BASE_DIR/cfg.xml
echo "</vdsf_config>                                            " >> $BASE_DIR/cfg.xml

if [ $verbose = 1 ] ; then
   verb=
else
   verb=>/dev/null 2>&1
fi

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

echo " "
echo "Launching the watchdog..."

$wddir/quasar  --config $BASE_DIR/cfg.xml

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

rm -rf $BASE_DIR

exit 0

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

