import os
import sys


# List of failed tests. We append to this list when an error is encountered
# while running the tests
failed_tests = []

# Lists containing the names of the test (one list for each subdir)
# The "ok" lists are for programs which are expected to return zero (succeed)
# and the "fail" lists are for the other ones.
dir_ok_programs = []
dir_fail_programs = []
err_ok_programs = []
err_fail_programs = []
mem_ok_programs = []
mem_fail_programs = []
opt_ok_programs = []
opt_fail_programs = []

# Lists containing all the info needed to run the for loops at the end of
# this file. It contains the 2 lists, the prefix used to name the .exe and
# the exact name of the subdir.
dir_list = [dir_ok_programs, dir_fail_programs, 'Dir', 'DirAccess']
err_list = [err_ok_programs, err_fail_programs, 'Err', 'Error']
mem_list = [mem_ok_programs, mem_fail_programs, 'Mem', 'MemFile']
opt_list = [opt_ok_programs, opt_fail_programs, 'Opt', 'Options']

# And the master list, the list of all subdir lists.
master_list = [dir_list, err_list, mem_list, opt_list]

##
## Now populate the program lists...
##
# Programs that should return 0
dir_ok_programs.append('ClosePass')
dir_ok_programs.append('FiniPass')
dir_ok_programs.append('GetNextNoSuchDir')
dir_ok_programs.append('GetNextPass')
dir_ok_programs.append('InitPass')
dir_ok_programs.append('OpenNoSuchDir')
dir_ok_programs.append('OpenPass')
dir_ok_programs.append('Tests')

dir_fail_programs.append('CloseInvalidSig')
dir_fail_programs.append('CloseNullDir')
dir_fail_programs.append('FiniInvalidSig')
dir_fail_programs.append('FiniNullDir')
dir_fail_programs.append('GetNextInvalidSig')
dir_fail_programs.append('GetNextNoOpen')
dir_fail_programs.append('GetNextNullDir')
dir_fail_programs.append('GetNextNullError')
dir_fail_programs.append('GetNextNullpDir')
dir_fail_programs.append('InitNullDir')
dir_fail_programs.append('OpenCallTwice')
dir_fail_programs.append('OpenInvalidSig')
dir_fail_programs.append('OpenNoInit')
dir_fail_programs.append('OpenNullDir')
dir_fail_programs.append('OpenNullDirname')
dir_fail_programs.append('OpenNullError')

err_ok_programs.append('AddHandlerPass')
err_ok_programs.append('ChainErrorPass')
err_ok_programs.append('FiniPass')
err_ok_programs.append('InitPass')
err_ok_programs.append('Tests')

err_fail_programs.append('AddHandlerNullDef')
err_fail_programs.append('AddHandlerNullHandler')
err_fail_programs.append('AddHandlerNullName')
err_fail_programs.append('ChainErrorNullDef')
err_fail_programs.append('ChainErrorNullHandler')
err_fail_programs.append('ChainErrorInvalidSig')
err_fail_programs.append('ChainErrorNoSetError')
err_fail_programs.append('ChainErrorTooMany')
err_fail_programs.append('FiniInvalidSig')
err_fail_programs.append('FiniNullError')
err_fail_programs.append('GetErrorInvalidSig')
err_fail_programs.append('GetErrorNullError')
err_fail_programs.append('GetErrorNullMsg')
err_fail_programs.append('GetErrorZeroLength')
err_fail_programs.append('InitNoInitDefs')
err_fail_programs.append('InitNullError')
err_fail_programs.append('SetErrorInvalidSig')
err_fail_programs.append('SetErrorInvalidValue')
err_fail_programs.append('SetErrorNullError')

mem_ok_programs.append('BackStatusPass')
mem_ok_programs.append('ClosePass')
mem_ok_programs.append('CreateBackPass')
mem_ok_programs.append('FiniPass')
mem_ok_programs.append('InitPass')
mem_ok_programs.append('OpenPass')
mem_ok_programs.append('ReadOnlyPass')
mem_ok_programs.append('ReadWritePass')
mem_ok_programs.append('SynchPass')
mem_ok_programs.append('Tests')

mem_fail_programs.append('BackStatusInvalidSig')
mem_fail_programs.append('BackStatusMemNull')
mem_fail_programs.append('BackStatusNameEmpty')
mem_fail_programs.append('BackStatusStatusNull')
mem_fail_programs.append('CloseInfoNull')
mem_fail_programs.append('CloseInvalidSig')
mem_fail_programs.append('CloseMemNull')
mem_fail_programs.append('CreateBackInfoNull')
mem_fail_programs.append('CreateBackInvalidSig')
mem_fail_programs.append('CreateBackInvLength')
mem_fail_programs.append('CreateBackMemNull')
mem_fail_programs.append('CreateBackNameEmpty')
mem_fail_programs.append('CreateBackPerm0000')
mem_fail_programs.append('CreateBackPerm0200')
mem_fail_programs.append('CreateBackPerm0400')
mem_fail_programs.append('CreateBackPerm0500')
mem_fail_programs.append('FiniInvalidSig')
mem_fail_programs.append('FiniNullMem')
mem_fail_programs.append('InitLengthPositive')
mem_fail_programs.append('InitMemNull')
mem_fail_programs.append('InitNameEmpty')
mem_fail_programs.append('InitNameNull')
mem_fail_programs.append('OpenAddrNull')
mem_fail_programs.append('OpenInfoNull')
mem_fail_programs.append('OpenInvalidLength')
mem_fail_programs.append('OpenInvalidSig')
mem_fail_programs.append('OpenMemNull')
mem_fail_programs.append('OpenNameEmpty')
mem_fail_programs.append('ReadOnlyInfoNull')
mem_fail_programs.append('ReadOnlyInvalidBaseAddr')
mem_fail_programs.append('ReadOnlyInvalidSig')
mem_fail_programs.append('ReadOnlyMemNull')
mem_fail_programs.append('ReadOnlyTryWrite')
mem_fail_programs.append('ReadWriteInfoNull')
mem_fail_programs.append('ReadWriteInvalidSig')
mem_fail_programs.append('ReadWriteInvBaseAddr')
mem_fail_programs.append('ReadWriteMemNull')
mem_fail_programs.append('SynchInfoNull')
mem_fail_programs.append('SynchInvalidBaseAddr')
mem_fail_programs.append('SynchInvalidSig')
mem_fail_programs.append('SynchMemNull')

opt_ok_programs.append('GetLongOptPass')
opt_ok_programs.append('GetShortOptPass')
opt_ok_programs.append('IsLongPresentPass')
opt_ok_programs.append('IsShortPresentPass')
opt_ok_programs.append('SetOptionsPass')
opt_ok_programs.append('ShowUsagePass')
opt_ok_programs.append('Tests')
opt_ok_programs.append('ValidatePass')

opt_fail_programs.append('GetLongOptNullHandle')
opt_fail_programs.append('GetLongOptNullOpt')
opt_fail_programs.append('GetLongOptNullValue')
opt_fail_programs.append('GetShortOptNullHandle')
opt_fail_programs.append('GetShortOptNullValue')
opt_fail_programs.append('IsLongPresentNullHandle')
opt_fail_programs.append('IsLongPresentNullOpt')
opt_fail_programs.append('IsShortPresentNullHandle')
opt_fail_programs.append('SetOptionsNullHandle')
opt_fail_programs.append('SetOptionsNullOpts')
opt_fail_programs.append('SetOptionsNullEndedOpt')
opt_fail_programs.append('SetOptionsNullEndedComment')
opt_fail_programs.append('SetOptionsNullEndedValue')
opt_fail_programs.append('SetOptionsNumOptsZero')
opt_fail_programs.append('SetOptionsRepeatedLong')
opt_fail_programs.append('SetOptionsRepeatedShort')
opt_fail_programs.append('ShowUsageNullAddArgs')
opt_fail_programs.append('ShowUsageNullHandle')
opt_fail_programs.append('ShowUsageNullProgName')
opt_fail_programs.append('UnsetOptNullHandle')
opt_fail_programs.append('ValidateArgcZero')
opt_fail_programs.append('ValidateNullArgv')
opt_fail_programs.append('ValidateNULLArgvi')
opt_fail_programs.append('ValidateNullHandle')

l = 0
for my_list in master_list:
   rc = 0
   prog_path = os.path.join(my_list[3],'Release')

   for program in my_list[0]:
      exe_name = my_list[2] + program + '.exe'
      full_name = os.path.join( prog_path, exe_name )
      try:
         rc = os.spawnl( os.P_WAIT, full_name, full_name )
         print program, ', rc = ', rc
         if rc != 0:
            failed_tests.append(os.path.join( full_name, program))
      except:
         failed_tests.append(os.path.join( full_name, program))
         print program, ' failed (spawnl threw an exception)!'

   for program in my_list[1]:
      exe_name = my_list[2] + program + '.exe'
      full_name = os.path.join( prog_path, exe_name )
      try:
         rc = os.spawnl( os.P_WAIT, full_name, full_name )
         print program, ', rc = ', rc
         if rc == 0:
            failed_tests.append(os.path.join( full_name, program))
      except:
         failed_tests.append(os.path.join( full_name, program))
         print program, ' failed (spawnl threw an exception)!'
   print 'ZZZ ', len(my_list[0]), len(my_list[1])
   l += len(my_list[0])+ len(my_list[1])
   
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
   

