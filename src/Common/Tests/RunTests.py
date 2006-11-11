import os
import sys

# List of failed tests
failed_tests = []

# Programs that should return 0
dir_ok_programs = []
dir_ok_programs.append('ClosePass')
dir_ok_programs.append('FiniPass')
dir_ok_programs.append('GetNextNoSuchDir')
dir_ok_programs.append('GetNextPass')
dir_ok_programs.append('InitPass')
dir_ok_programs.append('OpenNoSuchDir')
dir_ok_programs.append('OpenPass')
dir_ok_programs.append('Tests')

dir_fail_programs = []
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

err_ok_programs = []
err_ok_programs.append('AddHandlerPass')
err_ok_programs.append('ChainErrorPass')
err_ok_programs.append('FiniPass')
err_ok_programs.append('InitPass')
err_ok_programs.append('Tests')

err_fail_programs = []
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

mem_ok_programs = []
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

mem_fail_programs = []
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

opt_ok_programs = []
opt_ok_programs.append('GetLongOptPass')
opt_ok_programs.append('GetShortOptPass')
opt_ok_programs.append('IsLongPresentPass')
opt_ok_programs.append('IsShortPresentPass')
opt_ok_programs.append('SetOptionsPass')
opt_ok_programs.append('ShowUsagePass')
opt_ok_programs.append('Tests')
opt_ok_programs.append('ValidatePass')

opt_fail_programs = []
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

# DirAccess
prefix = os.path.join('DirAccess','Release')
rc = 0

for program in dir_ok_programs:
   arg0 = 'Dir' + program
   exe = 'Dir' + program + '.exe'
   path = os.path.join( prefix, exe )
   try:
      rc = os.spawnl( os.P_WAIT, path, path )
      print 'rc = ', rc
      if rc != 0:
         raise os.error
   except:
      failed_tests.append(os.path.join( prefix, program))
      print program, ' failed! Error = ', rc
#      raise os.error

for program in dir_fail_programs:
   arg0 = 'Dir' + program
   exe = 'Dir' + program + '.exe'
   path = os.path.join( prefix, exe )
   try:
      rc = os.spawnl( os.P_WAIT, path, path )
      print 'rc = ', rc
      if rc == 0:
         raise os.error
   except:
      failed_tests.append(os.path.join( prefix, program))
      print program, ' failed! Error = ', rc
#      raise os.error

### Error tests

prefix = os.path.join('Error','Release')
for program in err_ok_programs:
   arg0 = 'Err' + program
   exe = 'Err' + program + '.exe'
   path = os.path.join( prefix, exe )
   try:
      rc = os.spawnl( os.P_WAIT, path, path )
      print 'rc = ', rc
      if rc != 0:
         raise os.error
   except:
      failed_tests.append(os.path.join( prefix, program))
      print program, ' failed! Error = ', rc
#      raise os.error

for program in err_fail_programs:
   arg0 = 'Err' + program
   exe = 'Err' + program + '.exe'
   path = os.path.join( prefix, exe )
   try:
      rc = os.spawnl( os.P_WAIT, path, path )
      print 'rc = ', rc
      if rc == 0:
         raise os.error
   except:
      failed_tests.append(os.path.join( prefix, program))
      print program, ' failed! Error = ', rc
#      raise os.error

### Memfile tests

prefix = os.path.join('MemFile','Release')
for program in mem_ok_programs:
   arg0 = 'Mem' + program
   exe = 'Mem' + program + '.exe'
   path = os.path.join( prefix, exe )
   try:
      rc = os.spawnl( os.P_WAIT, path, path )
      print 'rc = ', rc
      if rc != 0:
         raise os.error
   except:
      failed_tests.append(os.path.join( prefix, program))
      print program, ' failed! Error = ', rc
#      raise os.error

for program in mem_fail_programs:
   arg0 = 'Mem' + program
   exe = 'Mem' + program + '.exe'
   path = os.path.join( prefix, exe )
   try:
      rc = os.spawnl( os.P_WAIT, path, path )
      print 'rc = ', rc
      if rc == 0:
         raise os.error
   except Exception, e:
      failed_tests.append(os.path.join( prefix, program))
      print program, ' failed! Error = ', rc
#      print "Exception is:", sys.exc_info()[0], e.args
#      raise os.error

prefix = os.path.join('Options','Release')
for program in opt_ok_programs:
   arg0 = 'Opt' + program
   exe = 'Opt' + program + '.exe'
   path = os.path.join( prefix, exe )
   try:
      rc = os.spawnl( os.P_WAIT, path, path )
      print 'rc = ', rc
      if rc != 0:
         raise os.error
   except:
      failed_tests.append(os.path.join( prefix, program))
      print program, ' failed! Error = ', rc
#      raise os.error

for program in opt_fail_programs:
   arg0 = 'Opt' + program
   exe = 'Opt' + program + '.exe'
   path = os.path.join( prefix, exe )
   try:
      rc = os.spawnl( os.P_WAIT, path, path )
      print 'rc = ', rc
      if rc == 0:
         raise os.error
   except Exception, e:
      failed_tests.append(os.path.join( prefix, program))
      print program, ' failed! Error = ', rc
#      print "Exception is:", sys.exc_info()[0], e.args
#      raise os.error

l  = len(dir_ok_programs)+ len(dir_fail_programs)
l += len(err_ok_programs)+ len(err_fail_programs)
l += len(mem_ok_programs)+ len(mem_fail_programs)
l += len(opt_ok_programs)+ len(opt_fail_programs)
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
   

