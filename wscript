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

#def set_options(opt):
#   print '  setting the options'
def set_options(opt):

   opt.add_option('--enable-dbc', action = 'store_true', default = True, dest = 'use_dbc',
      help = 'enable contract-validation checks to the code. You might want to disable this on production systems for performance reasons.')
   opt.add_option('--disable-dbc', action = 'store_false', dest = 'use_dbc',
      help = 'disable contract-validation checks to the code.')
   opt.add_option('--set-block-size', type = 'string', dest = 'block_size',
      help = 'set the size of Photon blocks (values must be either 2k, 4k or 8k)')

def configure(conf):
   print '  executing the configuration'
#   conf.write_config_header('config.h')
   conf.find_program('doxygen', var='DOXYGEN')
   conf.find_program('latex', var='LATEX')
   conf.find_program('docbook2x-man')
   conf.find_program('db2x_docbook2man')
   conf.find_program('db2pdf')
   conf.find_program('errorParser')
   
   AC_CHECK_PROG([LATEX_IS_PRESENT], [latex], [yes], [no])
AM_CONDITIONAL([COND_USE_LATEX], [test "$LATEX_IS_PRESENT" = yes])

# Tell our makefiles that we can use docbook2x to regenerate some 
# of the documentation, as needed.
AC_CHECK_PROG([DOCBOOK2XMAN_IS_PRESENT], [docbook2x-man], [yes], [no])
AM_CONDITIONAL([COND_USE_DOCBOOK2XMAN], [test "$DOCBOOK2XMAN_IS_PRESENT" = yes])
# On fedora, the exe has a different name
AC_CHECK_PROG([DOCBOOK2MAN_IS_PRESENT], [db2x_docbook2man], [yes], [no])
AM_CONDITIONAL([COND_USE_DOCBOOK2MAN], [test "$DOCBOOK2MAN_IS_PRESENT" = yes])

# Tell our makefiles if we can use db2pdf to regenerate some 
# of the documentation, as needed.
AC_CHECK_PROG([DB2PDF_IS_PRESENT], [db2pdf], [yes], [no])
AM_CONDITIONAL([COND_USE_DB2PDF], [test "$DB2PDF_IS_PRESENT" = yes])

# Tell our makefiles that we can use errorParser to regenerate the error
# handling code, as needed.
AC_CHECK_PROG([ERRORPARSER_IS_PRESENT], [errorParser], [yes], [no])
AM_CONDITIONAL([COND_USE_ERRORPARSER], [test "$ERRORPARSER_IS_PRESENT" = yes])


def build(bld):
   bld.add_subdirs(['src', 'doc'])

