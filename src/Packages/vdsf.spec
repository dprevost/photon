Summary: Provides data components in shared memory for fast development of complex applications.
Name: vdsf
Version: 0.1.0
Release: 1
License: GPL
Group: Development/Libraries
Source: vdsf-0.1.0.tar.gz
BuildRoot: /var/tmp/%{name}-buildroot

%description
VDSF is a development tool for complex server applications. It provides 
data components like queues and hash maps shared amongst processes, 
in virtual memory. 

VDSF as a fast development toll: instead of having one complex multi-threaded 
application doing multiple tasks, create a bunch of programs each doing its 
own single task. All the programs communicate with each other (as needed) 
using the shared objects in the Virtual Data Space.

%prep
%setup

%build
./configure --prefix=$RPM_BUILD_ROOT/usr
make

%install
rm -rf $RPM_BUILD_ROOT
make install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc README COPYING doc/refman.pdf

/usr/bin/vdswd
/usr/include/vdsf/vds.h
/usr/include/vdsf/vdsCommon.h
/usr/include/vdsf/vdsErrors.h
/usr/include/vdsf/vdsFolder.h
/usr/include/vdsf/vdsHashMap.h
/usr/include/vdsf/vdsProcess.h
/usr/include/vdsf/vdsQueue.h
/usr/include/vdsf/vdsSession.h
/usr/lib/libvdsf.a
/usr/lib/libvdsf.la
/usr/lib/libvdsf.so
/usr/lib/libvdsf.so.1
/usr/lib/libvdsf.so.1.0.0
/usr/share/man/man3/vds.h.3.gz
/usr/share/man/man3/vdsCommit.3.gz
/usr/share/man/man3/vdsCommon.h.3.gz
/usr/share/man/man3/vdsCreateObject.3.gz
/usr/share/man/man3/vdsDestroyObject.3.gz
/usr/share/man/man3/vdsErrorMsg.3.gz
/usr/share/man/man3/vdsErrors.3.gz
/usr/share/man/man3/vdsErrors.h.3.gz
/usr/share/man/man3/vdsExit.3.gz
/usr/share/man/man3/vdsExitSession.3.gz
/usr/share/man/man3/vdsFolder.h.3.gz
/usr/share/man/man3/vdsFolderClose.3.gz
/usr/share/man/man3/vdsFolderEntry.3.gz
/usr/share/man/man3/vdsFolderGetFirst.3.gz
/usr/share/man/man3/vdsFolderGetNext.3.gz
/usr/share/man/man3/vdsFolderOpen.3.gz
/usr/share/man/man3/vdsFolderStatus.3.gz
/usr/share/man/man3/vdsGetInfo.3.gz
/usr/share/man/man3/vdsGetStatus.3.gz
/usr/share/man/man3/vdsHashMap.h.3.gz
/usr/share/man/man3/vdsHashMapClose.3.gz
/usr/share/man/man3/vdsHashMapDelete.3.gz
/usr/share/man/man3/vdsHashMapGet.3.gz
/usr/share/man/man3/vdsHashMapGetFirst.3.gz
/usr/share/man/man3/vdsHashMapGetNext.3.gz
/usr/share/man/man3/vdsHashMapInsert.3.gz
/usr/share/man/man3/vdsHashMapOpen.3.gz
/usr/share/man/man3/vdsHashMapStatus.3.gz
/usr/share/man/man3/vdsInfo.3.gz
/usr/share/man/man3/vdsInit.3.gz
/usr/share/man/man3/vdsInitSession.3.gz
/usr/share/man/man3/vdsIteratorType.3.gz
/usr/share/man/man3/vdsLastError.3.gz
/usr/share/man/man3/vdsObjStatus.3.gz
/usr/share/man/man3/vdsObjectType.3.gz
/usr/share/man/man3/vdsProcess.h.3.gz
/usr/share/man/man3/vdsQueue.h.3.gz
/usr/share/man/man3/vdsQueueClose.3.gz
/usr/share/man/man3/vdsQueueGetFirst.3.gz
/usr/share/man/man3/vdsQueueGetNext.3.gz
/usr/share/man/man3/vdsQueueOpen.3.gz
/usr/share/man/man3/vdsQueuePop.3.gz
/usr/share/man/man3/vdsQueuePush.3.gz
/usr/share/man/man3/vdsQueueStatus.3.gz
/usr/share/man/man3/vdsRollback.3.gz
/usr/share/man/man3/vdsSession.h.3.gz

%changelog