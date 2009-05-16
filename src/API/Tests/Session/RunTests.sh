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
# To run these tests standalone (outside the scope of make check), you
# must provide some basic arguments:
#
#  - the path of the top directory of the source
#  - the path of the top directory of the build
#  - the tcp port for the server
#
# For example: 
# 
# ./InitPass.sh /home/dprevost/photon/ /home/dprevost/photon 10701
#
# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

ok_programs=$ok_programs"Close "
ok_programs=$ok_programs"CloseObjPass "
ok_programs=$ok_programs"Commit "
ok_programs=$ok_programs"CreateFolder "
ok_programs=$ok_programs"CreateMap "
ok_programs=$ok_programs"CreateQueue "
ok_programs=$ok_programs"DataDefinition "
ok_programs=$ok_programs"Definition "
ok_programs=$ok_programs"Destroy "
ok_programs=$ok_programs"ErrorMsg "
ok_programs=$ok_programs"ExitPass "
ok_programs=$ok_programs"ExitWithOpenObjects "
ok_programs=$ok_programs"GetInfo "
ok_programs=$ok_programs"Init "
ok_programs=$ok_programs"KeyDefinition "
ok_programs=$ok_programs"LastError "
ok_programs=$ok_programs"OpenObjNotCreated "
ok_programs=$ok_programs"OpenObjPass "
ok_programs=$ok_programs"Rollback "
ok_programs=$ok_programs"Status "

fail_programs=$fail_programs"CloseNullSession "
fail_programs=$fail_programs"CloseObjNullObject "
fail_programs=$fail_programs"CloseObjNullSession "
fail_programs=$fail_programs"OpenObjNullName "
fail_programs=$fail_programs"OpenObjNullObject "
fail_programs=$fail_programs"OpenObjNullSession "
fail_programs=$fail_programs"OpenObjWrongType "
fail_programs=$fail_programs"OpenObjZeroLength "

test_dir=src/API/Tests/Session
num_tests=0
num_failed_tests=0
failed_tests=""

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
   verbose=0
   PYTHON=python
fi

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

errcode=0

for test_name in $ok_programs ; do

if [ $verbose = 1 ] ; then
   $top_srcdir/src/Tests/TestWithQuasar.sh $top_srcdir $top_builddir $test_dir $test_name $tcp_port $errcode
else
   $top_srcdir/src/Tests/TestWithQuasar.sh $top_srcdir $top_builddir $test_dir $test_name $tcp_port $errcode >/dev/null 2>&1
fi
if [ "$?" != 0 ] ; then
   echo "FAIL: $test_name "
   num_failed_tests=`expr $num_failed_tests + 1`
   failed_tests=$failed_tests" "$test_name
else
   echo "PASS: $test_name "
fi
num_tests=`expr $num_tests + 1`

done

# -6 for SIGABRT (failure of a pre-condition forces a call to abort()).
# The value "6" might not be always constant across OSes, YMMV.
errcode=-6

for test_name in $fail_programs ; do

if [ $verbose = 1 ] ; then
   $top_srcdir/src/Tests/TestWithQuasar.sh $top_srcdir $top_builddir $test_dir $test_name $tcp_port $errcode
else
   $top_srcdir/src/Tests/TestWithQuasar.sh $top_srcdir $top_builddir $test_dir $test_name $tcp_port $errcode >/dev/null 2>&1
fi
if [ "$?" != 0 ] ; then
   echo "FAIL: $test_name "
   num_failed_tests=`expr $num_failed_tests + 1`
   failed_tests=$failed_tests" "$test_name
else
   echo "PASS: $test_name "
fi
num_tests=`expr $num_tests + 1`

done

echo ""
echo "Number of tests: " $num_tests
echo "Number of failed tests: " $num_failed_tests
if [ $num_failed_tests != 0 ] ; then
   echo "Failed tests: $failed_tests"
fi
echo ""

if [ $num_failed_tests != 0 ] ; then
   exit 1
fi
exit 0

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

