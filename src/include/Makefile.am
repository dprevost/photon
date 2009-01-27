# Copyright (C) 2006-2009 Daniel Prevost <dprevost@photonsoftware.org>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

CLEANFILES = *~ photon/*~ Doxygen/*~

C_BASE_HEADER_FILES = \
        psoCommon.h   \
        psoFastMap.h  \
        psoFolder.h   \
        psoHashMap.h  \
        psoLifo.h     \
        psoQueue.h    \
        psoProcess.h  \
        psoSession.h  

HEADER_FILES =                                        \
        $(top_srcdir)/src/include/photon/photon.h     \
        $(top_srcdir)/src/include/photon/psoCommon.h  \
        $(top_srcdir)/src/include/photon/psoFastMap.h \
        $(top_srcdir)/src/include/photon/psoFolder.h  \
        $(top_srcdir)/src/include/photon/psoHashMap.h \
        $(top_srcdir)/src/include/photon/psoLifo.h    \
        $(top_srcdir)/src/include/photon/psoQueue.h   \
        $(top_srcdir)/src/include/photon/psoProcess.h \
        $(top_srcdir)/src/include/photon/psoSession.h 

CPP_BASE_HEADER_FILES =  \
        psoDefinition    \
        psoException     \
        psoFastMap       \
        psoFastMapEditor \
        psoFolder        \
        psoHashMap       \
        psoLifo          \
        psoProcess       \
        psoQueue         \
        psoSession       

C_HEADER_FILES = $(subst pso,$(top_srcdir)/src/include/photon/pso,$(C_BASE_HEADER_FILES))
C_INSTALLED_FILES = $(subst pso,photon/pso,$(C_BASE_HEADER_FILES))

CPP_HEADER_FILES = $(subst pso,$(top_srcdir)/src/include/photon/pso,$(CPP_BASE_HEADER_FILES))
CPP_MAN_PAGES = $(addsuffix .3,$(subst pso,man3/pso,$(CPP_BASE_HEADER_FILES)))
CPP_INSTALLED_FILES = $(subst pso,photon/pso,$(CPP_BASE_HEADER_FILES))
CPP_DOXY_FILES = $(addsuffix .in,$(subst pso,$(top_srcdir)/src/include/Doxygen/pso,$(CPP_BASE_HEADER_FILES)))

# Sadly, this list must be edited by hand (it will vary if functions are
# added or removed from the C API).
C_MAN_PAGES =                           \
        man3/photon.h.3                 \
        man3/psoCommit.3                \
        man3/psoCommon.h.3              \
        man3/psoCreateObject.3          \
        man3/psoDestroyObject.3         \
        man3/psoErrorMsg.3              \
        man3/psoErrors.3                \
        man3/psoErrors.h.3              \
        man3/psoExit.3                  \
        man3/psoExitSession.3           \
        man3/psoFastMap_c.3             \
        man3/psoFastMapClose.3          \
        man3/psoFastMapDefinition.3     \
        man3/psoFastMapDelete.3         \
        man3/psoFastMapEdit.3           \
        man3/psoFastMapEmpty.3          \
        man3/psoFastMapGet.3            \
        man3/psoFastMapGetFirst.3       \
        man3/psoFastMapGetNext.3        \
        man3/psoFastMap.h.3             \
        man3/psoFastMapInsert.3         \
        man3/psoFastMapOpen.3           \
        man3/psoFastMapReplace.3        \
        man3/psoFastMapStatus.3         \
        man3/psoFieldDefinition.3       \
        man3/psoFieldType.3             \
        man3/psoFolder_c.3              \
        man3/psoFolderClose.3           \
        man3/psoFolderCreateObject.3    \
        man3/psoFolderCreateObjectXML.3 \
        man3/psoFolderDestroyObject.3   \
        man3/psoFolderEntry.3           \
        man3/psoFolderGetFirst.3        \
        man3/psoFolderGetNext.3         \
        man3/psoFolder.h.3              \
        man3/psoFolderOpen.3            \
        man3/psoFolderStatus.3          \
        man3/psoGetDefinition.3         \
        man3/psoGetInfo.3               \
        man3/psoGetStatus.3             \
        man3/psoHashMap_c.3             \
        man3/psoHashMapClose.3          \
        man3/psoHashMapDefinition.3     \
        man3/psoHashMapDelete.3         \
        man3/psoHashMapGet.3            \
        man3/psoHashMapGetFirst.3       \
        man3/psoHashMapGetNext.3        \
        man3/psoHashMap.h.3             \
        man3/psoHashMapInsert.3         \
        man3/psoHashMapOpen.3           \
        man3/psoHashMapReplace.3        \
        man3/psoHashMapStatus.3         \
        man3/psoInfo.3                  \
        man3/psoInit.3                  \
        man3/psoInitSession.3           \
        man3/psoIteratorType.3          \
        man3/psoKeyDefinition.3         \
        man3/psoKeyType.3               \
        man3/psoLastError.3             \
        man3/psoLifo_c.3                \
        man3/psoLifoClose.3             \
        man3/psoLifoDefinition.3        \
        man3/psoLifoGetFirst.3          \
        man3/psoLifoGetNext.3           \
        man3/psoLifo.h.3                \
        man3/psoLifoOpen.3              \
        man3/psoLifoPop.3               \
        man3/psoLifoPush.3              \
        man3/psoLifoStatus.3            \
        man3/psoBasicObjectDef.3      \
        man3/psoObjectType.3            \
        man3/psoObjStatus.3             \
        man3/psoProcess_c.3             \
        man3/psoProcess.h.3             \
        man3/psoQueue_c.3               \
        man3/psoQueueClose.3            \
        man3/psoQueueDefinition.3       \
        man3/psoQueueGetFirst.3         \
        man3/psoQueueGetNext.3          \
        man3/psoQueue.h.3               \
        man3/psoQueueOpen.3             \
        man3/psoQueuePop.3              \
        man3/psoQueuePush.3             \
        man3/psoQueuePushNow.3          \
        man3/psoQueueStatus.3           \
        man3/psoRollback.3              \
        man3/psoSession_c.3             \
        man3/psoSession.h.3             
        
man_MANS =               \
        $(C_MAN_PAGES)   \
        $(CPP_MAN_PAGES) 

EXTRA_DIST =              \
        Doxygen/pso_c.in  \
        $(C_MAN_PAGES)    \
        $(CPP_MAN_PAGES)  \
        $(CPP_DOXY_FILES) 

nobase_include_HEADERS =       \
        photon/photon          \
        photon/photon.h        \
        photon/psoErrors.h     \
        $(C_INSTALLED_FILES)   \
        $(CPP_INSTALLED_FILES) 

#
# Only rebuild the man pages if doxygen is present.
#
if COND_USE_DOXYGEN

$(CPP_MAN_PAGES): $(CPP_INSTALLED_FILES) photon/photon $(CPP_DOXY_FILES)
	@for f in $(CPP_BASE_HEADER_FILES); do           \
      doxygen $(top_srcdir)/src/include/Doxygen/$$f; \
      cp dummy/man/man3/$$f.3 man3/$$f.3; \
   done
	-rm -rf dummy

# Warning: this doxygen run creates many more files which we do
# not want to distribute (a header file for each error code, for each
# data field in the data structures, etc.). All these files are links
# to the "meat" we are keeping anyway!
$(C_MAN_PAGES): $(C_HEADER_FILES) $(top_srcdir)/src/include/photon/photon.h Doxygen/pso_c.in
	doxygen Doxygen/pso_c
	cp dummy/man/man3/pso*.3 man3/
	cp dummy/man/man3/photon*.3 man3/
	-rm -rf dummy

endif

clean-local:
	-rm -rf dummy
	echo $(ZZ)

#
# Only clean the man pages if doxygen is present.
#
if COND_USE_DOXYGEN

maintainer-clean-local:
	-rm -f man3/*.3

endif

