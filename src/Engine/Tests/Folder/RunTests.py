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
ok_programs.append('FiniPass')
ok_programs.append('GetPass')
ok_programs.append('InitPass')
ok_programs.append('InsertPass')
ok_programs.append('Tests')

fail_programs.append('DeleteInvalidSig')
fail_programs.append('DeleteNullContext')
fail_programs.append('DeleteNullFolder')
fail_programs.append('DeleteNullName')
fail_programs.append('DeleteZeroNameLength')
fail_programs.append('FiniInvalidSig')
fail_programs.append('FiniNullContext')
fail_programs.append('FiniNullFolder')
fail_programs.append('GetInvalidSig')
fail_programs.append('GetNullContext')
fail_programs.append('GetNullDescriptor')
fail_programs.append('GetNullFolder')
fail_programs.append('GetNullName')
fail_programs.append('GetZeroNameLength')
fail_programs.append('InitNullContext')
fail_programs.append('InitNullFolder')
fail_programs.append('InitNullName')
fail_programs.append('InitNullParentOffset')
fail_programs.append('InitZeroBlocks')
fail_programs.append('InitZeroNameLength')
fail_programs.append('InsertInvalidSig')
fail_programs.append('InsertNullContext')
fail_programs.append('InsertNullFolder')
fail_programs.append('InsertNullName')
fail_programs.append('InsertNullOrigName')
fail_programs.append('InsertZeroBlocks')
fail_programs.append('InsertZeroNameLength')


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

