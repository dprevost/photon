#
# Copyright (C) 2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

import os
import sys

# List of failed tests. We append to this list when an error is encountered
# while running the tests
failed_tests = []

# Lists containing the names of the tests
# The "ok" lists are for programs which are expected to return zero (succeed)
# and the "fail" lists are for the other ones.
ok_programs = []
fail_programs = []

##
## Now populate the program lists...
##
# Programs that should return 0

ok_programs.append('DeletePass')
ok_programs.append('EmptyPass')
ok_programs.append('FiniPass')
ok_programs.append('GetFirstPass')
ok_programs.append('GetNextPass')
ok_programs.append('GetPass')
ok_programs.append('InitPass')
ok_programs.append('InsertPass')
ok_programs.append('ResizePass')
ok_programs.append('GetNullBucket')

fail_programs.append('DeleteNullContext')
fail_programs.append('DeleteNullHash')
fail_programs.append('DeleteNullKey')
fail_programs.append('DeleteZeroLength')
fail_programs.append('EmptyNullContext')
fail_programs.append('EmptyNullHash')
fail_programs.append('FiniNullContext')
fail_programs.append('FiniNullHash')
fail_programs.append('GetFirstNullBucket')
fail_programs.append('GetFirstNullHash')
fail_programs.append('GetFirstNullOffset')
fail_programs.append('GetNextNullHash')
fail_programs.append('GetNextNullNextBucket')
fail_programs.append('GetNextNullNextOffset')
fail_programs.append('GetNullContext')
fail_programs.append('GetNullHash')
fail_programs.append('GetNullItem')
fail_programs.append('GetNullKey')
fail_programs.append('GetZeroKeyLength')
fail_programs.append('InitNullContext')
fail_programs.append('InitNullHash')
fail_programs.append('InsertNullContext')
fail_programs.append('InsertNullData')
fail_programs.append('InsertNullHash')
fail_programs.append('InsertNullKey')
fail_programs.append('InsertNullNewItem')
fail_programs.append('InsertZeroDataLength')
fail_programs.append('InsertZeroKeyLength')
fail_programs.append('ResizeNullContext')
fail_programs.append('ResizeNullHash')

l = 0
rc = 0
prog_path = 'Release'
for arg in sys.argv:
   s = arg.split('=')
   if s[0] == '--path':
      prog_path = s[1]

for program in ok_programs:
   exe_name = program + '.exe'
   full_name = os.path.join( prog_path, exe_name )
   try:
      rc = os.spawnl( os.P_WAIT, full_name, full_name )
      print program, ', rc = ', rc
      if rc != 0:
         failed_tests.append(full_name)
   except:
      failed_tests.append(full_name)
      print program, ' failed (spawnl threw an exception)!'

for program in fail_programs:
   exe_name = program + '.exe'
   full_name = os.path.join( prog_path, exe_name )
   try:
      rc = os.spawnl( os.P_WAIT, full_name, full_name )
      print program, ', rc = ', rc
      if rc == 0:
         failed_tests.append(full_name)
   except:
      failed_tests.append(full_name)
      print program, ' failed (spawnl threw an exception)!'

l += len(ok_programs)+ len(fail_programs)
   
m = len(failed_tests)

print ''
print 'Total number of tests: ', l
print 'Total number of failed tests: ', m
print ''

for program in failed_tests:
   print 'This test failed: ', program
   
if os.name in ("nt", "dos"):
   print "Type Return to exit"
   sys.stdin.read(1)

if m > 0:
   exit(1)

