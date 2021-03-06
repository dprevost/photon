' --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
'
' Copyright (C) 2008-2009 Daniel Prevost <dprevost@photonsoftware.org>
' 
' This file is part of Photon (photonsoftware.org).
' 
' This file may be distributed and/or modified under the terms of the
' GNU General Public License version 2 or version 3 as published by the 
' Free Software Foundation and appearing in the file COPYING.GPL2 and 
' COPYING.GPL3 included in the packaging of this software.
'
' Licensees holding a valid Photon Commercial license can use this file 
' in accordance with the terms of their license.
' 
' This software is distributed in the hope that it will be useful,
' but WITHOUT ANY WARRANTY; without even the implied warranty of
' MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
'
' --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
'
' The tests in this script are pretty basic (checking arguments, config
' parameters, etc.).
'
' --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

Option Explicit

' --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
'
' Declaration of variables
'
' --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

Dim consoleMode, verbose, status, rc
Dim tmpDir
Dim objArgs, strArg, i
Dim exeName
Dim objWshScriptExec
Dim objShell
Dim fso
Dim cmdFile
Dim dll_path

' --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
'
' Initialization of these variables
'
' --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

consoleMode = True
If Right(LCase(Wscript.FullName), 11) = "wscript.exe" Then
   consoleMode = False
End If

verbose = False
Set objArgs = WScript.Arguments
Set objShell = CreateObject("WScript.Shell")

' Create the FileSystemObject
Set fso = CreateObject ("Scripting.FileSystemObject")

tmpDir = objShell.Environment.item("TMP")
tmpDir = objShell.ExpandEnvironmentStrings(tmpDir)
tmpDir = tmpDir + "\quasar"

' --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
'
' Start the program
'
' --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

For Each strArg in objArgs
   If Left(LCase(strArg), 6) = "--path" AND Len(strArg) > 6 Then
      dll_path = Right(strArg, Len(strArg)-7)
   end if
   If Left(LCase(strArg), 9) = "--dllpath" AND Len(strArg) > 9 Then
      dll_path = Right(strArg, Len(strArg)-10)
   end if
   if LCase(strArg) = "--verbose" then 
      verbose = True
   end if
Next 

dim WshEnv
Set WshEnv = objShell.Environment("Process") 
WshEnv("Path") = WshEnv("Path") & ";" & fso.GetAbsolutePathName(dll_path)

if Not consoleMode then
   wscript.echo "Be patient - running the tests in batch mode - click ok to start"
end if

exeName = dll_path & "\" & "quasar.exe -z"
if consoleMode then 
   WScript.Echo "Running " & exeName
   Set objWshScriptExec = objShell.Exec("%comspec% /c " & Chr(34) & exeName & Chr(34))
   status = objWshScriptExec.Status
   Do While objWshScriptExec.Status = 0
      WScript.Sleep 100
   Loop
   if verbose then 
      WScript.Stdout.Write objWshScriptExec.StdErr.ReadAll
   end if
   rc = objWshScriptExec.ExitCode
else
   rc = objShell.Run("%comspec% /c " & Chr(34) & exeName & Chr(34), 2, true)
end if
if rc = 0 then
   WScript.Echo "Test failed!"
   wscript.quit(1)
end if

if (fso.FolderExists(tmpDir)) Then
   fso.DeleteFolder(tmpDir)
end if
fso.CreateFolder(tmpDir)

' Run it with a missing config file
exeName = dll_path & "\" & "quasar.exe -c " & tmpDir & "\cfg.xml"
if consoleMode then 
   WScript.Echo "Running " & exeName
   Set objWshScriptExec = objShell.Exec("%comspec% /c " & Chr(34) & exeName & Chr(34))
   status = objWshScriptExec.Status
   Do While objWshScriptExec.Status = 0
      WScript.Sleep 100
   Loop
   if verbose then 
      WScript.Stdout.Write objWshScriptExec.StdErr.ReadAll
   end if
   rc = objWshScriptExec.ExitCode
else
   rc = objShell.Run("%comspec% /c " & Chr(34) & exeName & Chr(34), 2, true)
end if
if rc = 0 then
   WScript.Echo "Test failed!"
   wscript.quit(1)
end if

' Run it with an empty file
Set cmdFile = fso.CreateTextFile(tmpDir + "\cfg.xml", True)
cmdFile.Close
exeName = dll_path & "\" & "quasar.exe -c " & tmpDir & "\cfg.xml"
if consoleMode then 
   WScript.Echo "Running " & exeName
   Set objWshScriptExec = objShell.Exec("%comspec% /c " & Chr(34) & exeName & Chr(34))
   status = objWshScriptExec.Status
   Do While objWshScriptExec.Status = 0
      WScript.Sleep 100
   Loop
   if verbose then 
      WScript.Stdout.Write objWshScriptExec.StdErr.ReadAll
   end if
   rc = objWshScriptExec.ExitCode
else
   rc = objShell.Run("%comspec% /c " & Chr(34) & exeName & Chr(34), 2, true)
end if
if rc = 0 then
   WScript.Echo "Test failed!"
   wscript.quit(1)
end if

fso.Copyfile "..\..\XML\quasar_config.xsd", tmpDir + "\quasar_config.xsd"

Set cmdFile = fso.CreateTextFile(tmpDir + "\cfg.xml", True)
cmdFile.WriteLine("<?xml version=""1.0""?>")
cmdFile.WriteLine("<quasar_config xmlns=""http://photonsoftware.org/quasarConfig""")
cmdFile.WriteLine("xmlns:xsi=""http://www.w3.org/2001/XMLSchema-instance""")
cmdFile.WriteLine("xsi:schemaLocation=""http://photonsoftware.org/quasarConfig " + tmpDir + "\quasar_config.xsd""> ")
cmdFile.WriteLine("  <mem_location>" + tmpDir + "</mem_location>")
cmdFile.WriteLine("  <mem_size size=""10240"" units=""kb"" />")
'cmdFile.WriteLine("  <quasar_address>10701</quasar_address>")
cmdFile.WriteLine("  <file_access access=""group"" />")
cmdFile.WriteLine("</quasar_config>")
cmdFile.Close

exeName = dll_path & "\" & "quasar.exe --test -c " & tmpDir & "\cfg.xml"
if consoleMode then 
   WScript.Echo "Running " & exeName
   Set objWshScriptExec = objShell.Exec("%comspec% /c " & Chr(34) & exeName & Chr(34))
   status = objWshScriptExec.Status
   Do While objWshScriptExec.Status = 0
      WScript.Sleep 100
   Loop
   if verbose then 
      WScript.Stdout.Write objWshScriptExec.StdErr.ReadAll
   end if
   rc = objWshScriptExec.ExitCode
else
   rc = objShell.Run("%comspec% /c " & Chr(34) & exeName & Chr(34), 2, true)
end if
if rc = 0 then
   WScript.Echo "Test failed!"
   wscript.quit(1)
end if

Set cmdFile = fso.CreateTextFile(tmpDir + "\cfg.xml", True)
cmdFile.WriteLine("<?xml version=""1.0""?>")
cmdFile.WriteLine("<quasar_config xmlns=""http://photonsoftware.org/quasarConfig""")
cmdFile.WriteLine("xmlns:xsi=""http://www.w3.org/2001/XMLSchema-instance""")
cmdFile.WriteLine("xsi:schemaLocation=""http://photonsoftware.org/quasarConfig " + tmpDir + "\quasar_config.xsd""> ")
cmdFile.WriteLine("  <mem_location>" + tmpDir + "</mem_location>")
cmdFile.WriteLine("  <mem_size size=""10240"" units=""kb"" />")
cmdFile.WriteLine("  <quasar_address>10701</quasar_address>")
cmdFile.WriteLine("  <file_access access=""group"" />")
cmdFile.WriteLine("</quasar_config>")
cmdFile.Close

exeName = dll_path & "\" & "quasar.exe --test -c " & tmpDir & "\cfg.xml"
if consoleMode then 
   WScript.Echo "Running " & exeName
   Set objWshScriptExec = objShell.Exec("%comspec% /c " & Chr(34) & exeName & Chr(34))
   status = objWshScriptExec.Status
   Do While objWshScriptExec.Status = 0
      WScript.Sleep 100
   Loop
   if verbose then 
      WScript.Stdout.Write objWshScriptExec.StdErr.ReadAll
   end if
   rc = objWshScriptExec.ExitCode
else
   rc = objShell.Run("%comspec% /c " & Chr(34) & exeName & Chr(34), 2, true)
end if
if rc <> 0 then
   WScript.Echo "Test failed!"
   wscript.quit(1)
end if

if (fso.FolderExists(tmpDir)) Then
   On Error Resume Next
   fso.DeleteFolder(tmpDir)
end if
WScript.Echo "Tests passed!"
wscript.quit(0)






