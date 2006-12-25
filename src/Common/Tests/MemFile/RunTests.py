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
ok_programs.append('BackStatusPass')
ok_programs.append('ClosePass')
ok_programs.append('CreateBackPass')
ok_programs.append('FiniPass')
ok_programs.append('InitPass')
ok_programs.append('OpenPass')
ok_programs.append('ReadOnlyPass')
ok_programs.append('ReadWritePass')
ok_programs.append('SynchPass')
ok_programs.append('Tests')

fail_programs.append('BackStatusInvalidSig')
fail_programs.append('BackStatusMemNull')
fail_programs.append('BackStatusNameEmpty')
fail_programs.append('BackStatusStatusNull')
fail_programs.append('CloseNullError')
fail_programs.append('CloseInvalidSig')
fail_programs.append('CloseMemNull')
fail_programs.append('CreateBackNullError')
fail_programs.append('CreateBackInvalidSig')
fail_programs.append('CreateBackInvLength')
fail_programs.append('CreateBackMemNull')
fail_programs.append('CreateBackNameEmpty')
fail_programs.append('CreateBackPerm0000')
fail_programs.append('CreateBackPerm0200')
fail_programs.append('CreateBackPerm0400')
fail_programs.append('CreateBackPerm0500')
fail_programs.append('FiniInvalidSig')
fail_programs.append('FiniNullMem')
fail_programs.append('InitLengthPositive')
fail_programs.append('InitMemNull')
fail_programs.append('InitNameEmpty')
fail_programs.append('InitNameNull')
fail_programs.append('OpenAddrNull')
fail_programs.append('OpenNullError')
fail_programs.append('OpenInvalidLength')
fail_programs.append('OpenInvalidSig')
fail_programs.append('OpenMemNull')
fail_programs.append('OpenNameEmpty')
fail_programs.append('ReadOnlyNullError')
fail_programs.append('ReadOnlyInvalidBaseAddr')
fail_programs.append('ReadOnlyInvalidSig')
fail_programs.append('ReadOnlyMemNull')
fail_programs.append('ReadOnlyTryWrite')
fail_programs.append('ReadWriteNullError')
fail_programs.append('ReadWriteInvalidSig')
fail_programs.append('ReadWriteInvBaseAddr')
fail_programs.append('ReadWriteMemNull')
fail_programs.append('SynchNullError')
fail_programs.append('SynchInvalidBaseAddr')
fail_programs.append('SynchInvalidSig')
fail_programs.append('SynchMemNull')

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
         failed_tests.append(os.path.join( full_name, program))
   except:
      failed_tests.append(os.path.join( full_name, program))
      print program, ' failed (spawnl threw an exception)!'

for program in fail_programs:
   exe_name = program + '.exe'
   full_name = os.path.join( prog_path, exe_name )
   try:
      rc = os.spawnl( os.P_WAIT, full_name, full_name )
      print program, ', rc = ', rc
      if rc == 0:
         failed_tests.append(os.path.join( full_name, program))
   except:
      failed_tests.append( full_name )
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

