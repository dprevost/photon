[Setup]
MinVersion=0,4.0
AppName=VDSF
AppVerName=Virtual Data Space Framework version 0.1
LicenseFile=COPYING
DefaultDirName={pf}\VDSF
DefaultGroupName=VDSF
SourceDir=..
OutputDir=Installation\Output
OutputBaseFilename=vdsf-0.1

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

[Icons]
Name: "{group}\Watchdog"; Filename: "{app}\bin\vdswd.EXE"; WorkingDir: "{app}"
Name: "{group}\Manual"; Filename: "{app}\refman.pdf"
Name: "{group}\Uninstall My Program"; Filename: "{uninstallexe}"

[Registry]
Root: HKCU; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\vdswd.exe; ValueType: string; ValueData: {app}\bin\vdswd.exe; Flags: uninsdeletekeyifempty uninsdeletevalue
Root: HKCU; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\vdswd.exe; ValueType: string; ValueName: Path; ValueData: {app}\bin; Flags: uninsdeletekeyifempty uninsdeletevalue
;Root: HKCU; SubKey: SOFTWARE\Tigris.org\Subversion; ValueType: string; ValueName: Version; ValueData: {#= svn_version}; Flags: uninsdeletekeyifempty uninsdeletevalue
;Root: HKCU; SubKey: SOFTWARE\Tigris.org\Subversion; ValueType: string; ValueName: Revision; ValueData: {#= svn_revision}; Flags: uninsdeletekeyifempty uninsdeletevalue
;Root: HKCU; Subkey: Environment; ValueType: string; ValueName: APR_ICONV_PATH; ValueData: {app}\iconv; Flags: uninsdeletevalue noerror

Root: HKLM; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\vdswd.exe; ValueType: string; ValueData: {app}\bin\vdswd.exe; Flags: uninsdeletekeyifempty uninsdeletevalue
Root: HKLM; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\vdswd.exe; ValueType: string; ValueName: Path; ValueData: {app}\bin; Flags: uninsdeletekeyifempty uninsdeletevalue
;Root: HKLM; SubKey: SOFTWARE\Tigris.org\Subversion; ValueType: string; ValueName: Version; ValueData: {#= svn_version}; Flags: noerror uninsdeletekey
;Root: HKLM; SubKey: SOFTWARE\Tigris.org\Subversion; ValueType: string; ValueName: Revision; ValueData: {#= svn_revision}; Flags: uninsdeletevalue noerror uninsdeletekeyifempty
;Root: HKLM; Subkey: SYSTEM\CurrentControlSet\Control\Session Manager\Environment; ValueType: string; ValueName: APR_ICONV_PATH; ValueData: {app}\iconv; Flags: uninsdeletevalue noerror

