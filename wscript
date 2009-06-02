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

def set_options(opt):
   print '  setting the options'

def configure(conf):
   print '  executing the configuration'
#   conf.write_config_header('config.h')
   print ' qq ' + conf.find_program('doxygen')

def build(bld):
   bld.add_subdirs(['src', 'doc'])

