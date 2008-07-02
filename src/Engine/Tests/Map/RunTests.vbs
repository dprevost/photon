' ***********************************************************************
'
' Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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
Dim objShell
Dim objWshScriptExec
Dim fso

' List of failed tests. We append to this list when an error is encountered
' while running the tests
Dim failed_tests(56)

' Lists containing the names of the tests
' The "ok" lists are for programs which are expected to return zero (succeed)
' and the "fail" lists are for the other ones.
Dim ok_programs(14)
Dim fail_programs(41)

Dim exe_name, prog_path, program
Dim consoleMode
Dim objArgs, strArg, i
dim strOutput

' ***********************************************************************
'
' Initialization of these variables
'
' ***********************************************************************

' Populate the program lists...
ok_programs(0)  = "DeletePass"
ok_programs(1)  = "FiniPass"
ok_programs(2)  = "GetFirstPass"
ok_programs(3)  = "GetFirstWrongKeyLength"
ok_programs(4)  = "GetFirstWrongLength"
ok_programs(5)  = "GetNextPass"
ok_programs(6)  = "GetNextWrongKeyLength"
ok_programs(7)  = "GetNextWrongLength"
ok_programs(8)  = "GetPass"
ok_programs(9)  = "GetWrongLength"
ok_programs(10) = "InitPass"
ok_programs(11) = "InsertPass"
ok_programs(12) = "ReleasePass"
ok_programs(13) = "ReplacePass"
ok_programs(14) = "StatusPass"

fail_programs(0)  = "DeleteNullContext"
fail_programs(1)  = "DeleteNullHash"
fail_programs(2)  = "DeleteNullKey"
fail_programs(3)  = "DeleteZeroKeyLength"
fail_programs(4)  = "FiniNullContext"
fail_programs(5)  = "FiniNullHash"
fail_programs(6)  = "GetFirstNullContext"
fail_programs(7)  = "GetFirstNullHash"
fail_programs(8)  = "GetFirstNullItem"
fail_programs(9)  = "GetNextNullContext"
fail_programs(10) = "GetNextNullHash"
fail_programs(11) = "GetNextNullItem"
fail_programs(12) = "GetNullContext"
fail_programs(13) = "GetNullHash"
fail_programs(14) = "GetNullItem"
fail_programs(15) = "GetNullKey"
fail_programs(16) = "GetZeroKeyLength"
fail_programs(17) = "InitNullContext"
fail_programs(18) = "InitNullDefinition"
fail_programs(19) = "InitNullHash"
fail_programs(20) = "InitNullName"
fail_programs(21) = "InitNullParent"
fail_programs(22) = "InitNullStatus"
fail_programs(23) = "InitZeroBlocks"
fail_programs(24) = "InitZeroLength"
fail_programs(25) = "InsertNullContext"
fail_programs(26) = "InsertNullHash"
fail_programs(27) = "InsertNullItem"
fail_programs(28) = "InsertNullKey"
fail_programs(29) = "InsertZeroLengthItem"
fail_programs(30) = "InsertZeroLengthKey"
fail_programs(31) = "ReleaseNullContext"
fail_programs(32) = "ReleaseNullHash"
fail_programs(33) = "ReleaseNullItem"
fail_programs(34) = "ReplaceNullContext"
fail_programs(35) = "ReplaceNullHash"
fail_programs(36) = "ReplaceNullItem"
fail_programs(37) = "ReplaceNullKey"
fail_programs(38) = "ReplaceZeroLengthItem"
fail_programs(39) = "ReplaceZeroLengthKey"
fail_programs(40) = "StatusNullHash"
fail_programs(41) = "StatusNullStatus"

numTests = 57                 ' Sum of length of both arrays 
numFailed = 0

' Create the FileSystemObject
Set fso = CreateObject ("Scripting.FileSystemObject")

consoleMode = True
If Right(LCase(Wscript.FullName), 11) = "wscript.exe" Then
   consoleMode = False
End If

Set objShell = CreateObject("WScript.Shell")
verbose = False

prog_path = "Release"
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
   if LCase(strArg) = "--verbose" then 
      verbose = True
   end if
Next 

if Not consoleMode then
   wscript.echo "Be patient - running the tests in batch mode - click ok to start"
end if

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
      if verbose then 
         WScript.Stdout.Write objWshScriptExec.StdErr.ReadAll
      end if
      rc = objWshScriptExec.ExitCode
   else
      rc = objShell.Run("%comspec% /c " & Chr(34) & exe_name & Chr(34), 2, true)
   end if
   if rc <> 0 then
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
