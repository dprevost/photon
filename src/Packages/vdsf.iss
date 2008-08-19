[Setup]
MinVersion=0,4.0
AppName=VDSF
AppVerName=Virtual Data Space Framework version 0.3.0
AppVersion=0.3.0
AppSupportURL=http://vdsf.sourceforge.net/
AppId=VDSF_V0
LicenseFile=COPYING
DefaultDirName={pf}\VDSF
DefaultGroupName=VDSF
SourceDir=..\..
OutputDir=src\Packages\Output
OutputBaseFilename=vdsf-0.3.0

[Files]
Source: src\Release\vdsf.dll; DestDir: {app}\bin
Source: src\Release\vdsfCommon.dll; DestDir: {app}\bin
Source: src\Release\vdsfcpp.dll; DestDir: {app}\bin
Source: src\Release\vdsfNucleus.dll; DestDir: {app}\bin
Source: src\Release\vdsInfo.exe; DestDir: {app}\bin
Source: src\Release\vdssh.exe; DestDir: {app}\bin
Source: src\Release\vdswd.exe; DestDir: {app}\bin
Source: src\include\vdsf\vds.h; DestDir: {app}\include
Source: src\include\vdsf\vdsCommon.h; DestDir: {app}\include
Source: src\include\vdsf\vdsErrors.h; DestDir: {app}\include
Source: src\include\vdsf\vdsFolder.h; DestDir: {app}\include
Source: src\include\vdsf\vdsHashMap.h; DestDir: {app}\include
Source: src\include\vdsf\vdsProcess.h; DestDir: {app}\include
Source: src\include\vdsf\vdsQueue.h; DestDir: {app}\include
Source: src\include\vdsf\vdsSession.h; DestDir: {app}\include
Source: src\include\vdsf\vds; DestDir: {app}\include
Source: src\include\vdsf\vdsException; DestDir: {app}\include
Source: src\include\vdsf\vdsFolder; DestDir: {app}\include
Source: src\include\vdsf\vdsHashMap; DestDir: {app}\include
Source: src\include\vdsf\vdsProcess; DestDir: {app}\include
Source: src\include\vdsf\vdsQueue; DestDir: {app}\include
Source: src\include\vdsf\vdsSession; DestDir: {app}\include
Source: src\Release\vdsf.lib; DestDir: {app}\lib
Source: src\Release\vdsfcpp.lib; DestDir: {app}\lib
Source: doc\refman_c.pdf; DestDir: {app}
Source: doc\refman_c++.pdf; DestDir: {app}
Source: COPYING; DestDir: {app}; DestName: License.txt
Source: src\Examples\iso_3166.tab; DestDir: {app}\Examples
Source: src\Examples\vdswd-config.example; DestDir: {app}\Examples
Source: src\Examples\README.txt; DestDir: {app}\Examples
Source: src\Examples\C\Examples.dsw; DestDir: {app}\Examples\C
Source: src\Examples\C\HashMap.c; DestDir: {app}\Examples\C
Source: src\Examples\C\HashMapLoop.c; DestDir: {app}\Examples\C
Source: src\Examples\C\iso_3166.h; DestDir: {app}\Examples\C
Source: src\Examples\C\Makefile; DestDir: {app}\Examples\C
Source: src\Examples\C\Queue.h; DestDir: {app}\Examples\C
Source: src\Examples\C\QueueIn.c; DestDir: {app}\Examples\C
Source: src\Examples\C\QueueOut.c; DestDir: {app}\Examples\C
Source: src\Examples\C\QueueWork.c; DestDir: {app}\Examples\C
Source: src\Examples\C\README.txt; DestDir: {app}\Examples\C
Source: src\Examples\C\RunQueue.vbs; DestDir: {app}\Examples\C
Source: src\Examples\C\Project_Files\HashMap.dsp; DestDir: {app}\Examples\C\Project_Files
Source: src\Examples\C\Project_Files\HashMapLoop.dsp; DestDir: {app}\Examples\C\Project_Files
Source: src\Examples\C\Project_Files\QueueIn.dsp; DestDir: {app}\Examples\C\Project_Files
Source: src\Examples\C\Project_Files\QueueOut.dsp; DestDir: {app}\Examples\C\Project_Files
Source: src\Examples\C\Project_Files\QueueWork.dsp; DestDir: {app}\Examples\C\Project_Files
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
Source: src\Examples\C++\Project_Files\HashMap.dsp; DestDir: {app}\Examples\C++\Project_Files
Source: src\Examples\C++\Project_Files\HashMapLoop.dsp; DestDir: {app}\Examples\C++\Project_Files
Source: src\Examples\C++\Project_Files\QueueIn.dsp; DestDir: {app}\Examples\C++\Project_Files
Source: src\Examples\C++\Project_Files\QueueOut.dsp; DestDir: {app}\Examples\C++\Project_Files
Source: src\Examples\C++\Project_Files\QueueWork.dsp; DestDir: {app}\Examples\C++\Project_Files
;Source: c:\windows\system32\mswinsck.ocx; DestDir: {sys}; Flags: regserver restartreplace sharedfile

[Icons]
;Name: "{group}\Watchdog"; Filename: "{app}\bin\vdswd.EXE"; WorkingDir: "{app}"
Name: {group}\Ref. Manual C API; Filename: {app}\refman_c.pdf
Name: {group}\Ref. Manual C++ API; Filename: {app}\refman_c++.pdf
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
[Code]

function InitializeSetup(): Boolean;

var
  installedVersion: String;
  removeOld: Boolean;
  strUninstall: String;
  errcode: Integer;

begin
  removeOld := False;
  Result := True;
  // Check for version 0.1 - we remove it if we see it without
  // asking (so few were downloaded - no point in being too careful...)
  if RegKeyExists(HKEY_LOCAL_MACHINE,
    'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\VDSF_is1') then
  begin
    removeOld := True;
    RegQueryStringValue(HKEY_LOCAL_MACHINE,
      'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\VDSF_is1',
      'UninstallString', strUninstall);
  end;

  // Version 0.2 and beyond.
  if RegKeyExists(HKEY_LOCAL_MACHINE,
    'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\VDSF_V0_is1') then
  begin
    RegQueryStringValue(HKEY_LOCAL_MACHINE,
      'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\VDSF_V0_is1',
      'DisplayVersion', installedVersion);


    if MsgBox('VDSF version ' + installedVersion + ' is already installed. Continue with the installation?',
      mbConfirmation, MB_YESNO) = IDYES then
    begin
      removeOld := True;
      RegQueryStringValue(HKEY_LOCAL_MACHINE,
        'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\VDSF_V0_is1',
        'UninstallString', strUninstall);
    end
    else
    begin
      Result := False;
    end;
  end;

  if removeOld = True then
  begin
    ShellExec('', strUninstall, '/SILENT', '', SW_HIDE, ewWaitUntilTerminated, errcode);
    if errcode <> 0 then
    begin
      MsgBox('Failure when attempting to uninstall the previous version - aborting the installation.',
        mbInformation, MB_OK);
      Result := False;
    end;
  end;
end;
