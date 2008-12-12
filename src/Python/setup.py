#!/usr/bin/env python

from distutils.core import setup, Extension

setup(name='photon',
      version='0.5',
      description='Python API for Photon',
      author='D. Prevost',
      author_email='dprevost@photonsoftware.org',
      url='http://photonsoftware.org/',
      ext_modules=[Extension('photon', ['PhotonWrap.c'],
         include_dirs=['../include'],
         library_dirs=['../API/.libs/'],
         libraries=['photon'] )],
     )
