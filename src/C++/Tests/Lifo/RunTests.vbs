' ***********************************************************************
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
' ***********************************************************************

Option Explicit

' ***********************************************************************
'
' Declaration of variables
'
' ***********************************************************************

Dim rc, numTests, numFailed, verbose, status
Dim objShell, objShellqsr
Dim objWshScriptExec
Dim fso
Dim objSocket

' List of failed tests. We append to this list when an error is encountered
' while running the tests
Dim failed_tests(8)

' Lists containing the names of the tests
' The "ok" lists are for programs which are expected to return zero (succeed)
' and the "fail" lists are for the other ones.
Dim ok_programs(8)

Dim exe_name, prog_path, program, dll_path, qsr_path, tmpDir, cmdFile, exeName
Dim consoleMode
Dim objArgs, strArg, i
dim strOutput

' ***********************************************************************
'
' Initialization of these variables
'
' ***********************************************************************

' Populate the program lists...
ok_programs(0) = "Close"
ok_programs(1) = "Definition"
ok_programs(2) = "FieldDefODBC"
ok_programs(3) = "GetFirst"
ok_programs(4) = "GetNext"
ok_programs(5) = "Open"
ok_programs(6) = "Pop"
ok_programs(7) = "Push"
ok_programs(8) = "Status"

numTests  =  9                
numFailed =  0

' Create the FileSystemObject
Set fso = CreateObject ("Scripting.FileSystemObject")

consoleMode = True
If Right(LCase(Wscript.FullName), 11) = "wscript.exe" Then
   consoleMode = False
End If

Set objShell = CreateObject("WScript.Shell")
Set objShellqsr = CreateObject("WScript.Shell")
verbose = False

prog_path = "Release"
qsr_path = "..\..\..\Release2005"
Set objArgs = WScript.Arguments

' ***********************************************************************
'
' Start the program
'
' ***********************************************************************

For Each strArg in objArgs
   If Left(LCase(strArg), 6) = "--path" AND Len(strArg) > 6 Then
      prog_path = Right(strArg, Len(strArg)-7)
   end if
   If Left(LCase(strArg), 9) = "--dllpath" AND Len(strArg) > 9 Then
      dll_path = Right(strArg, Len(strArg)-10)
   end if
   If Left(LCase(strArg), 6) = "--qsrpath" AND Len(strArg) > 8 Then
      qsr_path = Right(strArg, Len(strArg)-9)
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

' --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
'
' Start the server (reusing the same one for all the tests in this sub-dir).
'
' --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

tmpDir = objShell.Environment.item("TMP")
tmpDir = objShell.ExpandEnvironmentStrings(tmpDir)
tmpDir = tmpDir + "\photon_queue"

if (fso.FolderExists(tmpDir)) Then
   fso.DeleteFolder(tmpDir)
end if

fso.CreateFolder(tmpDir)

fso.Copyfile "..\..\..\XML\\quasar_config.xsd", tmpDir + "\quasar_config.xsd"

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

exeName = qsr_path + "\quasar.exe -c " + tmpDir + "\cfg.xml"

objShellqsr.Run "%comspec% /c title quasar | " & exeName, 2, false

'Turn on error handling
On Error Resume Next
set objSocket=CreateObject("MSWinsock.Winsock.1")
'Catch error - likely cause: no mswinsck.ocx
If Err.Number = 0 Then
   On Error GoTo 0 
   objSocket.Protocol=0 ' TCP
   Call objSocket.Close
   Call objSocket.Connect ("127.0.0.1",10701)
   while ((objSocket.State=6) or (objSocket.State=3)) 'socket state <> connecting or connection pending
      'do nothing
      Wscript.Sleep 10
      'WScript.Echo objSocket.State
   wend
   if(objSocket.State=7) then ' if socket connected
      'WScript.Echo "Port Open"
   elseif(objSocket.State=9)then' If Error
      WScript.Echo "Connection error"
      wscript.quit(1)
   elseif(objSocket.State=0)then 'Closed
      WScript.Echo "Connection refused"
      wscript.quit(1)
   end if
   call objSocket.Close
   set objSocket = nothing
else
   Err.Clear
   On Error GoTo 0 
   Wscript.Sleep 2000
end if

' Run all Programs

For Each program in ok_programs
   exe_name = prog_path & "\" & program & ".exe"
   if consoleMode then 
      WScript.Echo "Running " & exe_name
      Set objWshScriptExec = objShell.Exec("%comspec% /c " & Chr(34) & exe_name & Chr(34))
      status = objWshScriptExec.Status
      Do While objWshScriptExec.Status = 0
         WScript.Sleep 100
      Loop
      strOutput = objWshScriptExec.StdOut.ReadAll
'      if verbose then 
         WScript.Stdout.Write objWshScriptExec.StdErr.ReadAll
'      end if
      rc = objWshScriptExec.ExitCode
   else
      rc = objShell.Run("%comspec% /c " & Chr(34) & exe_name & Chr(34), 2, true)
   end if
   if rc <> 0 then   
   wscript.echo "rc = " & rc & " " & program
      failed_tests(numFailed) = program
      numFailed = numFailed + 1
   end if
Next

dim z
z = false
while z <> true 
   z = objShellqsr.AppActivate( "quasar" )
   Wscript.Sleep 100
wend
objShellqsr.SendKeys "^C"

if consoleMode then
   WScript.StdOut.Write vbcrlf & "Total number of tests: " & numTests & vbcrlf
   WScript.StdOut.Write "Total number of failed tests: " & numFailed & vbcrlf & vbcrlf
   For i = 1 to numFailed 
      WScript.StdOut.Write "This test failed: " & failed_tests(i-1) & vbcrlf
   Next
   if verbose then
      WScript.StdOut.Write "Type return to continue"
      Dim dummy
      dummy = WScript.StdIn.Read(1)
   end if
else                                 
   wscript.echo "Total number of tests: " & numTests & vbcrlf & _
      "Total number of failed tests: " & numFailed
end if

if (fso.FolderExists(tmpDir)) Then
   Wscript.Sleep 1000
   On Error Resume Next
   fso.DeleteFolder(tmpDir)
end if
if numFailed > 0 then wscript.quit(1)
wscript.quit(0)

