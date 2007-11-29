[Setup]
MinVersion=0,4.0
AppName=VDSF
AppVerName=Virtual Data Space Framework version 0.1.0
LicenseFile=COPYING
DefaultDirName={pf}\VDSF
DefaultGroupName=VDSF
SourceDir=..\..
OutputDir=src\Packages\Output
OutputBaseFilename=vdsf-0.1.0

[Files]
Source: src\Release\vdsfAPI.dll; DestDir: {app}\bin
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
Source: src\Release\vdsfAPI.lib; DestDir: {app}\lib
Source: doc\refman.pdf; DestDir: {app}
Source: src\Release\vdswd.exe; DestDir: {app}\bin
Source: COPYING; DestDir: {app}; DestName: License.txt
Source: src\Examples\Examples.dsw; DestDir: {app}\Examples
Source: src\Examples\HashMap.c; DestDir: {app}\Examples
Source: src\Examples\HashMapLoop.c; DestDir: {app}\Examples
Source: src\Examples\iso_3166.h; DestDir: {app}\Examples
Source: src\Examples\iso_3166.tab; DestDir: {app}\Examples
Source: src\Examples\Makefile; DestDir: {app}\Examples
Source: src\Examples\Queue.h; DestDir: {app}\Examples
Source: src\Examples\QueueIn.c; DestDir: {app}\Examples
Source: src\Examples\QueueOut.c; DestDir: {app}\Examples
Source: src\Examples\QueueWork.c; DestDir: {app}\Examples
Source: src\Examples\README; DestDir: {app}\Examples; DestName: README.txt
Source: src\Examples\RunQueue.vbs; DestDir: {app}\Examples
Source: src\Examples\Project Files\HashMap.dsp; DestDir: {app}\Examples\Project Files
Source: src\Examples\Project Files\HashMapLoop.dsp; DestDir: {app}\Examples\Project Files
Source: src\Examples\Project Files\QueueIn.dsp; DestDir: {app}\Examples\Project Files
Source: src\Examples\Project Files\QueueOut.dsp; DestDir: {app}\Examples\Project Files
Source: src\Examples\Project Files\QueueWork.dsp; DestDir: {app}\Examples\Project Files

[Icons]
;Name: "{group}\Watchdog"; Filename: "{app}\bin\vdswd.EXE"; WorkingDir: "{app}"
Name: {group}\Manual; Filename: {app}\refman.pdf
Name: {group}\Uninstall My Program; Filename: {uninstallexe}

[Registry]
Root: HKCU; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\vdswd.exe; ValueType: string; ValueData: {app}\bin\vdswd.exe; Flags: uninsdeletekeyifempty uninsdeletevalue
Root: HKCU; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\vdswd.exe; ValueType: string; ValueName: Path; ValueData: {app}\bin; Flags: uninsdeletekeyifempty uninsdeletevalue

Root: HKLM; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\vdswd.exe; ValueType: string; ValueData: {app}\bin\vdswd.exe; Flags: uninsdeletekeyifempty uninsdeletevalue
Root: HKLM; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\vdswd.exe; ValueType: string; ValueName: Path; ValueData: {app}\bin; Flags: uninsdeletekeyifempty uninsdeletevalue
