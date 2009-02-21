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

EXTRA_DIST =       \
        info.proto 

PROTO_FILES =                                       \
        $(top_srcdir)/src/include/photon/psoInfo.h  \
        $(top_srcdir)/src/Nucleus/Info.c            \
        $(top_srcdir)/src/Nucleus/Info.h            

if COND_USE_PROTOC_C
all-local: $(PROTO_FILES)

$(PROTO_FILES): info.proto
	-rm -rf output
	mkdir output
	protoc-c --c_out=output info.proto 
	cp output/info.pb-c.c ../Nucleus/Info.c
	$(AWK) -f t1.awk output/info.pb-c.h > ../Nucleus/Info.h
	$(AWK) -f t2.awk output/info.pb-c.h --proto=info.proto --guard=PSO_INFO_H > ../include/photon/psoInfo.h
endif

