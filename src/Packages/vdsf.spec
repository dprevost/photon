Summary: Provides data containers in shared memory for fast development of complex applications.
Name: vdsf
Version: 0.1.0
Release: 1
License: GPL
Group: Development/Libraries
Source: vdsf-0.1.0.tar.gz
BuildRoot: /var/tmp/%{name}-buildroot
Prefix: /usr/local

%description
VDSF is a development tool for complex server applications. It provides 
data containers like queues and hash maps shared amongst processes, 
in virtual memory. 

VDSF as a fast development tool: instead of having one complex multi-threaded 
application doing multiple tasks, create a bunch of programs each doing its 
own single task. All the programs communicate with each other (as needed) 
using the shared objects in the Virtual Data Space.

%prep
%setup

%build
./configure --prefix=$RPM_BUILD_ROOT/usr/local
make

%install
rm -rf $RPM_BUILD_ROOT
make install
mkdir -p $RPM_BUILD_ROOT/usr/local/share/doc/vdsf
install -m 644 README $RPM_BUILD_ROOT/usr/local/share/doc/vdsf/README
install -m 644 COPYING $RPM_BUILD_ROOT/usr/local/share/doc/vdsf/COPYING
install -m 644 doc/refman.pdf $RPM_BUILD_ROOT/usr/local/share/doc/vdsf/refman.pdf

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%docdir /usr/local/share/doc/vdsf
/usr/local/share/doc/vdsf
/usr/local/share/doc/vdsf/README 
/usr/local/share/doc/vdsf/COPYING 
/usr/local/share/doc/vdsf/refman.pdf

/usr/local/bin/vdswd
/usr/local/include/vdsf/vds.h
/usr/local/include/vdsf/vdsCommon.h
/usr/local/include/vdsf/vdsErrors.h
/usr/local/include/vdsf/vdsFolder.h
/usr/local/include/vdsf/vdsHashMap.h
/usr/local/include/vdsf/vdsProcess.h
/usr/local/include/vdsf/vdsQueue.h
/usr/local/include/vdsf/vdsSession.h
/usr/local/lib/libvdsf.a
/usr/local/lib/libvdsf.la
/usr/local/lib/libvdsf.so
/usr/local/lib/libvdsf.so.1
/usr/local/lib/libvdsf.so.1.0.0
/usr/local/share/man/man3/vds.h.3
/usr/local/share/man/man3/vdsCommit.3
/usr/local/share/man/man3/vdsCommon.h.3
/usr/local/share/man/man3/vdsCreateObject.3
/usr/local/share/man/man3/vdsDestroyObject.3
/usr/local/share/man/man3/vdsErrorMsg.3
/usr/local/share/man/man3/vdsErrors.3
/usr/local/share/man/man3/vdsErrors.h.3
/usr/local/share/man/man3/vdsExit.3
/usr/local/share/man/man3/vdsExitSession.3
/usr/local/share/man/man3/vdsFolder.h.3
/usr/local/share/man/man3/vdsFolderClose.3
/usr/local/share/man/man3/vdsFolderEntry.3
/usr/local/share/man/man3/vdsFolderGetFirst.3
/usr/local/share/man/man3/vdsFolderGetNext.3
/usr/local/share/man/man3/vdsFolderOpen.3
/usr/local/share/man/man3/vdsFolderStatus.3
/usr/local/share/man/man3/vdsGetInfo.3
/usr/local/share/man/man3/vdsGetStatus.3
/usr/local/share/man/man3/vdsHashMap.h.3
/usr/local/share/man/man3/vdsHashMapClose.3
/usr/local/share/man/man3/vdsHashMapDelete.3
/usr/local/share/man/man3/vdsHashMapGet.3
/usr/local/share/man/man3/vdsHashMapGetFirst.3
/usr/local/share/man/man3/vdsHashMapGetNext.3
/usr/local/share/man/man3/vdsHashMapInsert.3
/usr/local/share/man/man3/vdsHashMapOpen.3
/usr/local/share/man/man3/vdsHashMapStatus.3
/usr/local/share/man/man3/vdsInfo.3
/usr/local/share/man/man3/vdsInit.3
/usr/local/share/man/man3/vdsInitSession.3
/usr/local/share/man/man3/vdsIteratorType.3
/usr/local/share/man/man3/vdsLastError.3
/usr/local/share/man/man3/vdsObjStatus.3
/usr/local/share/man/man3/vdsObjectType.3
/usr/local/share/man/man3/vdsProcess.h.3
/usr/local/share/man/man3/vdsQueue.h.3
/usr/local/share/man/man3/vdsQueueClose.3
/usr/local/share/man/man3/vdsQueueGetFirst.3
/usr/local/share/man/man3/vdsQueueGetNext.3
/usr/local/share/man/man3/vdsQueueOpen.3
/usr/local/share/man/man3/vdsQueuePop.3
/usr/local/share/man/man3/vdsQueuePush.3
/usr/local/share/man/man3/vdsQueueStatus.3
/usr/local/share/man/man3/vdsRollback.3
/usr/local/share/man/man3/vdsSession.h.3

%changelog