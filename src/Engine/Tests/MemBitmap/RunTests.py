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
ok_programs.append('FiniPass')
ok_programs.append('GetLengthPass')
ok_programs.append('InitPass')
ok_programs.append('IsFreePass')
ok_programs.append('SetAllocPass')
ok_programs.append('SetFreePass')

fail_programs.append('FiniNullBitmap')
fail_programs.append('GetLengthPowerOfTwo7')
fail_programs.append('GetLengthPowerOfTwo9')
fail_programs.append('GetLengthZeroGranu')
fail_programs.append('GetLengthZeroLength')
fail_programs.append('InitNullBitmap')
fail_programs.append('InitNullOffset')
fail_programs.append('InitPowerOfTwo7')
fail_programs.append('InitPowerOfTwo9')
fail_programs.append('InitZeroGranu')
fail_programs.append('InitZeroLength')
fail_programs.append('IsFreeNullBitmap')
fail_programs.append('IsFreeNullOffset')
fail_programs.append('SetAllocNullBitmap')
fail_programs.append('SetAllocNullOffset')
fail_programs.append('SetAllocZeroLength')
fail_programs.append('SetFreeNullBitmap')
fail_programs.append('SetFreeNullOffset')

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

