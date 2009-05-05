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

echo "- autoconf"
autoconf
echo "- autoheader"
autoheader
echo "- ./configure"
./configure

exit
