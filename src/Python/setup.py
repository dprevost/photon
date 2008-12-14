#!/usr/bin/env python

from distutils.core import setup, Extension

setup(name='photon',
      version='0.5',
      description='Python API for Photon',
      author='D. Prevost',
      author_email='dprevost@photonsoftware.org',
      url='http://photonsoftware.org/',
#      packages=['pso'],
      ext_modules=[Extension('pso', 
                             ['src/pso.c'], 
                             libraries=['photon'])
#      ext_modules=[Extension('simpletype', 
#                             ['photon.c'], 
#                             libraries=['photon'])

      ]
     )

