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
ok_programs.append('GetFirstPass')
ok_programs.append('GetLastPass')
ok_programs.append('InitPass')
ok_programs.append('IsValidPass')
ok_programs.append('PeakFirstPass')
ok_programs.append('PeakLastPass')
ok_programs.append('PeakNextPass')
ok_programs.append('PeakPreviousPass')
ok_programs.append('PutFirstPass')
ok_programs.append('PutLastPass')
ok_programs.append('RemovePass')
ok_programs.append('ReplacePass')
ok_programs.append('ResetPass')
ok_programs.append('TestSmall')
ok_programs.append('TestBig')

#fail_programs.append('RemoveWrongItem')
fail_programs.append('FiniInvSig')
fail_programs.append('FiniNullList')
fail_programs.append('GetFirstInvSig')
fail_programs.append('GetFirstNullItem')
fail_programs.append('GetFirstNullList')
fail_programs.append('GetLastInvSig')
fail_programs.append('GetLastNullItem')
fail_programs.append('GetLastNullList')
fail_programs.append('InitNullList')
fail_programs.append('IsValidInvSig')
fail_programs.append('IsValidNullList')
fail_programs.append('IsValidNullUnknown')
fail_programs.append('PeakFirstInvSig')
fail_programs.append('PeakFirstNullItem')
fail_programs.append('PeakFirstNullList')
fail_programs.append('PeakLastInvSig')
fail_programs.append('PeakLastNullItem')
fail_programs.append('PeakLastNullList')
fail_programs.append('PeakNextInvSig')
fail_programs.append('PeakNextNullCurrent')
fail_programs.append('PeakNextNullCurrNextOffset')
fail_programs.append('PeakNextNullCurrPrevOffset')
fail_programs.append('PeakNextNullList')
fail_programs.append('PeakNextNullNext')
fail_programs.append('PeakPreviousInvSig')
fail_programs.append('PeakPreviousNullCurrent')
fail_programs.append('PeakPrevNullCurrNextOffset')
fail_programs.append('PeakPrevNullCurrPrevOffset')
fail_programs.append('PeakPreviousNullList')
fail_programs.append('PeakPreviousNullPrevious')
fail_programs.append('PutFirstInvSig')
fail_programs.append('PutFirstNullList')
fail_programs.append('PutFirstNullNewItem')
fail_programs.append('PutLastInvSig')
fail_programs.append('PutLastNullList')
fail_programs.append('PutLastNullNewItem')
fail_programs.append('RemoveInvSig')
fail_programs.append('RemoveNullItem')
fail_programs.append('RemoveNullNextOffset')
fail_programs.append('RemoveNullPrevOffset')
fail_programs.append('RemoveNullList')
fail_programs.append('RemoveAbsent')
fail_programs.append('ReplaceInvSig')
fail_programs.append('ReplaceNullList')
fail_programs.append('ReplaceNullNew')
fail_programs.append('ReplaceNullOld')
fail_programs.append('ResetInvSig')
fail_programs.append('ResetNullList')

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

