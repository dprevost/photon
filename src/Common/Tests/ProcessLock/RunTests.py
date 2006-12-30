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
ok_programs.append('AcquirePass')
ok_programs.append('FiniPass')
ok_programs.append('InitPass')
ok_programs.append('IsItLockedPass')
ok_programs.append('LockConcurrency')
ok_programs.append('LockShouldFail')
ok_programs.append('LockTests')
ok_programs.append('ReleasePass')
ok_programs.append('TestPidPass')
ok_programs.append('TryAcqPass')

fail_programs.append('AcquireInvalidSig')
fail_programs.append('AcquireNullLock')
fail_programs.append('AcquireZeroValue')
fail_programs.append('FiniInvalidSig')
fail_programs.append('FiniNullLock')
fail_programs.append('InitNullLock')
fail_programs.append('IsItLockedInvalidSig')
fail_programs.append('IsItLockedNullLock')
fail_programs.append('ReleaseInvalidSig')
fail_programs.append('ReleaseNullLock')
fail_programs.append('TestPidInvalidSig')
fail_programs.append('TestPidNullLock')
fail_programs.append('TestPidZeroPid')
fail_programs.append('TryAcqInvalidSig')
fail_programs.append('TryAcqNullLock')
fail_programs.append('TryAcqZeroValue')

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

full_name = os.path.join( prog_path, 'LockConcurrency.exe' )
try:
   rc = os.spawnl( os.P_WAIT, full_name, full_name, '-m', 'try' )
   print 'LockConcurrency -m try, rc = ', rc
   if rc != 0:
      failed_tests.append(full_name)
except:
   failed_tests.append(full_name)
   print 'LockConcurrency -m try failed (spawnl threw an exception)!'

l += len(ok_programs)+ len(fail_programs)
# Add LockConcurrency -m try
l += 1

m = len(failed_tests)

print ''
print 'Total number of tests: ', l
print 'Total number of failed tests: ', m
print ''

for program in failed_tests:
   print 'This test failed: ', program
   
os.unlink('Memfile.mem')

if os.name in ("nt", "dos"):
   print "Type Return to exit"
   sys.stdin.read(1)

if m > 0:
   exit(1)

