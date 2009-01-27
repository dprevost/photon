# Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
# 
# This file is part of Photon (photonsoftware.org).
#
# This file may be distributed and/or modified under the terms of the
# GNU General Public License version 2 or version 3 as published by the 
# Free Software Foundation and appearing in the file COPYING.GPL2 and 
# COPYING.GPL3 included in the packaging of this software.
#
# Licensees holding a valid Photon Commercial license can use this file 
# in accordance with the terms of their license.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
#
# To run these tests standalone (outside the scope of make check), you
# must provide these arguments:
#
#  - the path of the top directory of the source
#  - the path of the top directory of the build
#  - the relative path of the test to be run (versus the build dir)
#  - the name of the test to be run
#  - the tcp/ip port for the server
#  - expected error code
#  - the path of the directory for the shared memory
#
# For example:
#
# python TestWithQuasar.py /home/dprevost/photon/ home/dprevost/photon \
#        src/API/Tests/Api InitPass 10701 /tmp/photon_InitPass
#
# (the build directory will usually be the top directory unless you're
# building using VPATH (the simplest example being "make dist"))
#
# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

import sys, os, signal
import time
import socket

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
#
# Some global variables 
#
# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

if len(sys.argv) != 8:
	print 'usage: sys.argv[0] src_dir build_dir test_dir test_name tcp_port mem_dir'
	sys.exit(-1)

src_dir   = sys.argv[1]
build_dir = sys.argv[2]
test_dir  = os.path.join( build_dir, sys.argv[3] )
test_name = sys.argv[4]
tcp_port  = sys.argv[5]
errcode   = int(sys.argv[6])
mem_dir   = sys.argv[7]

return_code = 0
cfg_suff = 'cfg.xml'
qsr_name = 'quasar'
qsr_pid = 0
qsr_present = 0

qsr_dir = os.path.join( os.path.join( build_dir, 'src' ), 'Quasar' )
cfg_name = os.path.join( mem_dir, cfg_suff )

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
#
# Test to see if the server is up and running (by trying to connect 
# to the selected tcp port). 
#
# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

def IsQuasarRunning():

   global tcp_port

   try:
      sock = socket.socket (socket.AF_INET, socket.SOCK_STREAM )
   except:
      print "Unexpected error in socket():", sys.exc_info()[0]
      raise

   try:
      sock.connect( ('127.0.0.1', int(tcp_port)) )
   except socket.error, msg:
      return 0
   except:
      print "Unexpected error:", sys.exc_info()[0]
      raise

#  print 'sock.connect ok'
   sock.close()
   return 1


# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
#
# Start the server 
#
# Note that we do not daemonize it since we would loose the pid if we 
# did (it is possible to kill it even without the pid but it is more
# complex (not very portable) so, for the moment, we'll keep it simple).
#
# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

def StartQuasar():

   global qsr_dir, qsr_name, cfg_name, qsr_pid

   path = os.path.join( qsr_dir, qsr_name )
   try:
      qsr_pid = os.spawnl( os.P_NOWAIT, path, path, '--config',  cfg_name )
   except OSError, (errno, strerror):
      print 'Error starting the server'
      print "OS error(%s): %s" % (errno, strerror)
      raise
   except:
      print "Unexpected error in StartQuasar():", sys.exc_info()[0]
      raise

   # We sleep a bit (if needed) until we know for sure that the server
   # has the necessary time to properly start. 
   # If an exception occurs in IsQuasarRunning() the next function
   # on the stack will handle it.

   count = 0
   while IsQuasarRunning() == 0:
      time.sleep(0.1)
      count = count + 1
      if count > 10:
         print 'Quasar refuses to start'
         raise os.error

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

def StopQuasar():

   global qsr_pid

   if qsr_pid > 0:
      os.kill( qsr_pid, signal.SIGINT )

   # We sleep a bit (if needed) since signals are asynch and we need to 
   # make sure that the server is stopped (since other tests might be 
   # run just after this one). 
  
   try:
      count = 0
      while IsQuasarRunning() == 1:
         time.sleep(1)
         count = count + 1
         if count > 5:
#        print 'Unknown error - Quasar cannot be killed (still running?)'
            raise os.error

   # oh oh... no way to handle this gracefully (and we are in the process of 
   # exiting/cleaning up). Try SIGKILL and hope for the best...
   except:
      os.kill( qsr_pid, signal.SIGKILL )
#    time.sleep(5)
      try:
         count = 0
         while IsQuasarRunning() == 1:
            time.sleep(1)
            count = count + 1
            if count > 5:
               print 'Unknown error - Quasar cannot be killed '
               raise os.error
      except:
         raise

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
#
# Write the configuration file for the server
#
# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

def WriteCfg():

   global cfg_name, mem_dir

   try:
      cfg_file = open( cfg_name, 'w' )
   except IOError, (errno, strerror):
      print 'Error opening cfg file in WriteCfg()'
      print "I/O error(%s): %s" % (errno, strerror)
      raise
   except:
      print "Unexpected error:", sys.exc_info()[0]
      raise

   try:
      line = '<?xml version=\"1.0\"?>'
      cfg_file.write( line )   
      line = '<quasar_config xmlns=\"http://photonsoftware.org/quasarConfig\"'
      cfg_file.write( line )   
      line = 'xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"'
      cfg_file.write( line )   
      line = 'xsi:schemaLocation=\"http://photonsoftware.org/quasarConfig ' + os.path.join( mem_dir, 'quasar_config.xsd' ) + '\"> '
      cfg_file.write( line )   
      line = '  <mem_location>' + mem_dir + '</mem_location>'
      cfg_file.write( line )   
      line = '  <mem_size size=\"10240\" units=\"kb\" />'
      cfg_file.write( line )   
      line = '  <quasar_address>10701</quasar_address>'
      cfg_file.write( line )   
      line = '  <file_access access=\"group\" />'
      cfg_file.write( line )   
      line = '</quasar_config>'
      cfg_file.write( line )   
      cfg_file.close()
   except:
      print 'Error in WriteCfg()'
      raise os.error

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

def RunTest():

   global test_dir, test_name, tcp_port, errcode, src_dir

   path = os.path.join( test_dir, test_name )
   try:
      rc = os.spawnl( os.P_WAIT, path, path, tcp_port, src_dir )
      if rc != errcode:
         return -1
   except OSError, (errno, strerror):
      print 'Error running the test '
      print "OS error(%s): %s" % (errno, strerror)
      raise
   except:
      print "Unexpected error in RunTest():", sys.exc_info()[0]
      raise

   return 0

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

def Run():

   global return_code, qsr_present

   # We use try:/except blocks to determine our exit code unless the
   # error occurs in the C program.
 
   try:
      qsr_present = IsQuasarRunning()
      if qsr_present == 0:
         WriteCfg()
         StartQuasar()
#         time.sleep(1)
      return_code = RunTest()
   except:
      return_code = -1
 
# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

Run()
if qsr_present == 0:
   StopQuasar()
#if return_code == 0:
#   print 'PASS:', test_name, '(Python test)'
#else:
#   print 'FAIL:', test_name, '(Python test)'

sys.exit(return_code)

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
