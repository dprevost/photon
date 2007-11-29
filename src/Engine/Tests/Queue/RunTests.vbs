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
Dim objShell
Dim objWshScriptExec
Dim fso

' List of failed tests. We append to this list when an error is encountered
' while running the tests
Dim failed_tests(53)

' Lists containing the names of the tests
' The "ok" lists are for programs which are expected to return zero (succeed)
' and the "fail" lists are for the other ones.
Dim ok_programs(11)
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
ok_programs(0)  = "CommitAddPass"
ok_programs(1)  = "CommitRemovePass"
ok_programs(2)  = "FiniPass"
ok_programs(3)  = "GetPass"
ok_programs(4)  = "InitPass"
ok_programs(5)  = "InsertPass"
ok_programs(6)  = "ReleaseNoLockPass"
ok_programs(7)  = "ReleasePass"
ok_programs(8)  = "RemovePass"
ok_programs(9)  = "RollbackAddPass"
ok_programs(10) = "RollbackRemovePass"
ok_programs(11) = "StatusPass"

fail_programs(0)  = "CommitAddNullOffset"
fail_programs(1)  = "CommitAddNullQueue"
fail_programs(2)  = "CommitRemoveNullContext"
fail_programs(3)  = "CommitRemoveNullOffset"
fail_programs(4)  = "CommitRemoveNullQueue"
fail_programs(5)  = "FiniNullContext"
fail_programs(6)  = "FiniNullQueue"
fail_programs(7)  = "GetNullContext"
fail_programs(8)  = "GetNullQueue"
fail_programs(9)  = "GetNullIterator"
fail_programs(10) = "GetWrongFlag"
fail_programs(11) = "GetWrongLength"
fail_programs(12) = "InitNullContext"
fail_programs(13) = "InitNullQueue"
fail_programs(14) = "InitNullName"
fail_programs(15) = "InitNullParent"
fail_programs(16) = "InitNullStatus"
fail_programs(17) = "InitZeroBlocks"
fail_programs(18) = "InitZeroLength"
fail_programs(19) = "InsertNullContext"
fail_programs(20) = "InsertNullQueue"
fail_programs(21) = "InsertNullItem"
fail_programs(22) = "InsertWrongFlag"
fail_programs(23) = "InsertZeroLengthItem"
fail_programs(24) = "ReleaseNullContext"
fail_programs(25) = "ReleaseNullItem"
fail_programs(26) = "ReleaseNullQueue"
fail_programs(27) = "ReleaseNoLockNullContext"
fail_programs(28) = "ReleaseNoLockNullItem"
fail_programs(29) = "ReleaseNoLockNullQueue"
fail_programs(30) = "RemoveNullContext"
fail_programs(31) = "RemoveNullItem"
fail_programs(32) = "RemoveNullQueue"
fail_programs(33) = "RemoveWrongFlag"
fail_programs(34) = "RemoveWrongLength"
fail_programs(35) = "RollbackAddNullContext"
fail_programs(36) = "RollbackAddNullOffset"
fail_programs(37) = "RollbackAddNullQueue"
fail_programs(38) = "RollbackRemoveNullOffset"
fail_programs(39) = "RollbackRemoveNullQueue"
fail_programs(40) = "StatusNullQueue"
fail_programs(41) = "StatusNullStatus"

numTests = 54                 ' Sum of length of both arrays 
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

