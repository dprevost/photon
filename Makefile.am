# Copyright (C) 2006 Daniel Prevost <dprevost@users.sourceforge.net>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

CLEANFILES = *~

EXTRA_DIST =            \
   configure            \
   Config/config.guess  \
   Config/config.sub    \
   Config/install-sh    \
   Config/ltmain.sh     \
   Config/missing       \
   Config/mkinstalldirs

# The 'doc' subdir must be placed after 'src' since some of the 
# documentation and man pages are generated automatically when
# using "make dist" (otherwise, you may have missing files or
# outdated documentation).

SUBDIRS = m4 src doc

ACLOCAL_AMFLAGS = -I m4
