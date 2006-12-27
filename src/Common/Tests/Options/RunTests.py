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
ok_programs.append('GetLongOptPass')
ok_programs.append('GetShortOptPass')
ok_programs.append('IsLongPresentPass')
ok_programs.append('IsShortPresentPass')
ok_programs.append('SetOptionsPass')
ok_programs.append('ShowUsagePass')
ok_programs.append('Tests')
ok_programs.append('ValidatePass')

fail_programs.append('GetLongOptNullHandle')
fail_programs.append('GetLongOptNullOpt')
fail_programs.append('GetLongOptNullValue')
fail_programs.append('GetShortOptNullHandle')
fail_programs.append('GetShortOptNullValue')
fail_programs.append('IsLongPresentNullHandle')
fail_programs.append('IsLongPresentNullOpt')
fail_programs.append('IsShortPresentNullHandle')
fail_programs.append('SetOptionsNullHandle')
fail_programs.append('SetOptionsNullOpts')
fail_programs.append('SetOptionsNullEndedOpt')
fail_programs.append('SetOptionsNullEndedComment')
fail_programs.append('SetOptionsNullEndedValue')
fail_programs.append('SetOptionsNumOptsZero')
fail_programs.append('SetOptionsRepeatedLong')
fail_programs.append('SetOptionsRepeatedShort')
fail_programs.append('ShowUsageNullAddArgs')
fail_programs.append('ShowUsageNullHandle')
fail_programs.append('ShowUsageNullProgName')
fail_programs.append('UnsetOptNullHandle')
fail_programs.append('ValidateArgcZero')
fail_programs.append('ValidateNullArgv')
fail_programs.append('ValidateNULLArgvi')
fail_programs.append('ValidateNullHandle')

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

