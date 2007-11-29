' ***********************************************************************
'
' Copyright (C) 2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

' List of failed tests. We append to this list when an error is encountered
' while running the tests
Dim failed_tests(42)

' Lists containing the names of the tests
' The "ok" lists are for programs which are expected to return zero (succeed)
' and the "fail" lists are for the other ones.
Dim ok_programs(30)
Dim fail_programs(11)

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
ok_programs(0)  = "CloseNullHandle"
ok_programs(1)  = "ClosePass"
ok_programs(2)  = "CloseWrongHandle"
ok_programs(3)  = "DeleteNullHandle"
ok_programs(4)  = "DeleteNullKey"
ok_programs(5)  = "DeletePass"
ok_programs(6)  = "DeleteWrongHandle"
ok_programs(7)  = "DeleteZeroLength"
ok_programs(8)  = "FirstPass"
ok_programs(9)  = "Get"
ok_programs(10) = "GetFirst"
ok_programs(11) = "GetNext"
ok_programs(12) = "InsertNullData"
ok_programs(13) = "InsertNullHandle"
ok_programs(14) = "InsertNullKey"
ok_programs(15) = "InsertPass"
ok_programs(16) = "InsertWrongHandle"
ok_programs(17) = "InsertZeroDataLength"
ok_programs(18) = "InsertZeroKeyLength"
ok_programs(19) = "NextPass"
ok_programs(20) = "OpenNullName"
ok_programs(21) = "OpenNullObjHandle"
ok_programs(22) = "OpenNullSessHandle"
ok_programs(23) = "OpenPass"
ok_programs(24) = "OpenWrongSessHandle"
ok_programs(25) = "OpenZeroLength"
ok_programs(26) = "Replace"
ok_programs(27) = "RetrievePass"
ok_programs(28) = "StatusNullHandle"
ok_programs(29) = "StatusNullStatus"
ok_programs(30) = "StatusPass"

fail_programs(0)  = "FirstNullEntry"
fail_programs(1)  = "FirstNullHandle"
fail_programs(2)  = "FirstWrongHandle"
fail_programs(3)  = "NextNoFirst"
fail_programs(4)  = "NextNullEntry"
fail_programs(5)  = "NextNullHandle"
fail_programs(6)  = "NextWrongHandle"
fail_programs(7)  = "RetrieveNullEntry"
fail_programs(8)  = "RetrieveNullHandle"
fail_programs(9)  = "RetrieveNullKey"
fail_programs(10) = "RetrieveWrongHandle"
fail_programs(11) = "RetrieveZeroLength"

numTests  = 43                 ' Sum of length of both arrays 
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

Set cmdFile = fso.CreateTextFile(tmpDir + "\cfg.txt", True)
cmdFile.WriteLine("# VDSF Config file             ")
cmdFile.WriteLine("#                              ")
cmdFile.WriteLine("VDSLocation           " + tmpDir)
cmdFile.WriteLine("#MemorySize is in kbytes       ")
cmdFile.WriteLine("MemorySize            10000    ")
cmdFile.WriteLine("WatchdogAddress       10701    ")
cmdFile.WriteLine("LogTransaction        0        ")
cmdFile.WriteLine("FilePermissions       0660     ")
cmdFile.WriteLine("DirectoryPermissions  0770     ")

exeName = wd_path + "\vdswd.exe -c " + tmpDir + "\cfg.txt"

objShellwd.Run "%comspec% /c title vdswd | " & exeName, 2, false
'objShellwd.Run "%comspec% /k " & Chr(34) & exeName & Chr(34), 4, false
Wscript.Sleep 1000

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

objShellwd.AppActivate "vdswd"
Wscript.Sleep 1000
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

if numFailed > 0 then wscript.quit(1)
wscript.quit(0)

