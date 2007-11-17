[Files]
Source: src\Release\vdsfAPI.dll; DestDir: {sys}
Source: src\Release\vdsfCommon.dll; DestDir: {sys}
Source: src\Release\vdsfEngine.dll; DestDir: {sys}
Source: src\include\vdsf\vds.h; DestDir: {app}\include
Source: src\include\vdsf\vdsCommon.h; DestDir: {app}\include
Source: src\include\vdsf\vdsFolder.h; DestDir: {app}\include
Source: src\include\vdsf\vdsHashMap.h; DestDir: {app}\include
Source: src\include\vdsf\vdsQueue.h; DestDir: {app}\include
Source: src\Release\vdsfAPI.lib; DestDir: {app}\lib
Source: doc\refman.pdf; DestDir: {app}
[Setup]
MinVersion=0,4.0
AppName=VDSF
AppVerName=Virtual Data Space Framework version 0.1
LicenseFile=E:\vdsf\trunk\COPYING
DefaultDirName={pf}\VDSF
DefaultGroupName=VDSF
