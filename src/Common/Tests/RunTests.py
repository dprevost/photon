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

prefix = os.path.join('DirAccess','Release')
rc = 0

try:
   rc = os.spawnl(os.P_WAIT, 'DirAccess\Release\DirTests.exe', 'DirTests.exe')
   print 'rc = ', rc
   if rc != 0:
      failed_tests.append(os.path.join( prefix, 'DirTests'))
      raise os.error
except:
   print 'DirTests failed! Error = ', rc

for program in dir_ok_programs:
   arg0 = 'Dir' + program
   exe = 'Dir' + program + '.exe'
   path = os.path.join( prefix, exe )
   try:
      rc = os.spawnl( os.P_WAIT, path, path )
      print 'rc = ', rc
      if rc != 0:
         failed_tests.append(os.path.join( prefix, program))
         raise os.error
   except:
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
         failed_tests.append(os.path.join( prefix, program))
         raise os.error
   except:
      print program, ' failed! Error = ', rc
#      raise os.error

l = len(dir_ok_programs)+ len(dir_fail_programs) + 1
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
   

