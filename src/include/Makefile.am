# Copyright (C) 2006-2007 Daniel Prevost <dprevost@users.sourceforge.net>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

CLEANFILES = *~ vdsf/*~

VDS_C_FILES =                           \
              man3/vds.h.3              \
              man3/vdsCommit.3          \
              man3/vdsCreateObject.3    \
              man3/vdsDestroyObject.3   \
              man3/vdsExit.3            \
              man3/vdsExitSession.3     \
              man3/vdsFolderClose.3     \
              man3/vdsFolderOpen.3      \
              man3/vdsInit.3            \
              man3/vdsInitSession.3     \
              man3/vdsQueueClose.3      \
              man3/vdsQueueGetItem.3    \
              man3/vdsQueueInsertItem.3 \
              man3/vdsQueueOpen.3       \
              man3/vdsQueueRemoveItem.3 \
              man3/vdsQueueStatus.3     \
              man3/vdsRollback.3

man_MANS =                   \
           $(VDS_C_FILES)    \
           man3/vdsFolder.3  \
           man3/vdsQueue.3   \
           man3/vdsProcess.3 \
           man3/vdsSession.3 

EXTRA_DIST =                     \
             Doxyfile.vdsFolder  \
             Doxyfile.vdsQueue   \
             Doxyfile.vdsProcess \
             Doxyfile.vdsSession \
             Doxyfile.vds_c      \
             man3/*.3


#             $(man_MANS)

nobase_include_HEADERS = \
   vdsf/vdsCommon.h      \
   vdsf/vdsErrors.h      \
   vdsf/vdsFolder.h      \
   vdsf/vdsProcess.h     \
   vdsf/vdsQueue.h       \
   vdsf/vdsSession.h     \
   vdsf/vds.h            

#   Item.h

#
# Only rebuild the man pages if doxygen is present.
#
if COND_USE_DOXYGEN

man3/vdsFolder.3: vdsf/vdsFolder
	cd 
	doxygen $(top_srcdir)/src/include/Doxyfile.vdsFolder
	cp dummy/man/man3/vdsFolder.3 man3/vdsFolder.3
	-rm -rf dummy

man3/vdsQueue.3: vdsf/vdsQueue
	doxygen Doxyfile.vdsQueue
	cp dummy/man/man3/vdsQueue.3 man3/vdsQueue.3
	-rm -rf dummy

man3/vdsProcess.3: vdsf/vdsProcess
	doxygen Doxyfile.vdsProcess
	cp dummy/man/man3/vdsProcess.3 man3/vdsProcess.3
	-rm -rf dummy

man3/vdsSession.3: vdsf/vdsSession
	doxygen Doxyfile.vdsSession
	cp dummy/man/man3/vdsSession.3 man3/vdsSession.3
	-rm -rf dummy

$(VDS_C_FILES): vdsf/vds.h vdsf/vdsFolder.h vdsf/vdsQueue.h vdsf/vdsProcess.h vdsf/vdsSession.h
	doxygen Doxyfile.vds_c
	cp dummy/man/man3/vds* man3/
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
