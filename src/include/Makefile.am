# Copyright (C) 2006-2008 Daniel Prevost <dprevost@photonsoftware.org>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

CLEANFILES = *~ photon/*~

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

PSO_C_FILES =                       \
        man3/psoCommit.3            \
        man3/psoCommon.h.3          \
        man3/psoCreateObject.3      \
        man3/psoDestroyObject.3     \
        man3/psoErrorMsg.3          \
        man3/psoErrors.3            \
        man3/psoErrors.h.3          \
        man3/psoExit.3              \
        man3/psoExitSession.3       \
        man3/psoFastMap_c.3         \
        man3/psoFastMapClose.3      \
        man3/psoFastMapDefinition.3 \
        man3/psoFastMapDelete.3     \
        man3/psoFastMapEdit.3       \
        man3/psoFastMapGet.3        \
        man3/psoFastMapGetFirst.3   \
        man3/psoFastMapGetNext.3    \
        man3/psoFastMap.h.3         \
        man3/psoFastMapInsert.3     \
        man3/psoFastMapOpen.3       \
        man3/psoFastMapReplace.3    \
        man3/psoFastMapStatus.3     \
        man3/psoFolder_c.3          \
        man3/psoFolderClose.3       \
        man3/psoFolderEntry.3       \
        man3/psoFolderGetFirst.3    \
        man3/psoFolderGetNext.3     \
        man3/psoFolder.h.3          \
        man3/psoFolderOpen.3        \
        man3/psoFolderStatus.3      \
        man3/psoGetInfo.3           \
        man3/psoGetStatus.3         \
        man3/photon.h.3                \
        man3/psoHashMap_c.3         \
        man3/psoHashMapClose.3      \
        man3/psoHashMapDelete.3     \
        man3/psoHashMapGet.3        \
        man3/psoHashMapGetFirst.3   \
        man3/psoHashMapGetNext.3    \
        man3/psoHashMap.h.3         \
        man3/psoHashMapInsert.3     \
        man3/psoHashMapOpen.3       \
        man3/psoHashMapReplace.3    \
        man3/psoHashMapStatus.3     \
        man3/psoInfo.3              \
        man3/psoInit.3              \
        man3/psoInitSession.3       \
        man3/psoIteratorType.3      \
        man3/psoLastError.3         \
        man3/psoLifo_c.3            \
        man3/psoLifoClose.3         \
        man3/psoLifoDefinition.3    \
        man3/psoLifoGetFirst.3      \
        man3/psoLifoGetNext.3       \
        man3/psoLifo.h.3            \
        man3/psoLifoOpen.3          \
        man3/psoLifoPop.3           \
        man3/psoLifoPush.3          \
        man3/psoLifoStatus.3        \
        man3/psoObjectType.3        \
        man3/psoObjStatus.3         \
        man3/psoProcess_c.3         \
        man3/psoProcess.h.3         \
        man3/psoQueue_c.3           \
        man3/psoQueueClose.3        \
        man3/psoQueueGetFirst.3     \
        man3/psoQueueGetNext.3      \
        man3/psoQueue.h.3           \
        man3/psoQueueOpen.3         \
        man3/psoQueuePop.3          \
        man3/psoQueuePush.3         \
        man3/psoQueueStatus.3       \
        man3/psoRollback.3          \
        man3/psoSession_c.3         \
        man3/psoSession.h.3         

man_MANS =                  \
        $(PSO_C_FILES)      \
        man3/psoException.3 \
        man3/psoFolder.3    \
        man3/psoHashMap.3   \
        man3/psoProcess.3   \
        man3/psoQueue.3     \
        man3/psoSession.3   

EXTRA_DIST =                  \
        doxyfile.pso_c        \
        $(PSO_C_FILES)        \
        Doxyfile.psoException \
        Doxyfile.psoFolder    \
        Doxyfile.psoHashMap   \
        Doxyfile.psoProcess   \
        Doxyfile.psoQueue     \
        Doxyfile.psoSession   \
        man3/psoException.3   \
        man3/psoFolder.3      \
        man3/psoHashMap.3     \
        man3/psoProcess.3     \
        man3/psoQueue.3       \
        man3/psoSession.3     

nobase_include_HEADERS =        \
        photon/psoCommon.h      \
        photon/psoErrors.h      \
        photon/psoFastMap.h     \
        photon/psoFolder.h      \
        photon/psoHashMap.h     \
        photon/psoLifo.h        \
        photon/psoProcess.h     \
        photon/psoQueue.h       \
        photon/psoSession.h     \
        photon/photon.h         \
        photon/photon           \
        photon/psoException     \
        photon/psoFastMap       \
        photon/psoFastMapEditor \
        photon/psoFolder        \
        photon/psoHashMap       \
        photon/psoLifo          \
        photon/psoProcess       \
        photon/psoQueue         \
        photon/psoSession       

#
# Only rebuild the man pages if doxygen is present.
#
if COND_USE_DOXYGEN

man3/psoException.3: photon/psoException
	doxygen $(top_srcdir)/src/include/Doxyfile.psoException
	cp dummy/man/man3/psoException.3 man3/psoException.3
	-rm -rf dummy

man3/psoFolder.3: photon/psoFolder
	doxygen $(top_srcdir)/src/include/Doxyfile.psoFolder
	cp dummy/man/man3/psoFolder.3 man3/psoFolder.3
	-rm -rf dummy

man3/psoHashMap.3: photon/psoHashMap
	doxygen $(top_srcdir)/src/include/Doxyfile.psoHashMap
	cp dummy/man/man3/psoHashMap.3 man3/psoHashMap.3
	-rm -rf dummy

man3/psoProcess.3: photon/psoProcess
	doxygen Doxyfile.psoProcess
	cp dummy/man/man3/psoProcess.3 man3/psoProcess.3
	-rm -rf dummy

man3/psoQueue.3: photon/psoQueue
	doxygen Doxyfile.psoQueue
	cp dummy/man/man3/psoQueue.3 man3/psoQueue.3
	-rm -rf dummy

man3/psoSession.3: photon/psoSession
	doxygen Doxyfile.psoSession
	cp dummy/man/man3/psoSession.3 man3/psoSession.3
	-rm -rf dummy

# Warning: this doxygen run creates many more files which we do
# not want to distribute (a header file for each error code, for each
# data field in the data structures, etc.). All these files are links
# to the "meat" we are keeping anyway!
$(PSO_C_FILES): $(HEADER_FILES)
	doxygen doxyfile.pso_c
	cp dummy/man/man3/pso*.3 man3/
	cp dummy/man/man3/photon*.3 man3/
	-rm -rf dummy

endif

clean-local:
	-rm -rf dummy
   
#
# Only clean the man pages if doxygen is present.
#
if COND_USE_DOXYGEN

maintainer-clean-local:
	-rm -f man3/*.3

endif

