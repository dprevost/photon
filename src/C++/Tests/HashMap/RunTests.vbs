' ***********************************************************************
'
' Copyright (C) 2008 Daniel Prevost <dprevost@users.sourceforge.net>
' 
' This file is part of vdsf (Virtual Data Space Framework).
' 
' This file may be distributed and/or modified under the terms of the
' GNU General Public License version 2 as published by the Free Software
' Foundation and appearing in the file COPYING included in the
' packaging of this library.
' 
' This file is distributed in the hope that it will be useful,
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
Dim objShell, objShellwd
Dim objWshScriptExec
Dim fso
Dim objSocket

' List of failed tests. We append to this list when an error is encountered
' while running the tests
Dim failed_tests(8)

' Lists containing the names of the tests
' The "ok" lists are for programs which are expected to return zero (succeed)
' and the "fail" lists are for the other ones.
Dim ok_programs(9)

Dim exe_name, prog_path, program, wd_path, tmpDir, cmdFile, exeName
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
ok_programs(1) = "Delete"
ok_programs(2) = "Get"
ok_programs(3) = "GetFirst"
ok_programs(4) = "GetNext"
ok_programs(5) = "Insert"
ok_programs(6) = "Open"
ok_programs(7) = "Replace"
ok_programs(8) = "Status"

numTests  =  9                 ' Sum of length of both arrays 
numFailed =  0

' Create the FileSystemObject
Set fso = CreateObject ("Scripting.FileSystemObject")

consoleMode = True
If Right(LCase(Wscript.FullName), 11) = "wscript.exe" Then
   consoleMode = False
End If

Set objShell = CreateObject("WScript.Shell")
Set objShellwd = CreateObject("WScript.Shell")
verbose = False

prog_path = "Release"
wd_path = "..\..\..\Release"
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
   If Left(LCase(strArg), 6) = "--wdpath" AND Len(strArg) > 8 Then
      wd_path = Right(strArg, Len(strArg)-9)
   end if
   if LCase(strArg) = "--verbose" then 
      verbose = True
   end if
Next 

if Not consoleMode then
   wscript.echo "Be patient - running the tests in batch mode - click ok to start"
end if

' --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
'
' Start the watchdog (the same wd for all the test in this sub-dir).
'
' --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

tmpDir = objShell.Environment.item("TMP")
tmpDir = objShell.ExpandEnvironmentStrings(tmpDir)
tmpDir = tmpDir + "\vdsf_hash"

if (fso.FolderExists(tmpDir)) Then
   fso.DeleteFolder(tmpDir)
end if

fso.CreateFolder(tmpDir)

fso.Copyfile "..\..\..\XML\\wd_config.xsd", tmpDir + "\wd_config.xsd"

Set cmdFile = fso.CreateTextFile(tmpDir + "\cfg.xml", True)
cmdFile.WriteLine("<?xml version=""1.0""?>")
cmdFile.WriteLine("<vdsf_config xmlns=""http://vdsf.sourceforge.net/Config""")
cmdFile.WriteLine("xmlns:xsi=""http://www.w3.org/2001/XMLSchema-instance""")
cmdFile.WriteLine("xsi:schemaLocation=""http://vdsf.sourceforge.net/Config " + tmpDir + "\wd_config.xsd""> ")
cmdFile.WriteLine("  <vds_location>" + tmpDir + "</vds_location>")
cmdFile.WriteLine("  <mem_size size=""10240"" units=""kb"" />")
cmdFile.WriteLine("  <watchdog_address>10701</watchdog_address>")
cmdFile.WriteLine("  <file_access access=""group"" />")
cmdFile.WriteLine("</vdsf_config>")
cmdFile.Close

exeName = wd_path + "\vdswd.exe -c " + tmpDir + "\cfg.xml"

objShellwd.Run "%comspec% /c title vdswd | " & exeName, 2, false

'Turn on error handling
On Error Resume Next
set objSocket=CreateObject("MSWinsock.Winsock.1")
'Catch error - likely cause: nomswinsck.ocx
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
For Each program in fail_programs
   exe_name = prog_path & "\" & program & ".exe"
   if consoleMode then 
      WScript.Echo "Running " & exe_name
      Set objWshScriptExec = objShell.Exec("%comspec% /c " & Chr(34) & exe_name & Chr(34))
      status = objWshScriptExec.Status
      Do While objWshScriptExec.Status = 0
         WScript.Sleep 100
      Loop
      strOutput = objWshScriptExec.StdOut.ReadAll
      if verbose then 
         WScript.Stdout.Write objWshScriptExec.StdErr.ReadAll
      end if
      rc = objWshScriptExec.ExitCode
   else
      rc = objShell.Run("%comspec% /c " & Chr(34) & exe_name & Chr(34), 2, true)
   end if
   if rc = 0 then
      failed_tests(numFailed) = program
      numFailed = numFailed + 1
   end if
Next

dim z
z = false
while z <> true 
   z = objShellwd.AppActivate( "vdswd" )
   Wscript.Sleep 100
wend
objShellwd.SendKeys "^C"

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
