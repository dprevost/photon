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
ok_programs.append('ClosePass')
ok_programs.append('FiniPass')
ok_programs.append('GetNextNoSuchDir')
ok_programs.append('GetNextPass')
ok_programs.append('InitPass')
ok_programs.append('OpenNoSuchDir')
ok_programs.append('OpenPass')
ok_programs.append('Tests')

fail_programs.append('CloseInvalidSig')
fail_programs.append('CloseNullDir')
fail_programs.append('FiniInvalidSig')
fail_programs.append('FiniNullDir')
fail_programs.append('GetNextInvalidSig')
fail_programs.append('GetNextNoOpen')
fail_programs.append('GetNextNullDir')
fail_programs.append('GetNextNullError')
fail_programs.append('GetNextNullpDir')
fail_programs.append('InitNullDir')
fail_programs.append('OpenCallTwice')
fail_programs.append('OpenInvalidSig')
fail_programs.append('OpenNoInit')
fail_programs.append('OpenNullDir')
fail_programs.append('OpenNullDirname')
fail_programs.append('OpenNullError')

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

