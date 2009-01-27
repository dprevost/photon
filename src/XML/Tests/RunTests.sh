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
# For example: 
# 
# ./RunTests.sh /home/dprevost/photon/ 
#
# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

# If the env. variable top_srcdir is not defined... we must have one
# argument if we want to be able to run the test.
if test -z "$top_srcdir"; then
   if [ "$#" != 1 ] ; then
      echo "usage: $0 top_src_dir"
      exit 1
   fi

   top_srcdir=$1
   verbose=1
   PYTHON=python
fi

test_dir=$top_srcdir/src/XML/Tests
xml_dir=$top_srcdir/src/XML

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
#
# This function take a single parameter, the name of the test (which is
# the name of the xml file minus the .xml extension).
#
# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

run_md_test()
{
   test_name=$1

   if [ $verbose = 1 ] ; then
      xmllint --noout --schema $xml_dir/photon_meta10.xsd $test_name
   else
      xmllint --noout --schema $xml_dir/photon_meta10.xsd $test_name >/dev/null 2>&1
   fi
   if [ "$?" = 0 ] ; then
      echo "FAIL: $test_name "
      return 1
   fi

   if [ $verbose = 1 ] ; then
      echo "PASS: $test_name "
   fi
}


# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

list_tests=`ls $test_dir/md/*.xml`

for tests in $list_tests; do
   run_md_test $tests
   if [ "$?" != 0 ] ; then
      exit 1
   fi
done

exit 0

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

