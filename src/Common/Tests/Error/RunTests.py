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
ok_programs.append('AddHandlerPass')
ok_programs.append('AnyErrorPass')
ok_programs.append('ChainErrorPass')
ok_programs.append('FiniPass')
ok_programs.append('GetLastPass')
ok_programs.append('GetLengthPass')
ok_programs.append('InitPass')
ok_programs.append('Tests')

fail_programs.append('AddHandlerNullDef')
fail_programs.append('AddHandlerNullHandler')
fail_programs.append('AddHandlerNullName')
fail_programs.append('AnyErrorNullError')
fail_programs.append('ChainErrorNullDef')
fail_programs.append('ChainErrorNullHandler')
fail_programs.append('ChainErrorInvalidSig')
fail_programs.append('ChainErrorNoSetError')
fail_programs.append('ChainErrorTooMany')
fail_programs.append('FiniInvalidSig')
fail_programs.append('FiniNullError')
fail_programs.append('GetErrorInvalidSig')
fail_programs.append('GetErrorNullError')
fail_programs.append('GetErrorNullMsg')
fail_programs.append('GetErrorZeroLength')
fail_programs.append('GetLastNullError')
fail_programs.append('GetLengthNullError')
fail_programs.append('InitNoInitDefs')
fail_programs.append('InitNullError')
fail_programs.append('SetErrorInvalidSig')
fail_programs.append('SetErrorInvalidValue')
fail_programs.append('SetErrorNullError')

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
      failed_tests.append(os.path.join( full_name, program))
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

