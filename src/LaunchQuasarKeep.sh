#!/bin/sh

# Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
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
# This script is provided both as an example and as a way to easily
# launch the server for debugging purposes.
#
# The script will accept a single argument, the path where the server
# is located. If not provided, the script assumes that the server is
# located in "../Quasar".
#
# \todo Additional arguments (to control the location of the working
#       files, the size of the shared memory, etc.) should be added eventually.
#
# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

echo "This launch a new server (quasar). Warning: The shared memory is NOT removed upon termination."
echo "You can specify the path to quasar as the first argument"

if [ "$TMPDIR" = "" ] ; then
   TMPDIR=/tmp
fi
BASE_DIR=$TMPDIR/photon_002

trap 'rm -rf $BASE_DIR; exit 1' 1 3 6 15
trap 'exit 0' 2

if test -z "$1"; then
  QUASAR_DIR="./Quasar"
else
  QUASAR_DIR=$1
fi

verbose=1

if [ ! -d $BASE_DIR ]; then
  mkdir $BASE_DIR
  cp $QUASAR_DIR/../XML/quasar_config.xsd $BASE_DIR
  if [ "$?" != 0 ] ; then
    exit 1
  fi

  echo "<?xml version=\"1.0\"?>                                   " >> $BASE_DIR/cfg.xml
  echo "<quasar_config xmlns=\"http://photonsoftware.org/quasarConfig\" " >> $BASE_DIR/cfg.xml
  echo "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"   " >> $BASE_DIR/cfg.xml
  echo "xsi:schemaLocation=\"http://photonsoftware.org/quasarConfig $BASE_DIR/quasar_config.xsd\"> " >> $BASE_DIR/cfg.xml
  echo "  <mem_location>$BASE_DIR</mem_location>                  " >> $BASE_DIR/cfg.xml
  echo "  <mem_size size=\"10240\" units=\"kb\" />                " >> $BASE_DIR/cfg.xml
  echo "  <quasar_address>10701</quasar_address>              " >> $BASE_DIR/cfg.xml
  echo "  <file_access access=\"group\" />                        " >> $BASE_DIR/cfg.xml
  echo "</quasar_config>                                            " >> $BASE_DIR/cfg.xml
fi

if [ $verbose = 1 ] ; then
   verb=
else
   verb=>/dev/null 2>&1
fi

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

echo " "
echo "Launching quasar..."

$QUASAR_DIR/quasar --config $BASE_DIR/cfg.xml
# gdb $QUASAR_DIR/quasar  

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#rm -rf $BASE_DIR

exit 0

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

