[Setup]
MinVersion=0,4.0
AppName=Photon
AppVerName=Photon Software version 0.5.0
AppVersion=0.5.0
AppSupportURL=http://photonsoftware.org/
AppId=PSO_V0
InfoBeforeFile=COPYING
DefaultDirName={pf}\Photonsoftware
DefaultGroupName=Photon Software
SourceDir=..\..
OutputDir=src\Packages\Output
OutputBaseFilename=photon-0.5.0

[Files]
Source: src\Release2005\photon.dll; DestDir: {app}\bin
Source: src\Release2005\photonCommon.dll; DestDir: {app}\bin
Source: src\Release2005\photoncpp.dll; DestDir: {app}\bin
Source: src\Release2005\photonNucleus.dll; DestDir: {app}\bin
Source: src\Release2005\psoInfo.exe; DestDir: {app}\bin
Source: src\Release2005\psosh.exe; DestDir: {app}\bin
Source: src\Release2005\quasar.exe; DestDir: {app}\bin
Source: src\include\photon\photon.h; DestDir: {app}\include
Source: src\include\photon\psoCommon.h; DestDir: {app}\include
Source: src\include\photon\psoErrors.h; DestDir: {app}\include
Source: src\include\photon\psoFastMap.h; DestDir: {app}\include
Source: src\include\photon\psoFolder.h; DestDir: {app}\include
Source: src\include\photon\psoHashMap.h; DestDir: {app}\include
Source: src\include\photon\psoLifo.h; DestDir: {app}\include
Source: src\include\photon\psoProcess.h; DestDir: {app}\include
Source: src\include\photon\psoQueue.h; DestDir: {app}\include
Source: src\include\photon\psoSession.h; DestDir: {app}\include
Source: src\include\photon\photon; DestDir: {app}\include
Source: src\include\photon\psoDefinition; DestDir: {app}\include
Source: src\include\photon\psoException; DestDir: {app}\include
Source: src\include\photon\psoFastMap; DestDir: {app}\include
Source: src\include\photon\psoFastMapEditor; DestDir: {app}\include
Source: src\include\photon\psoFolder; DestDir: {app}\include
Source: src\include\photon\psoHashMap; DestDir: {app}\include
Source: src\include\photon\psoLifo; DestDir: {app}\include
Source: src\include\photon\psoProcess; DestDir: {app}\include
Source: src\include\photon\psoQueue; DestDir: {app}\include
Source: src\include\photon\psoSession; DestDir: {app}\include
Source: src\Release2005\photon.lib; DestDir: {app}\lib
Source: src\Release2005\photoncpp.lib; DestDir: {app}\lib
Source: doc\refman_c.pdf; DestDir: {app}
Source: doc\refman_c++.pdf; DestDir: {app}
Source: doc\Photon_C_API.chm; DestDir: {app}
Source: doc\Photon_C++_API.chm; DestDir: {app}
Source: COPYING; DestDir: {app}; DestName: License.txt
Source: src\Examples\iso_3166.tab; DestDir: {app}\Examples
Source: src\Examples\quasar-config.example.xml; DestDir: {app}\Examples
Source: src\Examples\quasar_config.xsd; DestDir: {app}\Examples
Source: src\Examples\README.txt; DestDir: {app}\Examples
Source: src\Examples\C\Examples.sln; DestDir: {app}\Examples\C
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
Source: src\Examples\C\Project_Files\HashMap.vcproj; DestDir: {app}\Examples\C\Project_Files
Source: src\Examples\C\Project_Files\HashMapLoop.vcproj; DestDir: {app}\Examples\C\Project_Files
Source: src\Examples\C\Project_Files\QueueIn.vcproj; DestDir: {app}\Examples\C\Project_Files
Source: src\Examples\C\Project_Files\QueueOut.vcproj; DestDir: {app}\Examples\C\Project_Files
Source: src\Examples\C\Project_Files\QueueWork.vcproj; DestDir: {app}\Examples\C\Project_Files
Source: src\Examples\C++\Examples.sln; DestDir: {app}\Examples\C++
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
Source: src\Examples\C++\Project_Files\HashMap.vcproj; DestDir: {app}\Examples\C++\Project_Files
Source: src\Examples\C++\Project_Files\HashMapLoop.vcproj; DestDir: {app}\Examples\C++\Project_Files
Source: src\Examples\C++\Project_Files\QueueIn.vcproj; DestDir: {app}\Examples\C++\Project_Files
Source: src\Examples\C++\Project_Files\QueueOut.vcproj; DestDir: {app}\Examples\C++\Project_Files
Source: src\Examples\C++\Project_Files\QueueWork.vcproj; DestDir: {app}\Examples\C++\Project_Files
Source: src\Packages\SetEnv.exe; DestDir: {app}\bin

[Icons]
Name: {group}\Quasar; Filename: {app}\bin\quasar.EXE; WorkingDir: {app}
Name: {group}\Photon Shell; Filename: {app}\bin\psosh.exe; WorkingDir: {app}
Name: {group}\Ref. Manual C API; Filename: {app}\Photon_C_API.chm
Name: {group}\Ref. Manual C++ API; Filename: {app}\Photon_C++_API.chm
Name: {group}\Uninstall Photon; Filename: {uninstallexe}

[Registry]
Root: HKCU; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\quasar.exe; ValueType: string; ValueData: {app}\bin\quasar.exe; Flags: uninsdeletekeyifempty uninsdeletevalue
Root: HKCU; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\quasar.exe; ValueType: string; ValueName: Path; ValueData: {app}\bin; Flags: uninsdeletekeyifempty uninsdeletevalue

Root: HKLM; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\quasar.exe; ValueType: string; ValueData: {app}\bin\quasar.exe; Flags: uninsdeletekeyifempty uninsdeletevalue
Root: HKLM; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\quasar.exe; ValueType: string; ValueName: Path; ValueData: {app}\bin; Flags: uninsdeletekeyifempty uninsdeletevalue

Root: HKCU; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\psosh.exe; ValueType: string; ValueData: {app}\bin\psosh.exe; Flags: uninsdeletekeyifempty uninsdeletevalue
Root: HKCU; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\psosh.exe; ValueType: string; ValueName: Path; ValueData: {app}\bin; Flags: uninsdeletekeyifempty uninsdeletevalue

Root: HKLM; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\psosh.exe; ValueType: string; ValueData: {app}\bin\psosh.exe; Flags: uninsdeletekeyifempty uninsdeletevalue
Root: HKLM; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\psosh.exe; ValueType: string; ValueName: Path; ValueData: {app}\bin; Flags: uninsdeletekeyifempty uninsdeletevalue

Root: HKCU; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\psoinfo.exe; ValueType: string; ValueData: {app}\bin\psoinfo.exe; Flags: uninsdeletekeyifempty uninsdeletevalue
Root: HKCU; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\psoinfo.exe; ValueType: string; ValueName: Path; ValueData: {app}\bin; Flags: uninsdeletekeyifempty uninsdeletevalue

Root: HKLM; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\psoinfo.exe; ValueType: string; ValueData: {app}\bin\psoinfo.exe; Flags: uninsdeletekeyifempty uninsdeletevalue
Root: HKLM; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\psoinfo.exe; ValueType: string; ValueName: Path; ValueData: {app}\bin; Flags: uninsdeletekeyifempty uninsdeletevalue
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

  // Version 0.2 and 0.3.
  if RegKeyExists(HKEY_LOCAL_MACHINE,
    'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\VDSF_V0_is1') then
  begin
    RegQueryStringValue(HKEY_LOCAL_MACHINE,
      'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\VDSF_V0_is1',
      'DisplayVersion', installedVersion);


    if MsgBox('Photon version ' + installedVersion + ' is already installed. Continue with the installation?',
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

  // Version 0.4 and beyond.
  if RegKeyExists(HKEY_LOCAL_MACHINE,
    'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\PSO_V0_is1') then
  begin
    RegQueryStringValue(HKEY_LOCAL_MACHINE,
      'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\PSO_V0_is1',
      'DisplayVersion', installedVersion);


    if MsgBox('Photon version ' + installedVersion + ' is already installed. Continue with the installation?',
      mbConfirmation, MB_YESNO) = IDYES then
    begin
      removeOld := True;
      RegQueryStringValue(HKEY_LOCAL_MACHINE,
        'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\PSO_V0_is1',
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
[Components]
;Name: libxml2; Description: The libxml2 library; Types: with_libxml2
[Types]
;Name: with_libxml2; Description: Install everything including libxml2 (run-time)
;Name: without_libxml2; Description: Install everything except libxml2
[Tasks]
Name: setenv; Description: "Add Photon to your ""path"""; Components: 
[Run]
Filename: {app}\bin\SetEnv.exe; Parameters: "-ua Path %""{app}\bin"""; Flags: runminimized skipifnotsilent; Components: ; Tasks: " setenv"
[UninstallRun]
Filename: {app}\bin\SetEnv.exe; Parameters: "-ud Path %""{app}\bin"""; Flags: runminimized; Components: ; Tasks: 
