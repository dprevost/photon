# Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

CLEANFILES = *~ vdsf/*~

HEADER_FILES =                                      \
        $(top_srcdir)/src/include/vdsf/vds.h        \
        $(top_srcdir)/src/include/vdsf/vdsCommon.h  \
        $(top_srcdir)/src/include/vdsf/vdsFastMap.h \
        $(top_srcdir)/src/include/vdsf/vdsFolder.h  \
        $(top_srcdir)/src/include/vdsf/vdsHashMap.h \
        $(top_srcdir)/src/include/vdsf/vdsLifo.h    \
        $(top_srcdir)/src/include/vdsf/vdsQueue.h   \
        $(top_srcdir)/src/include/vdsf/vdsProcess.h \
        $(top_srcdir)/src/include/vdsf/vdsSession.h 

VDS_C_FILES =                       \
        man3/vdsCommit.3            \
        man3/vdsCommon.h.3          \
        man3/vdsCreateObject.3      \
        man3/vdsDestroyObject.3     \
        man3/vdsErrorMsg.3          \
        man3/vdsErrors.3            \
        man3/vdsErrors.h.3          \
        man3/vdsExit.3              \
        man3/vdsExitSession.3       \
        man3/vdsFastMap_c.3         \
        man3/vdsFastMapClose.3      \
        man3/vdsFastMapDefinition.3 \
        man3/vdsFastMapDelete.3     \
        man3/vdsFastMapEdit.3       \
        man3/vdsFastMapGet.3        \
        man3/vdsFastMapGetFirst.3   \
        man3/vdsFastMapGetNext.3    \
        man3/vdsFastMap.h.3         \
        man3/vdsFastMapInsert.3     \
        man3/vdsFastMapOpen.3       \
        man3/vdsFastMapReplace.3    \
        man3/vdsFastMapStatus.3     \
        man3/vdsFolder_c.3          \
        man3/vdsFolderClose.3       \
        man3/vdsFolderEntry.3       \
        man3/vdsFolderGetFirst.3    \
        man3/vdsFolderGetNext.3     \
        man3/vdsFolder.h.3          \
        man3/vdsFolderOpen.3        \
        man3/vdsFolderStatus.3      \
        man3/vdsGetInfo.3           \
        man3/vdsGetStatus.3         \
        man3/vds.h.3                \
        man3/vdsHashMap_c.3         \
        man3/vdsHashMapClose.3      \
        man3/vdsHashMapDelete.3     \
        man3/vdsHashMapGet.3        \
        man3/vdsHashMapGetFirst.3   \
        man3/vdsHashMapGetNext.3    \
        man3/vdsHashMap.h.3         \
        man3/vdsHashMapInsert.3     \
        man3/vdsHashMapOpen.3       \
        man3/vdsHashMapReplace.3    \
        man3/vdsHashMapStatus.3     \
        man3/vdsInfo.3              \
        man3/vdsInit.3              \
        man3/vdsInitSession.3       \
        man3/vdsIteratorType.3      \
        man3/vdsLastError.3         \
        man3/vdsLifo_c.3            \
        man3/vdsLifoClose.3         \
        man3/vdsLifoDefinition.3    \
        man3/vdsLifoGetFirst.3      \
        man3/vdsLifoGetNext.3       \
        man3/vdsLifo.h.3            \
        man3/vdsLifoOpen.3          \
        man3/vdsLifoPop.3           \
        man3/vdsLifoPush.3          \
        man3/vdsLifoStatus.3        \
        man3/vdsObjectType.3        \
        man3/vdsObjStatus.3         \
        man3/vdsProcess_c.3         \
        man3/vdsProcess.h.3         \
        man3/vdsQueue_c.3           \
        man3/vdsQueueClose.3        \
        man3/vdsQueueGetFirst.3     \
        man3/vdsQueueGetNext.3      \
        man3/vdsQueue.h.3           \
        man3/vdsQueueOpen.3         \
        man3/vdsQueuePop.3          \
        man3/vdsQueuePush.3         \
        man3/vdsQueueStatus.3       \
        man3/vdsRollback.3          \
        man3/vdsSession_c.3         \
        man3/vdsSession.h.3         

man_MANS =                  \
        $(VDS_C_FILES)      \
        man3/vdsException.3 \
        man3/vdsFolder.3    \
        man3/vdsHashMap.3   \
        man3/vdsProcess.3   \
        man3/vdsQueue.3     \
        man3/vdsSession.3   

EXTRA_DIST =                  \
        doxyfile.vds_c        \
        $(VDS_C_FILES)        \
        Doxyfile.vdsException \
        Doxyfile.vdsFolder    \
        Doxyfile.vdsHashMap   \
        Doxyfile.vdsProcess   \
        Doxyfile.vdsQueue     \
        Doxyfile.vdsSession   \
        man3/vdsException.3   \
        man3/vdsFolder.3      \
        man3/vdsHashMap.3     \
        man3/vdsProcess.3     \
        man3/vdsQueue.3       \
        man3/vdsSession.3     

nobase_include_HEADERS =      \
        vdsf/vdsCommon.h      \
        vdsf/vdsErrors.h      \
        vdsf/vdsFastMap.h     \
        vdsf/vdsFolder.h      \
        vdsf/vdsHashMap.h     \
        vdsf/vdsLifo.h        \
        vdsf/vdsProcess.h     \
        vdsf/vdsQueue.h       \
        vdsf/vdsSession.h     \
        vdsf/vds.h            \
        vdsf/vds              \
        vdsf/vdsException     \
        vdsf/vdsFastMap       \
        vdsf/vdsFastMapEditor \
        vdsf/vdsFolder        \
        vdsf/vdsHashMap       \
        vdsf/vdsLifo          \
        vdsf/vdsProcess       \
        vdsf/vdsQueue         \
        vdsf/vdsSession       

#
# Only rebuild the man pages if doxygen is present.
#
if COND_USE_DOXYGEN

man3/vdsException.3: vdsf/vdsException
	doxygen $(top_srcdir)/src/include/Doxyfile.vdsException
	cp dummy/man/man3/vdsException.3 man3/vdsException.3
	-rm -rf dummy

man3/vdsFolder.3: vdsf/vdsFolder
	doxygen $(top_srcdir)/src/include/Doxyfile.vdsFolder
	cp dummy/man/man3/vdsFolder.3 man3/vdsFolder.3
	-rm -rf dummy

man3/vdsHashMap.3: vdsf/vdsHashMap
	doxygen $(top_srcdir)/src/include/Doxyfile.vdsHashMap
	cp dummy/man/man3/vdsHashMap.3 man3/vdsHashMap.3
	-rm -rf dummy

man3/vdsProcess.3: vdsf/vdsProcess
	doxygen Doxyfile.vdsProcess
	cp dummy/man/man3/vdsProcess.3 man3/vdsProcess.3
	-rm -rf dummy

man3/vdsQueue.3: vdsf/vdsQueue
	doxygen Doxyfile.vdsQueue
	cp dummy/man/man3/vdsQueue.3 man3/vdsQueue.3
	-rm -rf dummy

man3/vdsSession.3: vdsf/vdsSession
	doxygen Doxyfile.vdsSession
	cp dummy/man/man3/vdsSession.3 man3/vdsSession.3
	-rm -rf dummy

# Warning: this doxygen run creates many more files which we do
# not want to distribute (a header file for each error code, for each
# data field in the data structures, etc.). All these files are links
# to the "meat" we are keeping anyway!
$(VDS_C_FILES): $(HEADER_FILES)
	doxygen doxyfile.vds_c
	cp dummy/man/man3/vds*.3 man3/
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

