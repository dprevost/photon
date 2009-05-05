#!/bin/sh
#
# Copyright (C) 2006-2009 Daniel Prevost <dprevost@photonsoftware.org>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

##########################################################################

# Test our version of automake - we need at least 1.5 for nobase_ 
# although the code was always tested with 1.7 or greater. 
 
echo "Testing the version of automake."

am_version=`automake --version | head -1 | awk '{print $4}'`
am_major=`echo $am_version | awk -F . '{print $1}'`
am_minor=`echo $am_version | awk -F . '{print $2}'`

# The next one is needed because the format changed from somethink like 1.4-p6 
# to something like 1.7.8. 
am_minor=`echo $am_minor | awk -F - '{print $1}'`

#
if [ $am_major -lt 1 ] || [ $am_major -eq 1 -a $am_minor -lt 5 ] 
then
   echo "Error: automake 1.5 or later is required (your version is $am_version)."
   exit 1
fi

echo "Automake version: $am_version - all is ok"

##########################################################################

## Some cleanup, just in case (probably overkill...).

echo "-----------------------------------------------"
echo "Cleanup phase - Warning messages are normal" 
echo "------------------------------------------------"

if [ -f Makefile ] 
then 
  make distclean
fi

rm -f config.cache
rm -f autom4te.cache/output*
rm -f autom4te.cache/requests*
rm -f autom4te.cache/traces*

echo "-----------------------------------------------"
echo "End of cleanup phase - warning messages are not"
echo "expected from now on and should be examined... "
echo "-----------------------------------------------"

##########################################################################

echo "libtoolize"
libtoolize -f
echo "- aclocal -I m4"
aclocal -I m4
echo "- autoconf"
autoconf
echo "- autoheader"
autoheader
echo "- automake -a --foreign"
automake -a --foreign
echo "- ./configure"
./configure
echo "- running the python autogen.sh"
cd src/Python; ./autogen.sh; cd -

exit
