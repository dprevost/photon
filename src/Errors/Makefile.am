# Copyright (C) 2008-2009 Daniel Prevost <dprevost@photonsoftware.org>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

# Include this at the beginning to allow the current Makefile.am to
# overwrite any macros set in the common file
include $(top_srcdir)/src/Makefile.Common.am

CLEANFILES = *~ 

EXTRA_DIST =        \
        options.xml \
        photon.xml    

OUTPUT_FILES =                                       \
        $(top_srcdir)/src/include/photon/psoErrors.h \
        $(top_srcdir)/src/Nucleus/psoErrorHandler.c  \
        $(top_srcdir)/src/Nucleus/psoErrorHandler.h  

if COND_USE_ERRORPARSER
all-local: $(OUTPUT_FILES)

$(OUTPUT_FILES): options.xml photon.xml
	errorParser --options options.xml photon.xml

endif

