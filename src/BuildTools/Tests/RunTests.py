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

rc = 0
prog_path = 'Release'
for arg in sys.argv:
   s = arg.split('=')
   if s[0] == '--path':
      prog_path = s[1]

# Start with testParser      
      
exe_name = 'testParser.exe'
full_name = os.path.join( prog_path, exe_name )
try:
   rc = os.spawnl( os.P_WAIT, full_name, full_name )
   print exe_name, ', rc = ', rc
   if rc != 0:
      failed_tests.append(full_name)
except:
   failed_tests.append(full_name)
   print exe_name, ' failed (spawnl threw an exception)!'

# errorParser self-test
exe_name = os.path.join( prog_path, 'errorParser.exe' )
full_name = os.path.join( "..", exe_name )
try:
   rc = os.spawnl( os.P_WAIT, full_name, full_name, "--test" )
   print exe_name, ', rc = ', rc
   if rc != 0:
      failed_tests.append(full_name)
except:
   failed_tests.append(full_name)
   print exe_name, ' failed (spawnl threw an exception)!'

m = len(failed_tests)

print ''
print 'Total number of tests: ', 2
print 'Total number of failed tests: ', m
print ''

for program in failed_tests:
   print 'This test failed: ', program
   
if os.name in ("nt", "dos"):
   print "Type Return to exit"
   sys.stdin.read(1)

if m > 0:
   exit(1)

