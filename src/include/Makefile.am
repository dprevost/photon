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

C_HEADER_FILES =                                          \
        $(top_srcdir)/src/include/photon/photon.h         \
        $(top_srcdir)/src/include/photon/psoCommon.h      \
        $(top_srcdir)/src/include/photon/DataDefinition.h \
        $(top_srcdir)/src/include/photon/FastMap.h        \
        $(top_srcdir)/src/include/photon/Folder.h         \
        $(top_srcdir)/src/include/photon/HashMap.h        \
        $(top_srcdir)/src/include/photon/KeyDefinition.h  \
        $(top_srcdir)/src/include/photon/Lifo.h           \
        $(top_srcdir)/src/include/photon/Process.h        \
        $(top_srcdir)/src/include/photon/psoPhotonODBC.h  \
        $(top_srcdir)/src/include/photon/Queue.h          \
        $(top_srcdir)/src/include/photon/Session.h        

C_INSTALLED_FILES =             \
        photon/photon.h         \
        photon/psoCommon.h      \
        photon/psoErrors.h      \
        photon/DataDefinition.h \
        photon/FastMap.h        \
        photon/Folder.h         \
        photon/HashMap.h        \
        photon/KeyDefinition.h  \
        photon/Lifo.h           \
        photon/psoPhotonODBC.h  \
        photon/Process.h        \
        photon/Queue.h          \
        photon/Session.h        

CPP_INSTALLED_FILES =             \
        photon/photon             \
        photon/DataDefBuilderODBC \
        photon/DataDefBuilderUser \
        photon/DataDefinition     \
        photon/KeyDefBuilderODBC  \
        photon/KeyDefBuilderUser  \
        photon/KeyDefinition      \
        photon/Exception          \
        photon/FastMap            \
        photon/FastMapEditor      \
        photon/Folder             \
        photon/HashMap            \
        photon/Lifo               \
        photon/Process            \
        photon/Queue              \
        photon/Session            

CPP_DOXY_FILES =                                                \
        $(top_srcdir)/src/include/Doxygen/DataDefBuilderODBC.in \
        $(top_srcdir)/src/include/Doxygen/DataDefBuilderUser.in \
        $(top_srcdir)/src/include/Doxygen/DataDefinition.in     \
        $(top_srcdir)/src/include/Doxygen/KeyDefBuilderODBC.in  \
        $(top_srcdir)/src/include/Doxygen/KeyDefBuilderUser.in  \
        $(top_srcdir)/src/include/Doxygen/KeyDefinition.in      \
        $(top_srcdir)/src/include/Doxygen/psoException.in       \
        $(top_srcdir)/src/include/Doxygen/psoFastMap.in         \
        $(top_srcdir)/src/include/Doxygen/psoFastMapEditor.in   \
        $(top_srcdir)/src/include/Doxygen/psoFolder.in          \
        $(top_srcdir)/src/include/Doxygen/psoHashMap.in         \
        $(top_srcdir)/src/include/Doxygen/psoLifo.in            \
        $(top_srcdir)/src/include/Doxygen/psoProcess.in         \
        $(top_srcdir)/src/include/Doxygen/psoQueue.in           \
        $(top_srcdir)/src/include/Doxygen/psoSession.in         

CPP_MAN_PAGES =                      \
        man3/psoDataDefBuilderODBC.3 \
        man3/psoDataDefBuilderUser.3 \
        man3/psoDataDefinition.3     \
        man3/psoKeyDefBuilderODBC.3  \
        man3/psoKeyDefBuilderUser.3  \
        man3/psoKeyDefinition.3      \
        man3/psoException.3          \
        man3/psoFastMap.3            \
        man3/psoFastMapEditor.3      \
        man3/psoFolder.3             \
        man3/psoHashMap.3            \
        man3/psoLifo.3               \
        man3/psoProcess.3            \
        man3/psoQueue.3              \
        man3/psoSession.3            

# Sadly, this list must be edited by hand (it will vary if functions are
# added or removed from the C API).
C_MAN_PAGES = \
        man3/photon.h.3 \
        man3/psoCommit.3 \
        man3/psoCommon.h.3 \
        man3/psoCreateFolder.3 \
        man3/psoCreateMap.3 \
        man3/psoCreateQueue.3 \
        man3/psoDataDefClose.3 \
        man3/psoDataDefCreate.3 \
        man3/psoDataDefGet.3 \
        man3/psoDataDefGetLength.3 \
        man3/psoDataDefinition_c.3 \
        man3/psoDataDefOpen.3 \
        man3/psoDateStruct.3 \
        man3/psoDefinitionType.3 \
        man3/psoDestroyObject.3 \
        man3/psoErrorMsg.3 \
        man3/psoErrors.3 \
        man3/psoErrors.h.3 \
        man3/psoExit.3 \
        man3/psoExitSession.3 \
        man3/psoFastMap_c.3 \
        man3/psoFastMapClose.3 \
        man3/psoFastMapDefinition.3 \
        man3/psoFastMapDelete.3 \
        man3/psoFastMapEdit.3 \
        man3/psoFastMapEmpty.3 \
        man3/psoFastMapGet.3 \
        man3/psoFastMapGetFirst.3 \
        man3/psoFastMapGetNext.3 \
        man3/psoFastMapInsert.3 \
        man3/psoFastMapOpen.3 \
        man3/psoFastMapRecordDefinition.3 \
        man3/psoFastMapReplace.3 \
        man3/psoFastMapStatus.3 \
        man3/psoFieldDefinition.3 \
        man3/psoFieldType.3 \
        man3/psoFolder_c.3 \
        man3/psoFolderClose.3 \
        man3/psoFolderCreateFolder.3 \
        man3/psoFolderCreateMap.3 \
        man3/psoFolderCreateQueue.3 \
        man3/psoFolderDestroyObject.3 \
        man3/psoFolderEntry.3 \
        man3/psoFolderGetDataDefinition.3 \
        man3/psoFolderGetDefinition.3 \
        man3/psoFolderGetFirst.3 \
        man3/psoFolderGetKeyDefinition.3 \
        man3/psoFolderGetNext.3 \
        man3/psoFolderOpen.3 \
        man3/psoFolderStatus.3 \
        man3/psoGetDataDefinition.3 \
        man3/psoGetDefinition.3 \
        man3/psoGetInfo.3 \
        man3/psoGetKeyDefinition.3 \
        man3/psoGetStatus.3 \
        man3/psoHashMap_c.3 \
        man3/psoHashMapClose.3 \
        man3/psoHashMapDefinition.3 \
        man3/psoHashMapDelete.3 \
        man3/psoHashMapGet.3 \
        man3/psoHashMapGetFirst.3 \
        man3/psoHashMapGetNext.3 \
        man3/psoHashMapInsert.3 \
        man3/psoHashMapOpen.3 \
        man3/psoHashMapRecordDefinition.3 \
        man3/psoHashMapReplace.3 \
        man3/psoHashMapStatus.3 \
        man3/psoInfo.3 \
        man3/psoInit.3 \
        man3/psoInitSession.3 \
        man3/psoInt16.3 \
        man3/psoIteratorType.3 \
        man3/psoKeyDefClose.3 \
        man3/psoKeyDefCreate.3 \
        man3/psoKeyDefGet.3 \
        man3/psoKeyDefGetLength.3 \
        man3/psoKeyDefOpen.3 \
        man3/psoKeyFieldDefinition.3 \
        man3/psoKeyFieldDefinition_c.3 \
        man3/psoKeyType.3 \
        man3/psoLastError.3 \
        man3/psoLifo_c.3 \
        man3/psoLifoClose.3 \
        man3/psoLifoDefinition.3 \
        man3/psoLifoGetFirst.3 \
        man3/psoLifoGetNext.3 \
        man3/psoLifoOpen.3 \
        man3/psoLifoPop.3 \
        man3/psoLifoPush.3 \
        man3/psoLifoRecordDefinition.3 \
        man3/psoLifoStatus.3 \
        man3/psoNumericStruct.3 \
        man3/psoObjectDefinition.3 \
        man3/psoObjectType.3 \
        man3/psoObjStatus.3 \
        man3/psoPhotonODBC.h.3 \
        man3/psoProcess_c.3 \
        man3/psoQueue_c.3 \
        man3/psoQueueClose.3 \
        man3/psoQueueDefinition.3 \
        man3/psoQueueGetFirst.3 \
        man3/psoQueueGetNext.3 \
        man3/psoQueueOpen.3 \
        man3/psoQueuePop.3 \
        man3/psoQueuePush.3 \
        man3/psoQueuePushNow.3 \
        man3/psoQueueRecordDefinition.3 \
        man3/psoQueueStatus.3 \
        man3/psoRollback.3 \
        man3/psoSession_c.3 \
        man3/psoTimeStampStruct.3 \
        man3/psoTimeStruct.3 \
        man3/psoUint16.3 \
        man3/psoUint32.3 

man_MANS =               \
        $(C_MAN_PAGES)   \
        $(CPP_MAN_PAGES) 

EXTRA_DIST =              \
        Doxygen/pso_c.in  \
        $(C_MAN_PAGES)    \
        $(CPP_MAN_PAGES)  \
        $(CPP_DOXY_FILES) 

nobase_include_HEADERS =       \
        $(C_INSTALLED_FILES)   \
        $(CPP_INSTALLED_FILES) 

#
# Only rebuild the man pages if doxygen is present.
#
if COND_USE_DOXYGEN

man3/psoDataDefBuilderODBC.3: $(top_srcdir)/src/include/Doxygen/DataDefBuilderODBC.in $(top_srcdir)/src/include/photon/DataDefBuilderODBC
	doxygen $(top_srcdir)/src/include/Doxygen/DataDefBuilderODBC 
	cp dummy/man/man3/pso_DataDefBuilderODBC.3 man3/psoDataDefBuilderODBC.3
	-rm -rf dummy
man3/psoDataDefBuilderUser.3: $(top_srcdir)/src/include/Doxygen/DataDefBuilderUser.in $(top_srcdir)/src/include/photon/DataDefBuilderUser
	doxygen $(top_srcdir)/src/include/Doxygen/DataDefBuilderUser 
	cp dummy/man/man3/pso_DataDefBuilderUser.3 man3/psoDataDefBuilderUser.3
	-rm -rf dummy
man3/psoDataDefinition.3: $(top_srcdir)/src/include/Doxygen/DataDefinition.in $(top_srcdir)/src/include/photon/DataDefinition
	doxygen $(top_srcdir)/src/include/Doxygen/DataDefinition 
	cp dummy/man/man3/pso_DataDefinition.3 man3/psoDataDefinition.3
	-rm -rf dummy
man3/psoKeyDefBuilderODBC.3: $(top_srcdir)/src/include/Doxygen/KeyDefBuilderODBC.in $(top_srcdir)/src/include/photon/KeyDefBuilderODBC
	doxygen $(top_srcdir)/src/include/Doxygen/KeyDefBuilderODBC 
	cp dummy/man/man3/pso_KeyDefBuilderODBC.3 man3/psoKeyDefBuilderODBC.3
	-rm -rf dummy
man3/psoKeyDefBuilderUser.3: $(top_srcdir)/src/include/Doxygen/KeyDefBuilderUser.in $(top_srcdir)/src/include/photon/KeyDefBuilderUser
	doxygen $(top_srcdir)/src/include/Doxygen/KeyDefBuilderUser 
	cp dummy/man/man3/pso_KeyDefBuilderUser.3 man3/psoKeyDefBuilderUser.3
	-rm -rf dummy
man3/psoKeyDefinition.3: $(top_srcdir)/src/include/Doxygen/KeyDefinition.in $(top_srcdir)/src/include/photon/KeyDefinition
	doxygen $(top_srcdir)/src/include/Doxygen/KeyDefinition 
	cp dummy/man/man3/pso_KeyDefinition.3 man3/psoKeyDefinition.3
	-rm -rf dummy
man3/psoException.3: $(top_srcdir)/src/include/Doxygen/psoException.in $(top_srcdir)/src/include/photon/Exception
	doxygen $(top_srcdir)/src/include/Doxygen/psoException 
	cp dummy/man/man3/pso_Exception.3 man3/psoException.3
	-rm -rf dummy
man3/psoFastMap.3: $(top_srcdir)/src/include/Doxygen/psoFastMap.in $(top_srcdir)/src/include/photon/FastMap
	doxygen $(top_srcdir)/src/include/Doxygen/psoFastMap 
	cp dummy/man/man3/pso_FastMap.3 man3/psoFastMap.3
	-rm -rf dummy
man3/psoFastMapEditor.3: $(top_srcdir)/src/include/Doxygen/psoFastMapEditor.in $(top_srcdir)/src/include/photon/FastMapEditor
	doxygen $(top_srcdir)/src/include/Doxygen/psoFastMapEditor 
	cp dummy/man/man3/pso_FastMapEditor.3 man3/psoFastMapEditor.3
	-rm -rf dummy
man3/psoFolder.3: $(top_srcdir)/src/include/Doxygen/psoFolder.in $(top_srcdir)/src/include/photon/Folder
	doxygen $(top_srcdir)/src/include/Doxygen/psoFolder 
	cp dummy/man/man3/pso_Folder.3 man3/psoFolder.3
	-rm -rf dummy
man3/psoHashMap.3: $(top_srcdir)/src/include/Doxygen/psoHashMap.in $(top_srcdir)/src/include/photon/HashMap
	doxygen $(top_srcdir)/src/include/Doxygen/psoHashMap 
	cp dummy/man/man3/pso_HashMap.3 man3/psoHashMap.3
	-rm -rf dummy
man3/psoLifo.3: $(top_srcdir)/src/include/Doxygen/psoLifo.in $(top_srcdir)/src/include/photon/Lifo
	doxygen $(top_srcdir)/src/include/Doxygen/psoLifo 
	cp dummy/man/man3/pso_Lifo.3 man3/psoLifo.3
	-rm -rf dummy
man3/psoProcess.3: $(top_srcdir)/src/include/Doxygen/psoProcess.in $(top_srcdir)/src/include/photon/Process
	doxygen $(top_srcdir)/src/include/Doxygen/psoProcess 
	cp dummy/man/man3/pso_Process.3 man3/psoProcess.3
	-rm -rf dummy
man3/psoQueue.3: $(top_srcdir)/src/include/Doxygen/psoQueue.in $(top_srcdir)/src/include/photon/Queue
	doxygen $(top_srcdir)/src/include/Doxygen/psoQueue 
	cp dummy/man/man3/pso_Queue.3 man3/psoQueue.3
	-rm -rf dummy
man3/psoSession.3: $(top_srcdir)/src/include/Doxygen/psoSession.in $(top_srcdir)/src/include/photon/Session
	doxygen $(top_srcdir)/src/include/Doxygen/psoSession 
	cp dummy/man/man3/pso_Session.3 man3/psoSession.3
	-rm -rf dummy

# Warning: this doxygen run creates many more files which we do
# not want to distribute (a header file for each error code, for each
# data field in the data structures, etc.). All these files are links
# to the "meat" we are keeping anyway!
$(C_MAN_PAGES): $(C_HEADER_FILES) Doxygen/pso_c.in
	doxygen Doxygen/pso_c
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

