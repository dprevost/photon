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
ok_programs.append('AddPass')
ok_programs.append('CommitPass')
ok_programs.append('FiniPass')
ok_programs.append('InitPass')
ok_programs.append('RemoveLastPass')
ok_programs.append('RollbackPass')

fail_programs.append('AddInvalidSig')
fail_programs.append('AddNullChildOffset')
fail_programs.append('AddNullContext')
fail_programs.append('AddNullParentOffset')
fail_programs.append('AddNullTx')
fail_programs.append('CommitInvalidSig')
fail_programs.append('CommitNullContext')
fail_programs.append('CommitNullTx')
fail_programs.append('FiniInvalidSig')
fail_programs.append('FiniNullContext')
fail_programs.append('FiniNullTx')
fail_programs.append('InitNullContext')
fail_programs.append('InitNullTx')
fail_programs.append('InitZeroBlocks')
fail_programs.append('RemoveLastInvalidSig')
fail_programs.append('RemoveLastNullContext')
fail_programs.append('RemoveLastNullTx')
fail_programs.append('RollbackInvalidSig')
fail_programs.append('RollbackNullContext')
fail_programs.append('RollbackNullTx')

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

