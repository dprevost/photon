[Setup]
MinVersion=0,4.0
AppName=VDSF
AppVerName=Virtual Data Space Framework version 0.2.0
LicenseFile=COPYING
DefaultDirName={pf}\VDSF
DefaultGroupName=VDSF
SourceDir=..\..
OutputDir=src\Packages\Output
OutputBaseFilename=vdsf-0.2.0

[Files]
Source: src\Release\vdsf.dll; DestDir: {app}\bin
Source: src\Release\vdsfCommon.dll; DestDir: {app}\bin
Source: src\Release\vdsfEngine.dll; DestDir: {app}\bin
Source: src\include\vdsf\vds.h; DestDir: {app}\include
Source: src\include\vdsf\vdsCommon.h; DestDir: {app}\include
Source: src\include\vdsf\vdsErrors.h; DestDir: {app}\include
Source: src\include\vdsf\vdsFolder.h; DestDir: {app}\include
Source: src\include\vdsf\vdsHashMap.h; DestDir: {app}\include
Source: src\include\vdsf\vdsProcess.h; DestDir: {app}\include
Source: src\include\vdsf\vdsQueue.h; DestDir: {app}\include
Source: src\include\vdsf\vdsSession.h; DestDir: {app}\include
Source: src\Release\vdsf.lib; DestDir: {app}\lib
Source: doc\refman.pdf; DestDir: {app}
Source: src\Release\vdswd.exe; DestDir: {app}\bin
Source: COPYING; DestDir: {app}; DestName: License.txt
Source: src\Examples\iso_3166.tab; DestDir: {app}\Examples
Source: src\Examples\C\Examples.dsw; DestDir: {app}\Examples\C
Source: src\Examples\C\HashMap.c; DestDir: {app}\Examples\C
Source: src\Examples\C\HashMapLoop.c; DestDir: {app}\Examples\C
Source: src\Examples\C\iso_3166.h; DestDir: {app}\Examples\C
Source: src\Examples\C\Makefile; DestDir: {app}\Examples\C
Source: src\Examples\C\Queue.h; DestDir: {app}\Examples\C
Source: src\Examples\C\QueueIn.c; DestDir: {app}\Examples\C
Source: src\Examples\C\QueueOut.c; DestDir: {app}\Examples\C
Source: src\Examples\C\QueueWork.c; DestDir: {app}\Examples\C
Source: src\Examples\C\README; DestDir: {app}\Examples\C; DestName: README.txt
Source: src\Examples\C\RunQueue.vbs; DestDir: {app}\Examples\C
Source: src\Examples\C\Project Files\HashMap.dsp; DestDir: {app}\Examples\C\Project Files
Source: src\Examples\C\Project Files\HashMapLoop.dsp; DestDir: {app}\Examples\C\Project Files
Source: src\Examples\C\Project Files\QueueIn.dsp; DestDir: {app}\Examples\C\Project Files
Source: src\Examples\C\Project Files\QueueOut.dsp; DestDir: {app}\Examples\C\Project Files
Source: src\Examples\C\Project Files\QueueWork.dsp; DestDir: {app}\Examples\C\Project Files
Source: src\Examples\C++\Examples.dsw; DestDir: {app}\Examples\C++
Source: src\Examples\C++\HashMap.cpp; DestDir: {app}\Examples\C++
Source: src\Examples\C++\HashMapLoop.cpp; DestDir: {app}\Examples\C++
Source: src\Examples\C++\iso_3166.h; DestDir: {app}\Examples\C++
Source: src\Examples\C++\Makefile; DestDir: {app}\Examples\C++
Source: src\Examples\C++\Queue.h; DestDir: {app}\Examples\C++
Source: src\Examples\C++\QueueIn.cpp; DestDir: {app}\Examples\C++
Source: src\Examples\C++\QueueOut.cpp; DestDir: {app}\Examples\C++
Source: src\Examples\C++\QueueWork.cpp; DestDir: {app}\Examples\C++
Source: src\Examples\C++\README.txt; DestDir: {app}\Examples\C++
Source: src\Examples\C++\RunQueue.vbs; DestDir: {app}\Examples\C++
Source: src\Examples\C++\Project Files\HashMap.dsp; DestDir: {app}\Examples\C++\Project Files
Source: src\Examples\C++\Project Files\HashMapLoop.dsp; DestDir: {app}\Examples\C++\Project Files
Source: src\Examples\C++\Project Files\QueueIn.dsp; DestDir: {app}\Examples\C++\Project Files
Source: src\Examples\C++\Project Files\QueueOut.dsp; DestDir: {app}\Examples\C++\Project Files
Source: src\Examples\C++\Project Files\QueueWork.dsp; DestDir: {app}\Examples\C++\Project Files
;Source: c:\windows\system32\mswinsck.ocx; DestDir: {sys}; Flags: regserver restartreplace sharedfile
Source: src\Release\vdsfcpp.lib; DestDir: {app}\lib
Source: src\Release\vdsfcpp.dll; DestDir: {app}\bin
Source: src\Release\vdsInfo.exe; DestDir: {sys}\bin
Source: src\Release\vdssh.exe; DestDir: {app}\bin

[Icons]
;Name: "{group}\Watchdog"; Filename: "{app}\bin\vdswd.EXE"; WorkingDir: "{app}"
Name: {group}\Manual; Filename: {app}\refman.pdf
Name: {group}\Uninstall My Program; Filename: {uninstallexe}

[Registry]
Root: HKCU; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\vdswd.exe; ValueType: string; ValueData: {app}\bin\vdswd.exe; Flags: uninsdeletekeyifempty uninsdeletevalue
Root: HKCU; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\vdswd.exe; ValueType: string; ValueName: Path; ValueData: {app}\bin; Flags: uninsdeletekeyifempty uninsdeletevalue

Root: HKLM; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\vdswd.exe; ValueType: string; ValueData: {app}\bin\vdswd.exe; Flags: uninsdeletekeyifempty uninsdeletevalue
Root: HKLM; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\vdswd.exe; ValueType: string; ValueName: Path; ValueData: {app}\bin; Flags: uninsdeletekeyifempty uninsdeletevalue

Root: HKCU; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\vdssh.exe; ValueType: string; ValueData: {app}\bin\vdssh.exe; Flags: uninsdeletekeyifempty uninsdeletevalue
Root: HKCU; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\vdssh.exe; ValueType: string; ValueName: Path; ValueData: {app}\bin; Flags: uninsdeletekeyifempty uninsdeletevalue

Root: HKLM; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\vdssh.exe; ValueType: string; ValueData: {app}\bin\vdssh.exe; Flags: uninsdeletekeyifempty uninsdeletevalue
Root: HKLM; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\vdssh.exe; ValueType: string; ValueName: Path; ValueData: {app}\bin; Flags: uninsdeletekeyifempty uninsdeletevalue

Root: HKCU; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\vdsinfo.exe; ValueType: string; ValueData: {app}\bin\vdsinfo.exe; Flags: uninsdeletekeyifempty uninsdeletevalue
Root: HKCU; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\vdsinfo.exe; ValueType: string; ValueName: Path; ValueData: {app}\bin; Flags: uninsdeletekeyifempty uninsdeletevalue

Root: HKLM; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\vdsinfo.exe; ValueType: string; ValueData: {app}\bin\vdsinfo.exe; Flags: uninsdeletekeyifempty uninsdeletevalue
Root: HKLM; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\vdsinfo.exe; ValueType: string; ValueName: Path; ValueData: {app}\bin; Flags: uninsdeletekeyifempty uninsdeletevalue
