#! /usr/bin/env python
# encoding: utf-8
# :mode=python:  - For jedit

# Copyright (C) 2009 Daniel Prevost <dprevost@photonsoftware.org>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

VERSION='0.5.0'
APPNAME='Photon'
srcdir = '.'
blddir = 'build'

import Options
#import Utils #,
#import Build
from Configure import conf

import sys, os

def set_options(opt):

   opt.add_option('--enable-dbc', action='store_true', default=True, dest='use_dbc',
      help='enable contract-validation checks to the code. You might want to disable this on production systems for performance reasons.')
   opt.add_option('--disable-dbc', action='store_false', dest='use_dbc',
      help='disable contract-validation checks to the code.')
   opt.add_option('--block-size', type='string', dest='block_size',default='4k',
      help='set the size of Photon blocks (values must be either 2k, 4k or 8k)')
   opt.tool_options('gcc')
   opt.tool_options('g++')

def configure(conf):
   print '  executing the configuration'

   conf.check_tool('gcc')
   conf.check_tool('g++')

   conf.check(header_name='time.h')

   conf.sub_config('m4')


#   conf.write_config_header('config.h')
   conf.find_program('doxygen', var='DOXYGEN')
   conf.find_program('latex', var='LATEX')
   conf.find_program('docbook2x-man', var='DOCBOOK2X_MAN')
   conf.find_program('db2x_docbook2man', var='DOCBOOK2X_MAN')
   conf.find_program('db2pdf', var='DB2PDF')
   conf.find_program('errorParser', var='ERROR_PARSER')

#	conf.env["ES"] = not (not Options.options.es)

   block_size = Options.options.block_size
#   block_size = opt.get_option('block_size')
   if (block_size == '2k'):
      conf.define('PSON_BLOCK_SIZE',  2048)
      conf.define('PSON_BLOCK_SHIFT',   11)
   elif (block_size == '4k'):
      conf.define('PSON_BLOCK_SIZE',  4096)
      conf.define('PSON_BLOCK_SHIFT',   12)
   elif (block_size == '8k'):
      conf.define('PSON_BLOCK_SIZE',  8192)
      conf.define('PSON_BLOCK_SHIFT',   13)
   else:
      conf.fatal('Invalid value! Values allowed: 2k, 4k or 8k')

   conf.write_config_header('config.h')

#   print dir(conf)
   
def build(bld):
   bld.add_subdirs(['src', 'doc'])
   print bld.env['DOXYGEN']
   print sys.platform, ' ', os.name

